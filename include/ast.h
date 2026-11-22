// DO NOT CHANGE! This is a generated file, changes will be overwritten. Change ast.h.in or gen_ast.py instead.
#ifndef AST_H
#define AST_H


#include <stddef.h>
#include <stdbool.h>
#include <gmp.h>
#include <dpa/utils/list.h>
#include <dpa/utils/bo.h>

#define abstract
#define related

#define AST_is(VALUE, TYPE) ((VALUE) >= AST_S_##TYPE && (VALUE) <= AST_E_##TYPE)


typedef struct AST_M_child AST_M_child;
typedef struct AST_M_type AST_M_type;
typedef struct AST_IT AST_IT;

struct AST_M_child {
  const char* name;
  int offset;
  int count;
  bool is_list;
};

struct AST_M_type {
  const char* name;
  const struct AST_M_child* children;
  int children_count;
  int size; // sizeof(AST_*)
};


// <AST_BEGIN>

enum AST_node_type {
  AST_S_AST = 0,
  AST_S_statement = 0,
  AST_S_declaration = 0,
  AST_T_declaration = 0,
  AST_E_declaration = 0,
  AST_E_statement = 0,
  AST_S_statement_expression = 1,
  AST_T_statement_expression = 1,
  AST_E_statement_expression = 1,
  AST_S_block = 2,
  AST_S_block_unknown = 2,
  AST_T_block_unknown = 2,
  AST_E_block_unknown = 2,
  AST_S_block_declarative = 3,
  AST_S_block_module = 3,
  AST_T_block_module = 3,
  AST_E_block_module = 3,
  AST_S_block_struct_type = 4,
  AST_T_block_struct_type = 4,
  AST_E_block_struct_type = 4,
  AST_E_block_declarative = 4,
  AST_S_block_code = 5,
  AST_T_block_code = 5,
  AST_E_block_code = 5,
  AST_S_block_dataflow = 6,
  AST_T_block_dataflow = 6,
  AST_E_block_dataflow = 6,
  AST_S_block_struct_value = 7,
  AST_T_block_struct_value = 7,
  AST_E_block_struct_value = 7,
  AST_E_block = 7,
  AST_S_expression = 8,
  AST_S_named_constant = 8,
  AST_T_named_constant = 8,
  AST_E_named_constant = 8,
  AST_S_struct_type = 9,
  AST_T_struct_type = 9,
  AST_E_struct_type = 9,
  AST_S_struct_value = 10,
  AST_T_struct_value = 10,
  AST_E_struct_value = 10,
  AST_S_string_value = 11,
  AST_T_string_value = 11,
  AST_E_string_value = 11,
  AST_S_integer_value = 12,
  AST_T_integer_value = 12,
  AST_E_integer_value = 12,
  AST_S_operator = 13,
  AST_S_operator_unary = 13,
  AST_S_op_copy = 13,
  AST_T_op_copy = 13,
  AST_E_op_copy = 13,
  AST_S_op_deref = 14,
  AST_T_op_deref = 14,
  AST_E_op_deref = 14,
  AST_S_op_ref = 15,
  AST_T_op_ref = 15,
  AST_E_op_ref = 15,
  AST_S_op_meta = 16,
  AST_T_op_meta = 16,
  AST_E_op_meta = 16,
  AST_S_op_negate = 17,
  AST_T_op_negate = 17,
  AST_E_op_negate = 17,
  AST_S_op_invert = 18,
  AST_T_op_invert = 18,
  AST_E_op_invert = 18,
  AST_S_op_not = 19,
  AST_T_op_not = 19,
  AST_E_op_not = 19,
  AST_S_incomplete_member_or_deref_cast = 20,
  AST_T_incomplete_member_or_deref_cast = 20,
  AST_E_incomplete_member_or_deref_cast = 20,
  AST_S_op_nothing = 21,
  AST_T_op_nothing = 21,
  AST_E_op_nothing = 21,
  AST_E_operator_unary = 21,
  AST_S_operator_binary = 22,
  AST_T_operator_binary = 22,
  AST_S_op_member = 23,
  AST_T_op_member = 23,
  AST_E_op_member = 23,
  AST_S_op_cast = 24,
  AST_T_op_cast = 24,
  AST_E_op_cast = 24,
  AST_S_op_assignment = 25,
  AST_S_op_assignment_normal = 25,
  AST_T_op_assignment_normal = 25,
  AST_E_op_assignment_normal = 25,
  AST_S_op_assignment_inline = 26,
  AST_T_op_assignment_inline = 26,
  AST_E_op_assignment_inline = 26,
  AST_E_op_assignment = 26,
  AST_S_op_subscript = 27,
  AST_T_op_subscript = 27,
  AST_E_op_subscript = 27,
  AST_S_op_minus = 28,
  AST_T_op_minus = 28,
  AST_E_op_minus = 28,
  AST_S_op_plus = 29,
  AST_T_op_plus = 29,
  AST_E_op_plus = 29,
  AST_S_op_multiply = 30,
  AST_T_op_multiply = 30,
  AST_E_op_multiply = 30,
  AST_S_op_divide = 31,
  AST_T_op_divide = 31,
  AST_E_op_divide = 31,
  AST_S_op_reminder = 32,
  AST_T_op_reminder = 32,
  AST_E_op_reminder = 32,
  AST_S_op_divide_reminder = 33,
  AST_T_op_divide_reminder = 33,
  AST_E_op_divide_reminder = 33,
  AST_S_op_power = 34,
  AST_T_op_power = 34,
  AST_E_op_power = 34,
  AST_S_op_bitshift_left = 35,
  AST_T_op_bitshift_left = 35,
  AST_E_op_bitshift_left = 35,
  AST_S_op_bitshift_right = 36,
  AST_T_op_bitshift_right = 36,
  AST_E_op_bitshift_right = 36,
  AST_S_op_binary_and = 37,
  AST_T_op_binary_and = 37,
  AST_E_op_binary_and = 37,
  AST_S_op_binary_or = 38,
  AST_T_op_binary_or = 38,
  AST_E_op_binary_or = 38,
  AST_S_op_binary_xor = 39,
  AST_T_op_binary_xor = 39,
  AST_E_op_binary_xor = 39,
  AST_S_op_logic_and = 40,
  AST_T_op_logic_and = 40,
  AST_E_op_logic_and = 40,
  AST_S_op_logic_or = 41,
  AST_T_op_logic_or = 41,
  AST_E_op_logic_or = 41,
  AST_S_op_equal = 42,
  AST_T_op_equal = 42,
  AST_E_op_equal = 42,
  AST_S_op_not_equal = 43,
  AST_T_op_not_equal = 43,
  AST_E_op_not_equal = 43,
  AST_S_op_less_than = 44,
  AST_T_op_less_than = 44,
  AST_E_op_less_than = 44,
  AST_S_op_less_than_equal = 45,
  AST_T_op_less_than_equal = 45,
  AST_E_op_less_than_equal = 45,
  AST_S_op_greater_than = 46,
  AST_T_op_greater_than = 46,
  AST_E_op_greater_than = 46,
  AST_S_op_greater_than_equal = 47,
  AST_T_op_greater_than_equal = 47,
  AST_E_op_greater_than_equal = 47,
  AST_E_operator_binary = 47,
  AST_S_incomplete_expression_application = 48,
  AST_T_incomplete_expression_application = 48,
  AST_E_incomplete_expression_application = 48,
  AST_E_operator = 48,
  AST_S_expr_terminator = 49,
  AST_T_expr_terminator = 49,
  AST_E_expr_terminator = 49,
  AST_E_expression = 49,
  AST_E_AST = 49,
};
typedef struct AST_AST AST_AST;
typedef struct AST_statement AST_statement;
typedef struct AST_declaration AST_declaration;
typedef struct AST_statement_expression AST_statement_expression;
typedef struct AST_block AST_block;
typedef struct AST_block_unknown AST_block_unknown;
typedef struct AST_block_declarative AST_block_declarative;
typedef struct AST_block_module AST_block_module;
typedef struct AST_block_struct_type AST_block_struct_type;
typedef struct AST_block_code AST_block_code;
typedef struct AST_block_dataflow AST_block_dataflow;
typedef struct AST_block_struct_value AST_block_struct_value;
typedef struct AST_expression AST_expression;
typedef struct AST_named_constant AST_named_constant;
typedef struct AST_struct_type AST_struct_type;
typedef struct AST_struct_value AST_struct_value;
typedef struct AST_string_value AST_string_value;
typedef struct AST_integer_value AST_integer_value;
typedef struct AST_operator AST_operator;
typedef struct AST_operator_unary AST_operator_unary;
typedef struct AST_op_copy AST_op_copy;
typedef struct AST_op_deref AST_op_deref;
typedef struct AST_op_ref AST_op_ref;
typedef struct AST_op_meta AST_op_meta;
typedef struct AST_op_negate AST_op_negate;
typedef struct AST_op_invert AST_op_invert;
typedef struct AST_op_not AST_op_not;
typedef struct AST_incomplete_member_or_deref_cast AST_incomplete_member_or_deref_cast;
typedef struct AST_op_nothing AST_op_nothing;
typedef struct AST_operator_binary AST_operator_binary;
typedef struct AST_op_member AST_op_member;
typedef struct AST_op_cast AST_op_cast;
typedef struct AST_op_assignment AST_op_assignment;
typedef struct AST_op_assignment_normal AST_op_assignment_normal;
typedef struct AST_op_assignment_inline AST_op_assignment_inline;
typedef struct AST_op_subscript AST_op_subscript;
typedef struct AST_op_minus AST_op_minus;
typedef struct AST_op_plus AST_op_plus;
typedef struct AST_op_multiply AST_op_multiply;
typedef struct AST_op_divide AST_op_divide;
typedef struct AST_op_reminder AST_op_reminder;
typedef struct AST_op_divide_reminder AST_op_divide_reminder;
typedef struct AST_op_power AST_op_power;
typedef struct AST_op_bitshift_left AST_op_bitshift_left;
typedef struct AST_op_bitshift_right AST_op_bitshift_right;
typedef struct AST_op_binary_and AST_op_binary_and;
typedef struct AST_op_binary_or AST_op_binary_or;
typedef struct AST_op_binary_xor AST_op_binary_xor;
typedef struct AST_op_logic_and AST_op_logic_and;
typedef struct AST_op_logic_or AST_op_logic_or;
typedef struct AST_op_equal AST_op_equal;
typedef struct AST_op_not_equal AST_op_not_equal;
typedef struct AST_op_less_than AST_op_less_than;
typedef struct AST_op_less_than_equal AST_op_less_than_equal;
typedef struct AST_op_greater_than AST_op_greater_than;
typedef struct AST_op_greater_than_equal AST_op_greater_than_equal;
typedef struct AST_incomplete_expression_application AST_incomplete_expression_application;
typedef struct AST_expr_terminator AST_expr_terminator;

// </AST_BEGIN>



abstract struct AST_AST {
  enum AST_node_type type;
  related AST_AST* parent;
};


////// Statement //////

abstract struct AST_statement {
  AST_AST ast;
};

struct AST_statement_expression {
  AST_AST ast;
  struct AST_expression* expression;
};

struct AST_declaration { // Note: also used for definitions. Definitions have a value.
  AST_statement statement;
  dpa_u_a_bo_unique_t name;
  // struct scope_entry entry;
  AST_expression* expression;
};


////// Blocks //////

abstract struct AST_block {
  AST_AST ast;
};

struct AST_block_unknown {
  AST_block block;
  dpa_u_list(struct token*) token_list;
};

abstract struct AST_block_declarative {
  AST_block block;
  dpa_u_list(AST_statement*) declarations;
  // scope scope;
};

struct AST_block_module {
  AST_block_declarative block_declarative;
};

struct AST_block_code {
  AST_block block;
  dpa_u_list(AST_statement*) statement;
  bool is_stackful;
};

struct AST_block_dataflow {
  AST_block block;
};

struct AST_block_struct_type {
  AST_block_declarative block_declarative;
};

struct AST_block_struct_value {
  AST_block block;
};


////// Expression //////

abstract struct AST_expression {
  AST_AST ast;
  union {
    int precedence; // Only used when constructing the AST
  };
};

struct AST_named_constant {
  AST_expression expression;
  dpa_u_a_bo_unique_t name;
};

struct AST_struct_type {
  AST_expression expression;
  AST_block_struct_type* block;
};

struct AST_struct_value {
  AST_expression expression;
  AST_block_struct_value* block;
};

struct AST_string_value {
  AST_expression expression;
  dpa_u_a_bo_unique_t value;
};

struct AST_integer_value {
  AST_expression expression;
  mpz_t value;
};

////// Operation //////

abstract struct AST_operator {
  AST_expression expression;
};

abstract struct AST_operator_unary {
  AST_operator operator;
  AST_expression* input;
};

struct AST_op_copy { AST_operator_unary operator_unary; };
struct AST_op_deref { AST_operator_unary operator_unary; };
struct AST_op_ref { AST_operator_unary operator_unary; }; // Only applicable to types
struct AST_op_meta { AST_operator_unary operator_unary; };
struct AST_op_negate { AST_operator_unary operator_unary; };
struct AST_op_invert { AST_operator_unary operator_unary; };
struct AST_op_not { AST_operator_unary operator_unary; };

struct AST_operator_binary {
  AST_operator operator;
  AST_expression* input[2];
};

struct AST_op_member { AST_operator_binary operator_binary; };
struct AST_op_cast { AST_operator_binary operator_binary; };
abstract struct AST_op_assignment { AST_operator_binary operator_binary; };
struct AST_op_assignment_normal { AST_op_assignment assignment; }; // Has verry low precedence
struct AST_op_assignment_inline { AST_op_assignment assignment; }; // Has higher precedence
struct AST_op_subscript { AST_operator_binary operator_binary; };
struct AST_op_minus { AST_operator_binary operator_binary; };
struct AST_op_plus { AST_operator_binary operator_binary; };
struct AST_op_multiply { AST_operator_binary operator_binary; };
struct AST_op_divide { AST_operator_binary operator_binary; };
struct AST_op_reminder { AST_operator_binary operator_binary; };
struct AST_op_divide_reminder { AST_operator_binary operator_binary; };
struct AST_op_power { AST_operator_binary operator_binary; };
struct AST_op_bitshift_left { AST_operator_binary operator_binary; };
struct AST_op_bitshift_right { AST_operator_binary operator_binary; };
struct AST_op_binary_and { AST_operator_binary operator_binary; };
struct AST_op_binary_or { AST_operator_binary operator_binary; };
struct AST_op_binary_xor { AST_operator_binary operator_binary; };
struct AST_op_logic_and { AST_operator_binary operator_binary; };
struct AST_op_logic_or { AST_operator_binary operator_binary; };
struct AST_op_equal { AST_operator_binary operator_binary; };
struct AST_op_not_equal { AST_operator_binary operator_binary; };
struct AST_op_less_than { AST_operator_binary operator_binary; };
struct AST_op_less_than_equal { AST_operator_binary operator_binary; };
struct AST_op_greater_than { AST_operator_binary operator_binary; };
struct AST_op_greater_than_equal { AST_operator_binary operator_binary; };

struct AST_incomplete_expression_application {
  // For "type type (value)", we have cast(type,cast(type,value))
  // but for "function (function) (value)", we have call(call(function),value)
  // This can only be resolved after the type category of the resulting values is known
  AST_operator operator;
  dpa_u_list(AST_expression*) children;
};

struct AST_incomplete_member_or_deref_cast {
  // An expression a.b.c can be a member(member(a,b),c) or a cast(deref(a),cast(deref(b),c)).
  // Because these differ in structure, but we still need to reason about the final type which may be part of
  // a recursive type, we temporarely put it here, and resolve this laterr once we know enough about the types
  // of the identifiers involved.
  // The leftmost expression it applies to is expression.input, for example: "a.b.c -> a" or "f().b.c -> f()"
  AST_operator_unary operator_unary;
  dpa_u_list(struct token_string*) children; // The remaining . and unresolved identifier tokens.
};

// Helper nodes, used when constructing the AST.
struct AST_expr_terminator { AST_expression expression; };
struct AST_op_nothing { AST_operator_unary operator_unary; };


// <AST_END>

static_assert(offsetof(AST_statement, ast) == 0, "AST_statement::ast must be the first member!");
static_assert(offsetof(AST_statement_expression, ast) == 0, "AST_statement_expression::ast must be the first member!");
static_assert(offsetof(AST_declaration, statement) == 0, "AST_declaration::statement must be the first member!");
static_assert(offsetof(AST_block, ast) == 0, "AST_block::ast must be the first member!");
static_assert(offsetof(AST_block_unknown, block) == 0, "AST_block_unknown::block must be the first member!");
static_assert(offsetof(AST_block_declarative, block) == 0, "AST_block_declarative::block must be the first member!");
static_assert(offsetof(AST_block_module, block_declarative) == 0, "AST_block_module::block_declarative must be the first member!");
static_assert(offsetof(AST_block_code, block) == 0, "AST_block_code::block must be the first member!");
static_assert(offsetof(AST_block_dataflow, block) == 0, "AST_block_dataflow::block must be the first member!");
static_assert(offsetof(AST_block_struct_type, block_declarative) == 0, "AST_block_struct_type::block_declarative must be the first member!");
static_assert(offsetof(AST_block_struct_value, block) == 0, "AST_block_struct_value::block must be the first member!");
static_assert(offsetof(AST_expression, ast) == 0, "AST_expression::ast must be the first member!");
static_assert(offsetof(AST_named_constant, expression) == 0, "AST_named_constant::expression must be the first member!");
static_assert(offsetof(AST_struct_type, expression) == 0, "AST_struct_type::expression must be the first member!");
static_assert(offsetof(AST_struct_value, expression) == 0, "AST_struct_value::expression must be the first member!");
static_assert(offsetof(AST_string_value, expression) == 0, "AST_string_value::expression must be the first member!");
static_assert(offsetof(AST_integer_value, expression) == 0, "AST_integer_value::expression must be the first member!");
static_assert(offsetof(AST_operator, expression) == 0, "AST_operator::expression must be the first member!");
static_assert(offsetof(AST_operator_unary, operator) == 0, "AST_operator_unary::operator must be the first member!");
static_assert(offsetof(AST_op_copy, operator_unary) == 0, "AST_op_copy::operator_unary must be the first member!");
static_assert(offsetof(AST_op_deref, operator_unary) == 0, "AST_op_deref::operator_unary must be the first member!");
static_assert(offsetof(AST_op_ref, operator_unary) == 0, "AST_op_ref::operator_unary must be the first member!");
static_assert(offsetof(AST_op_meta, operator_unary) == 0, "AST_op_meta::operator_unary must be the first member!");
static_assert(offsetof(AST_op_negate, operator_unary) == 0, "AST_op_negate::operator_unary must be the first member!");
static_assert(offsetof(AST_op_invert, operator_unary) == 0, "AST_op_invert::operator_unary must be the first member!");
static_assert(offsetof(AST_op_not, operator_unary) == 0, "AST_op_not::operator_unary must be the first member!");
static_assert(offsetof(AST_operator_binary, operator) == 0, "AST_operator_binary::operator must be the first member!");
static_assert(offsetof(AST_op_member, operator_binary) == 0, "AST_op_member::operator_binary must be the first member!");
static_assert(offsetof(AST_op_cast, operator_binary) == 0, "AST_op_cast::operator_binary must be the first member!");
static_assert(offsetof(AST_op_assignment, operator_binary) == 0, "AST_op_assignment::operator_binary must be the first member!");
static_assert(offsetof(AST_op_assignment_normal, assignment) == 0, "AST_op_assignment_normal::assignment must be the first member!");
static_assert(offsetof(AST_op_assignment_inline, assignment) == 0, "AST_op_assignment_inline::assignment must be the first member!");
static_assert(offsetof(AST_op_subscript, operator_binary) == 0, "AST_op_subscript::operator_binary must be the first member!");
static_assert(offsetof(AST_op_minus, operator_binary) == 0, "AST_op_minus::operator_binary must be the first member!");
static_assert(offsetof(AST_op_plus, operator_binary) == 0, "AST_op_plus::operator_binary must be the first member!");
static_assert(offsetof(AST_op_multiply, operator_binary) == 0, "AST_op_multiply::operator_binary must be the first member!");
static_assert(offsetof(AST_op_divide, operator_binary) == 0, "AST_op_divide::operator_binary must be the first member!");
static_assert(offsetof(AST_op_reminder, operator_binary) == 0, "AST_op_reminder::operator_binary must be the first member!");
static_assert(offsetof(AST_op_divide_reminder, operator_binary) == 0, "AST_op_divide_reminder::operator_binary must be the first member!");
static_assert(offsetof(AST_op_power, operator_binary) == 0, "AST_op_power::operator_binary must be the first member!");
static_assert(offsetof(AST_op_bitshift_left, operator_binary) == 0, "AST_op_bitshift_left::operator_binary must be the first member!");
static_assert(offsetof(AST_op_bitshift_right, operator_binary) == 0, "AST_op_bitshift_right::operator_binary must be the first member!");
static_assert(offsetof(AST_op_binary_and, operator_binary) == 0, "AST_op_binary_and::operator_binary must be the first member!");
static_assert(offsetof(AST_op_binary_or, operator_binary) == 0, "AST_op_binary_or::operator_binary must be the first member!");
static_assert(offsetof(AST_op_binary_xor, operator_binary) == 0, "AST_op_binary_xor::operator_binary must be the first member!");
static_assert(offsetof(AST_op_logic_and, operator_binary) == 0, "AST_op_logic_and::operator_binary must be the first member!");
static_assert(offsetof(AST_op_logic_or, operator_binary) == 0, "AST_op_logic_or::operator_binary must be the first member!");
static_assert(offsetof(AST_op_equal, operator_binary) == 0, "AST_op_equal::operator_binary must be the first member!");
static_assert(offsetof(AST_op_not_equal, operator_binary) == 0, "AST_op_not_equal::operator_binary must be the first member!");
static_assert(offsetof(AST_op_less_than, operator_binary) == 0, "AST_op_less_than::operator_binary must be the first member!");
static_assert(offsetof(AST_op_less_than_equal, operator_binary) == 0, "AST_op_less_than_equal::operator_binary must be the first member!");
static_assert(offsetof(AST_op_greater_than, operator_binary) == 0, "AST_op_greater_than::operator_binary must be the first member!");
static_assert(offsetof(AST_op_greater_than_equal, operator_binary) == 0, "AST_op_greater_than_equal::operator_binary must be the first member!");
static_assert(offsetof(AST_incomplete_expression_application, operator) == 0, "AST_incomplete_expression_application::operator must be the first member!");
static_assert(offsetof(AST_incomplete_member_or_deref_cast, operator_unary) == 0, "AST_incomplete_member_or_deref_cast::operator_unary must be the first member!");
static_assert(offsetof(AST_expr_terminator, expression) == 0, "AST_expr_terminator::expression must be the first member!");
static_assert(offsetof(AST_op_nothing, operator_unary) == 0, "AST_op_nothing::operator_unary must be the first member!");

// </AST_END>



typedef struct AST_IT AST_IT;
struct AST_IT {
  const AST_AST* ast;
  AST_AST **node, **end;
  int child;
};

bool AST_IT_next(AST_IT*restrict ast, bool skip_null);

void print_ast_tree(const AST_AST* ast, int indent);
void print_ast_node(const AST_AST* ast, int indent);

extern const struct AST_M_type ast_type_meta[];


#undef related
#undef abstract
#endif
