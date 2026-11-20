#!/usr/bin/env python3

import re
import sys
import json
import os.path
from functools import reduce

re_ast_node = re.compile(f'(abstract[ \t\n]*)?struct[ \t\n]*([a-zA-Z_][a-zA-Z_0-9]*)[ \t\n]*\\{{');
re_ast_section = re.compile(f'^BEGIN AST\n.*?\nEND AST\n', re.DOTALL | re.MULTILINE)
re_block_delimiters = re.compile('[{};]');
re_field = re.compile('(related[ \t\n]+)?(?:(dpa_u_list)\\(([^)]*)\\)|([a-zA-Z_][a-zA-Z_0-9]*\\*?))[ \t\n]*([a-zA-Z_][a-zA-Z_0-9]*)[ \t\n]*(?:\\[([^\\]]*)\\])?$');
re_comment = re.compile('(?://|#)[^\n]*|/\\*.*?\\*/', re.DOTALL)

def parse_ast_header(content):
  content = re_comment.sub('', content)
  content = re_ast_section.search(content)[0]
  nodes = dict()

  pos=0
  while True:
    match = re_ast_node.search(content, pos)
    if not match:
      break
    pos = match.end()
    modifier, name = match.groups()
    modifier = modifier.split() if modifier else []
    last = pos
    fields = {}
    stack = []
    it_delim = re_block_delimiters.finditer(content, pos)
    for m_delim in it_delim:
      delim = m_delim.group()
      if delim == '{':
        stack.append(fields)
        fields = {}
      elif delim == '}':
        if len(stack) == 0:
          break
        up = stack.pop()
        last = m_delim.end()
        m_delim = next(it_delim)
        end = m_delim.end()
        assert(m_delim.group() == ';')
        fname = content[last:end-1].strip()
        for ffname, field in fields.items():
          up[fname+'.'+ffname] = field
        fields = up
      else:
        end = m_delim.end()
        entry = content[last:end-1].strip()
        last = end
        parts = re_field.search(entry)
        if not parts:
          continue
        parts = parts.groups()
        is_related = parts[0] != None
        is_list = parts[1] != None
        typ = parts[2] or parts[3]
        if not typ.startswith(type_prefix):
          continue
        typ = typ[len(type_prefix):]
        fname = parts[4]
        count = int(parts[5] or 1)
        fields[fname] = {
          "name": fname,
          "is_list": is_list,
          "is_related": is_related,
          "type": typ,
          "count": count,
        }
    if name.startswith(type_prefix):
      name = name[len(type_prefix):]
      nodes[name] = {
        "kind": 'ast_node',
        "is_abstract": 'abstract' in modifier,
        "name": name,
        "fields": fields,
        "super": None,
        "derived": [],
      }
    pos = last
  for t in nodes.values():
    for k, f in [*t['fields'].items()]:
      typ = f["type"]
      isptr = typ[-1] == '*'
      if isptr:
        typ = typ[:-1].strip()
      if typ not in nodes:
        del t['fields'][k]
        continue
      if not isptr:
        if t['super']:
          raise Exception("Multiple inherritance not implemented")
        t['super'] = t['fields'][k]
        del t['fields'][k]
        nodes[typ]['derived'].append(t['name'])
    if not t['super'] and t['name'] != 'AST':
      raise Exception(type_prefix+t['name']+": All AST node types must ultimately be derived from the `AST` node type");
  return nodes

def ast_walk_types(node_types, t='AST', edge=False):
  current = node_types[t]
  yield current
  for t in current['derived']:
    yield from ast_walk_types(node_types, t, edge)
  if edge:
    yield {"kind": 'end', "value": current}

def get_fields(node_types, typ):
  prefix = ''
  result = []
  while True:
    fields = {}
    for k,f in typ['fields'].items():
      fields[prefix+k] = f
    result.append(fields)
    if not typ['super']:
      break
    prefix += typ['super']['name'] + '.'
    typ = node_types[typ['super']['type']]
  result = reversed(result)
  result = reduce(lambda a, b: a | b, result)
  return result

type_prefix = "AST_"

warning_generated = """\
// DO NOT CHANGE! This is a generated file, changes will be overwritten. Change ast.h.in or gen_ast.py instead.
"""

if __name__ == '__main__':
  basedir = os.path.dirname(__file__)
  with open(os.path.join(basedir, "include/ast.h.in"), 'r', encoding='utf-8') as f:
    content = f.read()
  node_types = parse_ast_header(content)

  begin_ast = '// <AST_BEGIN>\n\n'
  begin_ast += 'enum AST_node_type {\n'
  i = 0
  for typ in ast_walk_types(node_types, 'AST', True):
    if typ['kind'] == 'end':
      typ = typ['value']
      begin_ast += f'  AST_E_{typ['name']} = {i-1},\n'
    else:
      begin_ast += f'  AST_S_{typ['name']} = {i},\n'
      if typ['kind'] == 'ast_node' and not typ['is_abstract']:
        begin_ast += f'  AST_T_{typ['name']} = {i},\n'
        i += 1
  begin_ast += '};\n'

  for typ in ast_walk_types(node_types, 'AST'):
    begin_ast += f'typedef struct {type_prefix}{typ['name']} {type_prefix}{typ['name']};\n'
  begin_ast += '\n// </AST_BEGIN>\n'

  end_ast = '// <AST_END>\n\n'
  for typ in node_types.values():
    if not typ["super"]:
      continue
    end_ast += f'static_assert(offsetof({type_prefix}{typ['name']}, {typ['super']['name']}) == 0, "{type_prefix}{typ['name']}::{typ['super']['name']} must be the first member!");\n'
  end_ast += '\n// </AST_END>\n'

  c_source = warning_generated
  c_source += f'#include <ast.h>\n\n'
  c_source += f'const struct AST_M_type ast_type_meta[] = {{\n'
  for typ in node_types.values():
    if typ['kind'] == 'ast_node' and not typ['is_abstract']:
      c_source += f'  [AST_T_{typ['name']}] = {{\n'
      fields = get_fields(node_types, typ)
      children = {k:v for k,v in fields.items() if not v['is_related']}
      c_source += f'    .name = {json.dumps(typ['name'])},\n'
      if len(children):
        c_source += f'    .children = (const struct AST_M_child[]){{\n'
        for member,f in children.items():
          c_source += f'      {{\n'
          c_source += f'        .name = "{member}",\n'
          c_source += f'        .offset = offsetof({type_prefix}{typ['name']}, {member}),\n'
          c_source += f'        .count = {f['count']},\n'
          c_source += f'        .is_list = {+f['is_list']},\n'
          c_source += f'      }},\n'
        c_source += f'    }},\n'
        c_source += f'    .children_count = {len(children)},\n'
      c_source += f'    .size = sizeof({type_prefix}{typ['name']}),\n'
      c_source += f'  }},\n'
  c_source += f'}};\n'

  header_source = (warning_generated + content) \
                    .replace("BEGIN AST", begin_ast) \
                    .replace("END AST", end_ast)

  with open(os.path.join(basedir, "include/ast.h"), 'w', encoding='utf-8') as f:
    f.write(header_source)
  with open(os.path.join(basedir, "src/gen/ast.c"), 'w', encoding='utf-8') as f:
    f.write(c_source)
