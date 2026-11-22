#include <parser.h>
#include <tokenizer.h>
#include <ast.h>
#include <dpa/utils/map.h>

static void* AST_new_g(enum AST_node_type type, AST_AST* parent){
  const AST_M_type*const restrict ast_meta = &ast_type_meta[type];
  // fprintf(stderr, "type: %d size: %d\n", type, ast_meta->size);
  AST_AST* ast = calloc(1, ast_meta->size);
  ast->type = type;
  ast->parent = parent;
  return ast;
}
#define AST_new(NAME, parent) (AST_##NAME*)AST_new_g(AST_T_##NAME, parent)

enum visibility {
  VISIBILITY_EXTERNAL = 1,
  VISIBILITY_GLOBAL,
  VISIBILITY_MODULE,
  VISIBILITY_LOCAL,
};

struct scope {
  dpa_u_map_string_t entry_map; // struct scope_entry*
};

enum scop_entry_type {
  SCOPE_ENTRY_NAMED_CONSTANT,
};

struct scope_entry {
  dpa_u_a_bo_unique_t name;
};

static inline bool token_is_identifier(const struct token*restrict token, dpa_u_a_bo_unique_t bo){
  if(token->type != TOKEN_IDENTIFIER)
    return false;
  return dpa_u_bo_is_equal(((const struct token_string*)token)->value, bo);
}

static inline bool token_is_operator(const struct token*restrict token, dpa_u_a_bo_unique_t bo){
  if(token->type != TOKEN_OPERATOR)
    return false;
  return dpa_u_bo_is_equal(((const struct token_string*)token)->value, bo);
}



struct operator_parser_meta {
  // 0 is reserved for none. A prefix has no left precedence, a suffix no right precedence.
  // The first bit is used for the direction in case of same precedence, odd numbers are right to left.
  unsigned precedence[2];
  enum AST_node_type ast_type;
};

struct operator_parser_meta_list {
  int count;
  const struct operator_parser_meta* list;
};


const struct operator_parser_meta_list opmeta[] = {
  [TOKEN_OP_MUL] = { // *
    .count = 2,
    .list = (const struct operator_parser_meta[]){
      {{0x0900,0x0900}, AST_T_op_multiply}, // a * b
      {{0x0100,0x0000}, AST_T_op_ref}, // type*
    },
  },
  [TOKEN_OP_META] = { // @
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0100,0x0000}, AST_T_op_meta},
    },
  },
  [TOKEN_OP_AND] = { // &
    .count = 2,
    .list = (const struct operator_parser_meta[]){
      {{0x0C00,0x0C00}, AST_T_op_binary_and}, // a & b
      {{0x0100,0x0000}, AST_T_op_copy      }, // a&
    },
  },
  [TOKEN_OP_DOT] = { // .
    // This one is handled in a special way. Things like `value.member.member` or `value...member.member.` are put in
    // this temporary AST node. That is because it could also be `type. type. value`, for example, the `.` removing an
    // indirrection from a pointer type before the value is cast to it. It will later be translated into the proper AST,
    // node when what the identifiers refer to has been resolved. Crucially, the structure of the resulting tree is
    // very different depending on what the constituents turn out to be, which is why we can't break it don wurrther at
    // this point.
    .count = 2,
    .list = (const struct operator_parser_meta[]){
      {{0x0100,0x0000}, AST_T_incomplete_member_or_deref_cast},
      {{0x0100,0x0000}, AST_T_op_deref},
    },
  },
  [TOKEN_OP_DEFINE] = { // =
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0401,0x1201}, AST_T_op_assignment_normal},
    },
  },
  [TOKEN_OP_ASSIGN] = { // :=
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0401,0x0D01}, AST_T_op_assignment_inline},
    },
  },
  [TOKEN_OP_POWER] = { // **
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0601,0x0601}, AST_T_op_power}, // a**b
    },
  },
  [TOKEN_OP_MINUS] = { // -
    .count = 2,
    .list = (const struct operator_parser_meta[]){
      {{0x0A00,0x0A00}, AST_T_op_minus}, // a - b
      {{0x0000,0x0701}, AST_T_op_negate}, // -x
    },
  },
  [TOKEN_OP_NOT] = { // ~
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0000,0x0701}, AST_T_op_invert}, // ~x
    },
  },
  [TOKEN_OP_LOGIC_NOT] = { // !
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0000,0x0701}, AST_T_op_not}, // !x
    },
  },
  [TOKEN_OP_SHIFT_LEFT] = { // <<
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0B00,0x0800}, AST_T_op_bitshift_left}, // a << b
    },
  },
  [TOKEN_OP_SHIFT_RIGHT] = { // >>
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0B00,0x0800}, AST_T_op_bitshift_right}, // a >> b
    },
  },
  [TOKEN_OP_DIV] = { // /
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0900,0x0900}, AST_T_op_divide}, // a / b
    },
  },
  [TOKEN_OP_MOD] = { // %
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0900,0x0900}, AST_T_op_reminder}, // a % b
    },
  },
  [TOKEN_OP_DIVMOD] = { // /%
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0900,0x0900}, AST_T_op_divide_reminder}, // a /% b
    },
  },
  [TOKEN_OP_PLUS] = { // +
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0A00,0x0A00}, AST_T_op_plus}, // a + b
    },
  },
  [TOKEN_OP_OR] = { // |
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0C00,0x0C00}, AST_T_op_binary_or}, // a | b
    },
  },
  [TOKEN_OP_XOR] = { // ^
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0C00,0x0C00}, AST_T_op_binary_xor}, // a ^ b
    },
  },
  [TOKEN_OP_LE] = { // <
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0D00,0x0D00}, AST_T_op_less_than}, // a < b
    },
  },
  [TOKEN_OP_GT] = { // >
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0D00,0x0D00}, AST_T_op_greater_than}, // a > b
    },
  },
  [TOKEN_OP_LTE] = { // <=
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0D00,0x0D00}, AST_T_op_less_than_equal}, // a <= b
    },
  },
  [TOKEN_OP_GTE] = { // >=
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0D00,0x0D00}, AST_T_op_greater_than_equal}, // a >= b
    },
  },
  [KW_AS] = { // as
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x0E01,0x0301}, AST_T_op_cast}, // a as b
    },
  },
  [TOKEN_OP_EQ] = { // ==
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x1000,0x1000}, AST_T_op_equal}, // a == b
    },
  },
  [TOKEN_OP_NEQ] = { // !=
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x1000,0x1000}, AST_T_op_not_equal}, // a != b
    },
  },
  [TOKEN_OP_LOGIC_AND] = { // &&
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x1100,0x1100}, AST_T_op_logic_and}, // a && b
    },
  },
  [TOKEN_OP_LOGIC_OR] = { // ||
    .count = 1,
    .list = (const struct operator_parser_meta[]){
      {{0x1200,0x1200}, AST_T_op_logic_or}, // a || b
    },
  },
};
const size_t opmeta_count = sizeof(opmeta)/sizeof(*opmeta);

bool AST_replace(AST_AST* old_child, AST_AST* new_child){
  if(old_child == new_child)
    return true;
  if(new_child && new_child->parent)
    AST_replace(new_child, 0);
  if(!old_child->parent)
    return !new_child;
  for(AST_IT it = {.ast=old_child->parent}; AST_IT_next(&it, true); ){
    if(*it.node != old_child)
      continue;
    *it.node = new_child;
    break;
  }
  if(new_child)
    new_child->parent = old_child->parent;
  old_child->parent = 0;
  return true;
}

void insert_node_by_precedence(
  AST_expression* top,
  AST_expression* node,
  int left_precedence
){
  if(top) while(top->ast.parent){
    AST_expression* parent;
    {
      AST_AST* n = top->ast.parent;
      if(!AST_is(n->type, expression)){
        fprintf(stderr, "node is not an expression\n");
        exit(99);
      }
      parent = dpa_u_container_of(n, AST_expression, ast);
    }
    if( (top->precedence == left_precedence && !(top->precedence & 1)) // Same precedence, and left to right
      || parent->precedence > left_precedence
    ){
      AST_replace(&top->ast, &node->ast);
      if(AST_is(node->ast.type, operator_unary)){
        AST_operator_unary* op = dpa_u_container_of(node, AST_operator_unary, operator.expression);
        op->input = top;
        top->ast.parent = &node->ast;
      }else if(AST_is(node->ast.type, operator_binary)){
        AST_operator_binary* op = dpa_u_container_of(node, AST_operator_binary, operator.expression);
        op->input[0] = top;
        top->ast.parent = &node->ast;
      }else{
        fprintf(stderr, "node is of an unsupported type\n");
        exit(99);
      }
      // fprintf(stderr, "---\n");
      // print_ast_tree(&top->ast, 0);
      // print_ast_tree(&node->ast, 0);
      // fprintf(stderr, "---\n");
      return;
    }
    top = parent;
  }
  fprintf(stderr, "insert_node_by_precedence failed to find the right place to insert the operator\n");
  exit(99);
}

AST_expression* parse_expression(struct token** start, struct token** end){
  // Note: currently, we need to backtrack at most once, if an infix operator or EOF follows an infix operator
  // We only handle that special case
  int backtrack_i = 0;
  struct token** backtrack_token = start;
  AST_operator_binary* backtrack_op = 0;
  AST_expression* backtrack_top = 0;
  ///////
  AST_op_nothing root = {
    .operator_unary.operator.expression = {
      .ast.type = AST_T_op_nothing,
      .precedence = INT_MAX,
    },
  };
  AST_expr_terminator terminator = {
    .expression = {
      .ast = {
        .type = AST_T_expr_terminator,
        .parent = &root.operator_unary.operator.expression.ast,
      },
      .precedence = -1,
    },
  };
  root.operator_unary.input = &terminator.expression;
  AST_expression* top = &terminator.expression;
  int i;
  // printf("expression\n");
  struct token** t;
  for(t=start; t<end; t++){
    i = 0;
    if(0) backtrack: {
      i = backtrack_i;
      t = backtrack_token;
      top = backtrack_top;
      AST_replace(&backtrack_op->operator.expression.ast, &backtrack_op->input[0]->ast);
      free(backtrack_op);
      terminator.expression.ast.parent = 0;
      backtrack_op = 0;
      backtrack_i = 0;
    }
    AST_expression* value = 0;
    bool maybe_function_call = false;
    switch(t[0]->type){
      case TOKEN_BLOCK: {
        struct token_block* b = (struct token_block*)t[0];
        switch(b->block_type){
          case TOKEN_BLOCK_PARENTHESIS: {
            struct token** tokens = dpa_u_list_data(&b->token_list);
            size_t size = dpa_u_list_length(&b->token_list);
            value = parse_expression(tokens, tokens+size);
            maybe_function_call = true;
          } break;
        }
      }; break;
      case TOKEN_SPACE:
      case TOKEN_COMMENT: continue;
      case TOKEN_IDENTIFIER: {
        AST_named_constant* v = AST_new(named_constant, 0);
        v->name = ((struct token_string*)t[0])->value;
        value = &v->expression;
      } break;
      case TOKEN_STRING: {
        AST_string_value* v = AST_new(string_value, 0);
        v->value = ((struct token_string*)t[0])->value;
        value = &v->expression;
      }; break;
      case TOKEN_INTEGER: {
        AST_integer_value* v = AST_new(integer_value, 0);
        mpz_init(v->value);
        mpz_set(v->value, ((struct token_integer*)t[0])->value);
        value = &v->expression;
      }; break;
      case TOKEN_CHARACTER: {
        // TODO
        continue;
      }; break;
      case TOKEN_KEYWORD:
      case TOKEN_OPERATOR: {
        const struct token_string*restrict ts = (struct token_string*)t[0];
        if(ts->id < opmeta_count && opmeta[ts->id].count){ // It's an operator
          const struct operator_parser_meta_list*restrict const metaops = &opmeta[ts->id];
          for(; i<metaops->count; i++){
            const struct operator_parser_meta*restrict const meta = &metaops->list[i];
            const AST_M_type*const ast_meta = &ast_type_meta[meta->ast_type];
            if(!AST_is(meta->ast_type, operator)){
              fprintf(stderr, "BUG: Expected AST node type to be an AST_operator!\n");
              exit(99);
            }
            AST_operator* op = AST_new_g(meta->ast_type, 0);
            op->expression.precedence = meta->precedence[1];
            if(meta->precedence[0] && meta->precedence[1]){ // infix operator
              if(top == &terminator.expression){
                free(op);
                continue;
              }
              AST_operator_binary* biop = dpa_u_container_of(op, AST_operator_binary, operator);
              backtrack_i = i + 1;
              backtrack_op = biop;
              backtrack_top = top;
              backtrack_token = t;
              biop->input[1] = &terminator.expression;
              insert_node_by_precedence(top, &op->expression, meta->precedence[0]);
              top = &terminator.expression;
              top->ast.parent = &op->expression.ast;
            }else if(meta->precedence[0]){ // suffix operator
              if(AST_is(op->expression.ast.type, incomplete_member_or_deref_cast)){
                if(t<end-1 && t[1]->type == TOKEN_IDENTIFIER){
                  for(; t<end-1; t+=2){
                    if(t[0]->type == TOKEN_OPERATOR && ((struct token_string*)t[0])->id == TOKEN_OP_DOT && t[1]->type == TOKEN_IDENTIFIER){
                      AST_incomplete_member_or_deref_cast* n = dpa_u_container_of(op, AST_incomplete_member_or_deref_cast, operator_unary.operator);
                      dpa_u_list_append(&n->children, 1, (struct token_string**)(t+1));
                    }else break;
                  }
                  t--;
                }else{
                  free(op);
                  continue;
                }
              }
              insert_node_by_precedence(top, &op->expression, meta->precedence[0]);
            }else if(meta->precedence[1]){ // prefix operator
              if(top == &terminator.expression){
                // insert_node_by_precedence(top, &op->expression, 0);
                AST_replace(&top->ast, &op->expression.ast);
                AST_operator_unary* uop = dpa_u_container_of(op, AST_operator_unary, operator);
                uop->input = top;
                top->ast.parent = &op->expression.ast;
              }else{
                value = &op->expression;
              }
            }
            goto op_found;
          }
          if(backtrack_i)
            goto backtrack;
          fprintf(stderr, "This kind of operator can not be used here.\n");
          exit(99);
          op_found:;
        }else{ // It's a keyword
          
        }
      }; break;
    }
    if(!value){
      // return 0;
      continue;
    }
    backtrack_i = 0;
    if(top == &terminator.expression){
      AST_replace(&top->ast, &value->ast);
      top = value;
    }else{
      // expr expr, first expr likely to be a type or function
      
    }
    // result->ast.parent = top.expression.ast.parent;
    // if(!root.operator_unary.input)
    //   root.operator_unary.input = result;
    // top.expression.ast.parent = &result->ast;
  }
  // printf("-----------------\n");
  if(terminator.expression.ast.parent){
    printf("%d\n", backtrack_i);
    print_ast_tree(&root.operator_unary.operator.expression.ast, 0);
    if(backtrack_i)
      goto backtrack;
    fprintf(stderr, "top.parent is still set, operator lacks value!\n");
    return 0;
  }
  AST_expression* bottom = root.operator_unary.input;
  bottom->ast.parent = 0;
  return bottom;
}

AST_statement* parse_declaration(struct token*** pit, struct token** end, enum AST_node_type type){
  struct token** it = *pit;
  enum visibility visibility=0;
  if(token_is_identifier(it[0], KW(KW_EXTERN))){
    visibility = VISIBILITY_EXTERNAL;
    it++;
  }else if(token_is_identifier(it[0], KW(KW_GLOBAL))){
    visibility = VISIBILITY_GLOBAL;
    it++;
  }else if(token_is_identifier(it[0], KW(KW_MODULE))){
    visibility = VISIBILITY_MODULE;
    it++;
  }else if(token_is_identifier(it[0], KW(KW_LOCAL))){
    visibility = VISIBILITY_LOCAL;
    it++;
  }else{
    if(AST_is(type, block_code)) // Visibility is mandatory in a code block
      return 0;
  }
  if(AST_is(type, block_struct_type) && visibility)
    return 0; // Declarations in a struct mustn't have a visibility specified
  if(end-it < 3)
    return 0;
  if(it[0]->type != TOKEN_IDENTIFIER || !token_is_operator(it[1], OP(TOKEN_OP_DEFINE)))
    return 0;
  dpa_u_a_bo_unique_t name = ((const struct token_string*)*it)->value;
  it += 2;
  bool consume_delimiter = false;
  struct token** expr_start = it;
  for(; it<end; it++){
    if(token_is_operator(it[0], OP(TOKEN_OP_SEMICOLON))){
      consume_delimiter = true;
      goto found;
    }
  }
  return 0;
found:;
  AST_declaration* dec = calloc(1, sizeof(AST_declaration));
  dec->statement.ast.type = AST_T_declaration;
  dec->name = name;
  dec->expression = parse_expression(expr_start, it);
  *pit = it + consume_delimiter;
  return &dec->statement;
}

AST_statement* parse_statement(struct token*** pit, struct token** end, enum AST_node_type type){
  struct token** it = *pit;
  if(AST_is(type, block_code)){
    if(token_is_identifier(it[0], KW(KW_WHILE))){
      puts("while");
    }else if(token_is_identifier(it[0], KW(KW_FOR))){
      puts("for");
    }else if(token_is_identifier(it[0], KW(KW_IF))){
      puts("if");
    }
  }
  AST_statement* s = parse_declaration(pit, end, type);
  if(s) return s;
  print_token(*it, 0);
  it++;
  *pit = it;
  return 0;
}

AST_block* parse_block_declarative(AST_block_unknown* ub, enum AST_node_type type){
  AST_block_declarative* b=0;
  if(AST_is(type, block_module)){
    b = calloc(1, sizeof(AST_block_module));
  }else if(AST_is(type, block_struct_type)){
    b = calloc(1, sizeof(AST_block_struct_type));
  }else return 0;
  b->block.ast.type = type;
  struct token**const start = dpa_u_list_data(&ub->token_list);
  struct token**const end = start + dpa_u_list_length(&ub->token_list);
  struct token** it = start;
  while(it < end){
    AST_statement* s = parse_statement(&it, end, type);
    if(!s){
      // goto error; // TODO
      continue;
    }
    dpa_u_list_append(&b->declarations, 1, &s);
  }
  return &b->block;
}

bool parse(struct lexer* lex, struct parser* result){
  AST_block_unknown block = {
    .block.ast.type = AST_T_block_unknown,
  };
  dpa_u_list_swap(&block.token_list, &lex->block.token_list);
  AST_block* module_block = parse_block_declarative(&block, AST_T_block_module);
  if(!module_block)
    goto error;
  result->ast = &module_block->ast;
  return true;
error:
  return false;
}
