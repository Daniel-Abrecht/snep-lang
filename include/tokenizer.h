#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <dpa/utils/list.h>
#include <dpa/utils/bo.h>
#include <inttypes.h>
#include <gmp.h>

enum token_type {
  TOKEN_SPACE,
  TOKEN_COMMENT,
  TOKEN_IDENTIFIER,
  TOKEN_KEYWORD,
  TOKEN_OPERATOR,
  TOKEN_STRING,
  TOKEN_CHARACTER,
  TOKEN_INTEGER,
  TOKEN_BLOCK,
};

struct token {
  enum token_type type;
  size_t line;
  size_t character;
};

struct token_string {
  struct token super;
  dpa_u_a_bo_unique_t value;
  unsigned id;
};

struct token_integer {
  struct token super;
  mpz_t value;
};

#define OPERATOR_LIST \
  X(("=="), EQ) \
  X((":="), ASSIGN) \
  X(("!="), NEQ) \
  X(("<="), LTE) \
  X((">="), GTE) \
  X(("&&"), LOGIC_AND) \
  X(("||"), LOGIC_OR) \
  X(("<<"), SHIFT_LEFT) \
  X((">>"), SHIFT_RIGHT) \
  X(("::"), SCOPE) \
  X(("**"), POWER) \
  X(("/%"), DIVMOD) \
  X(("@"), META) \
  X(("$"), COPY) \
  X(("'"), PTR) \
  X(("&"), AND) \
  X(("|"), OR) \
  X(("^"), XOR) \
  X(("<"), LE) \
  X((">"), GT) \
  X(("="), DEFINE) \
  X(("+"), PLUS) \
  X(("-"), MINUS) \
  X(("*"), MUL) \
  X(("/"), DIV) \
  X(("%"), MOD) \
  X(("."), DOT) \
  X((";"), SEMICOLON) \
  X((":"), COLON) \
  X(("!"), LOGIC_NOT) \
  X(("~"), NOT) \
  X(("?"), QUESTION_MARK)

#define KEYWORD_LIST \
  X(("as"), AS) \
  X(("while"), WHILE) \
  X(("for"), FOR) \
  X(("if"), IF) \
  X(("else"), ELSE) \
  X(("type"), TYPE) \
  X(("struct"), STRUCT) \
  X(("union"), UNION) \
  X(("int"), INT) \
  X(("signed"), SIGNED) \
  X(("unsigned"), UNSIGNED) \
  X(("float"), FLOAT) \
  X(("namespace"), NAMESPACE) \
  X(("inline"), INLINE) \
  X(("extern"), EXTERN) \
  X(("global"), GLOBAL) \
  X(("module"), MODULE) \
  X(("local"), LOCAL) \
  X(("rw"), RW) \
  X(("ro"), RO) \
  X(("wo"), WO) \
  X(("unique"), UNIQUE) \
  X(("return"), RETURN) \
  X(("yield"), YIELD) \
  X(("routine"), ROUTINE) \
  X(("stack"), STACK) \
  X(("modifier"), MODIFIER) \
  X(("dataflow"), DATAFLOW) \
  X(("coroutine"), COROUTINE) \
  X(("generator"), GENERATOR) \
  X(("goto"), GOTO) \
  X(("break"), BREAK) \
  X(("continue"), CONTINUE)

enum keyword_enum {
  OPERATOR_FIRST = 0,
#define X(OP, NAME) TOKEN_OP_ ## NAME,
OPERATOR_LIST
#undef X
  OPERATOR_LAST,
  KEYWORD_FIRST = OPERATOR_LAST,
#define X(S, NAME) KW_ ## NAME,
KEYWORD_LIST
#undef X
  KEYWORD_LAST,
  KEYWORD_COUNT,
};

union keyword_list {
  dpa_u_a_bo_unique_t id[KEYWORD_COUNT];
  const char* init[KEYWORD_COUNT];
};
extern union keyword_list g_keyword;
#define KW(X) g_keyword.id[(X)]
#define OP(X) g_keyword.id[(X)]


#define TOKEN_BLOCK_LIST \
  X(("<{"), ("}"), BRACE_CODE) \
  X(("!{"), ("}"), BRACE_TYPE) \
  X(("${"), ("}"), BRACE_VALUE) \
  X(("@{"), ("}"), BRACE_TEMPLATE) \
  X(("{"), ("}"), BRACE_AUTO) \
  X(("("), (")"), PARENTHESIS) \
  X(("["), ("]"), BRACKET)

enum block_token_enum {
  TOKEN_BLOCK_FIRST = 0,
#define X(OPEN, CLOSE, NAME) TOKEN_BLOCK_ ## NAME,
TOKEN_BLOCK_LIST
#undef X
  TOKEN_BLOCK_LAST,
};

union token_block_list {
  struct {
    dpa_u_a_bo_unique_t open, close;
  } id[TOKEN_BLOCK_LAST-TOKEN_BLOCK_FIRST];
  struct {
    const char *open, *close;
  } init[TOKEN_BLOCK_LAST-TOKEN_BLOCK_FIRST];
};
extern union token_block_list tb_list;
#define TB(X) tb_list.id[(X)-TOKEN_BLOCK_FIRST]

struct token_block {
  struct token super;
  enum block_token_enum block_type;
  dpa_u_list(struct token*) token_list;
};

struct lexer {
  struct token_block block;
};

void print_token(struct token* t, int level);
void lex_destroy(struct lexer* lexer);
bool lex(FILE* f, struct lexer* result);

#endif
