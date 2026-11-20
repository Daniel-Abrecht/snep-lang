#include <ast.h>
#include <dpa/utils/bo.h>

bool AST_IT_next(AST_IT*restrict it, bool skip_null){
  if(!it) return false;
next:
  if(it->node && it->node+1 < it->end){
    it->node += 1;
    if(skip_null && !*it->node)
      goto next;
    return true;
  }
  if(!it->ast) return false;
  enum AST_node_type type = it->ast->type;
  if(type < AST_S_AST || type > AST_E_AST)
    return false;
  while(true){
    it->child += 1;
    const AST_M_type* meta = &ast_type_meta[type];
    if(it->child > meta->children_count){
      it->child = 0;
      return false;
    }
    const AST_M_child* child = &meta->children[it->child-1];
    void* entry = ((char*)it->ast) + child->offset;
    if(child->is_list){
      dpa_u_list(AST_AST*)* list = entry;
      it->node = dpa_u_list_data(list);
      it->end = it->node + dpa_u_list_length(list);
    }else{
      it->node = entry;
      it->end = (AST_AST**)entry + child->count;
    }
    if(it->node < it->end){
      if(skip_null && !*it->node)
        goto next;
      return true;
    }
  }
}

void print_ast_node_member(const AST_AST* ast, int indent, const AST_M_child* child){
  if(!ast){
    printf("%*snull", indent, "");
  }else{
    const AST_M_type* meta = &ast_type_meta[ast->type];
    printf("%*s%s", indent, "", meta->name);
  }
  if(child){
    printf(" : %s", child->name);
  }
  if(ast) switch(ast->type){
    case AST_T_declaration: {
      dpa_u_bo_t bo = dpa_u_to_bo(((AST_declaration*)ast)->name);
      printf(" `%.*s`", (int)bo.size, (char*)bo.data);
    } break;
    case AST_T_integer_value: {
      gmp_printf(" %Zd", ((AST_integer_value*)ast)->value);
    } break;
    default: break;
  }
  puts("");
}

void print_ast_node(const AST_AST* ast, int indent){
  print_ast_node_member(ast, indent, 0);
}

void print_ast_tree_sub(const AST_AST* ast, int indent, const AST_M_child* child, bool skip_null){
  print_ast_node_member(ast, indent, child);
  AST_IT it = {.ast=ast};
  while(AST_IT_next(&it, skip_null)){
    const AST_M_type* meta = &ast_type_meta[ast->type];
    const AST_M_child* child = &meta->children[it.child-1];
    print_ast_tree_sub(*it.node, indent+2, child, skip_null);
  }
}

void print_ast_tree(const AST_AST* ast, int indent){
  print_ast_tree_sub(ast, indent, 0, true);
}
