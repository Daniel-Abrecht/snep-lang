// DO NOT CHANGE! This is a generated file, changes will be overwritten. Change ast.h.in or gen_ast.py instead.
#include <ast.h>

const struct AST_M_type ast_type_meta[] = {
  [AST_T_statement_expression] = {
    .name = "statement_expression",
    .children = (const struct AST_M_child[]){
      {
        .name = "expression",
        .offset = offsetof(AST_statement_expression, expression),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_statement_expression),
  },
  [AST_T_declaration] = {
    .name = "declaration",
    .children = (const struct AST_M_child[]){
      {
        .name = "expression",
        .offset = offsetof(AST_declaration, expression),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_declaration),
  },
  [AST_T_block_unknown] = {
    .name = "block_unknown",
    .size = sizeof(AST_block_unknown),
  },
  [AST_T_block_module] = {
    .name = "block_module",
    .children = (const struct AST_M_child[]){
      {
        .name = "block_declarative.declarations",
        .offset = offsetof(AST_block_module, block_declarative.declarations),
        .count = 1,
        .is_list = 1,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_block_module),
  },
  [AST_T_block_code] = {
    .name = "block_code",
    .children = (const struct AST_M_child[]){
      {
        .name = "statement",
        .offset = offsetof(AST_block_code, statement),
        .count = 1,
        .is_list = 1,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_block_code),
  },
  [AST_T_block_dataflow] = {
    .name = "block_dataflow",
    .size = sizeof(AST_block_dataflow),
  },
  [AST_T_block_struct_type] = {
    .name = "block_struct_type",
    .children = (const struct AST_M_child[]){
      {
        .name = "block_declarative.declarations",
        .offset = offsetof(AST_block_struct_type, block_declarative.declarations),
        .count = 1,
        .is_list = 1,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_block_struct_type),
  },
  [AST_T_block_struct_value] = {
    .name = "block_struct_value",
    .size = sizeof(AST_block_struct_value),
  },
  [AST_T_named_constant] = {
    .name = "named_constant",
    .size = sizeof(AST_named_constant),
  },
  [AST_T_struct_type] = {
    .name = "struct_type",
    .children = (const struct AST_M_child[]){
      {
        .name = "block",
        .offset = offsetof(AST_struct_type, block),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_struct_type),
  },
  [AST_T_struct_value] = {
    .name = "struct_value",
    .children = (const struct AST_M_child[]){
      {
        .name = "block",
        .offset = offsetof(AST_struct_value, block),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_struct_value),
  },
  [AST_T_string_value] = {
    .name = "string_value",
    .size = sizeof(AST_string_value),
  },
  [AST_T_integer_value] = {
    .name = "integer_value",
    .size = sizeof(AST_integer_value),
  },
  [AST_T_op_copy] = {
    .name = "op_copy",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_unary.input",
        .offset = offsetof(AST_op_copy, operator_unary.input),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_copy),
  },
  [AST_T_op_deref] = {
    .name = "op_deref",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_unary.input",
        .offset = offsetof(AST_op_deref, operator_unary.input),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_deref),
  },
  [AST_T_op_ref] = {
    .name = "op_ref",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_unary.input",
        .offset = offsetof(AST_op_ref, operator_unary.input),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_ref),
  },
  [AST_T_op_meta] = {
    .name = "op_meta",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_unary.input",
        .offset = offsetof(AST_op_meta, operator_unary.input),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_meta),
  },
  [AST_T_op_negate] = {
    .name = "op_negate",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_unary.input",
        .offset = offsetof(AST_op_negate, operator_unary.input),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_negate),
  },
  [AST_T_op_invert] = {
    .name = "op_invert",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_unary.input",
        .offset = offsetof(AST_op_invert, operator_unary.input),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_invert),
  },
  [AST_T_op_not] = {
    .name = "op_not",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_unary.input",
        .offset = offsetof(AST_op_not, operator_unary.input),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_not),
  },
  [AST_T_operator_binary] = {
    .name = "operator_binary",
    .children = (const struct AST_M_child[]){
      {
        .name = "input",
        .offset = offsetof(AST_operator_binary, input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_operator_binary),
  },
  [AST_T_op_member] = {
    .name = "op_member",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_member, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_member),
  },
  [AST_T_op_cast] = {
    .name = "op_cast",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_cast, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_cast),
  },
  [AST_T_op_assignment_normal] = {
    .name = "op_assignment_normal",
    .children = (const struct AST_M_child[]){
      {
        .name = "assignment.operator_binary.input",
        .offset = offsetof(AST_op_assignment_normal, assignment.operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_assignment_normal),
  },
  [AST_T_op_assignment_inline] = {
    .name = "op_assignment_inline",
    .children = (const struct AST_M_child[]){
      {
        .name = "assignment.operator_binary.input",
        .offset = offsetof(AST_op_assignment_inline, assignment.operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_assignment_inline),
  },
  [AST_T_op_subscript] = {
    .name = "op_subscript",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_subscript, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_subscript),
  },
  [AST_T_op_minus] = {
    .name = "op_minus",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_minus, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_minus),
  },
  [AST_T_op_plus] = {
    .name = "op_plus",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_plus, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_plus),
  },
  [AST_T_op_multiply] = {
    .name = "op_multiply",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_multiply, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_multiply),
  },
  [AST_T_op_divide] = {
    .name = "op_divide",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_divide, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_divide),
  },
  [AST_T_op_reminder] = {
    .name = "op_reminder",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_reminder, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_reminder),
  },
  [AST_T_op_divide_reminder] = {
    .name = "op_divide_reminder",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_divide_reminder, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_divide_reminder),
  },
  [AST_T_op_power] = {
    .name = "op_power",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_power, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_power),
  },
  [AST_T_op_bitshift_left] = {
    .name = "op_bitshift_left",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_bitshift_left, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_bitshift_left),
  },
  [AST_T_op_bitshift_right] = {
    .name = "op_bitshift_right",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_bitshift_right, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_bitshift_right),
  },
  [AST_T_op_binary_and] = {
    .name = "op_binary_and",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_binary_and, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_binary_and),
  },
  [AST_T_op_binary_or] = {
    .name = "op_binary_or",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_binary_or, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_binary_or),
  },
  [AST_T_op_binary_xor] = {
    .name = "op_binary_xor",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_binary_xor, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_binary_xor),
  },
  [AST_T_op_logic_and] = {
    .name = "op_logic_and",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_logic_and, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_logic_and),
  },
  [AST_T_op_logic_or] = {
    .name = "op_logic_or",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_logic_or, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_logic_or),
  },
  [AST_T_op_equal] = {
    .name = "op_equal",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_equal, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_equal),
  },
  [AST_T_op_not_equal] = {
    .name = "op_not_equal",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_not_equal, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_not_equal),
  },
  [AST_T_op_less_than] = {
    .name = "op_less_than",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_less_than, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_less_than),
  },
  [AST_T_op_less_than_equal] = {
    .name = "op_less_than_equal",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_less_than_equal, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_less_than_equal),
  },
  [AST_T_op_greater_than] = {
    .name = "op_greater_than",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_greater_than, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_greater_than),
  },
  [AST_T_op_greater_than_equal] = {
    .name = "op_greater_than_equal",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_binary.input",
        .offset = offsetof(AST_op_greater_than_equal, operator_binary.input),
        .count = 2,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_greater_than_equal),
  },
  [AST_T_incomplete_expression_application] = {
    .name = "incomplete_expression_application",
    .children = (const struct AST_M_child[]){
      {
        .name = "children",
        .offset = offsetof(AST_incomplete_expression_application, children),
        .count = 1,
        .is_list = 1,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_incomplete_expression_application),
  },
  [AST_T_incomplete_member_or_deref_cast] = {
    .name = "incomplete_member_or_deref_cast",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_unary.input",
        .offset = offsetof(AST_incomplete_member_or_deref_cast, operator_unary.input),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_incomplete_member_or_deref_cast),
  },
  [AST_T_expr_terminator] = {
    .name = "expr_terminator",
    .size = sizeof(AST_expr_terminator),
  },
  [AST_T_op_nothing] = {
    .name = "op_nothing",
    .children = (const struct AST_M_child[]){
      {
        .name = "operator_unary.input",
        .offset = offsetof(AST_op_nothing, operator_unary.input),
        .count = 1,
        .is_list = 0,
      },
    },
    .children_count = 1,
    .size = sizeof(AST_op_nothing),
  },
};
