#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <dpa/utils/bo.h>
#include <tokenizer.h>


struct lexer;
struct parser {
  struct AST_AST* ast;
};

bool parse(struct lexer* lex, struct parser* result);

#endif
