#include <tokenizer.h>
#include <parser.h>
#include <ast.h>

int main(int argc, char* argv[]){
  (void)argc;
  FILE* f = argc >= 2 ? fopen(argv[1], "rb") : stdin;
  if(!f){
    perror("fopen");
    return 1;
  }
  struct lexer lexer = {0};
  if(!lex(f, &lexer))
    return 1;
  // print_token(&lexer.block.super, 0);
  struct parser parser = {0};
  if(!parse(&lexer, &parser))
     return 2;
  lex_destroy(&lexer);
  print_ast_tree(parser.ast, 0);
  return 0;
}
