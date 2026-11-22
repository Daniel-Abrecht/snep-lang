#include <tokenizer.h>
#include <dpa/utils/map.h>
#include <dpa/utils/utf8.h>
#include <dpa/utils/unicode.h>


union keyword_list g_keyword = {
  .init = {
#define X(OP, NAME) [TOKEN_OP_ ## NAME] = OP,
    OPERATOR_LIST
#undef X
#define X(S, NAME) [KW_ ## NAME] = S,
    KEYWORD_LIST
#undef X
  }
};

#define X(OPEN, CLOSE, NAME) [TOKEN_BLOCK_ ## NAME - TOKEN_BLOCK_FIRST] = {OPEN, CLOSE},
union token_block_list tb_list = {
  .init = { TOKEN_BLOCK_LIST }
};
#undef X

dpa_u_map_string_t keyword_map;

dpa_u_a_bo_unique_t ubo_H;
dpa_u_a_bo_unique_t ubo_B;

dpa_u_init static void init(void){
  ubo_H = dpa_u_bo_intern(dpa_u_to_a_bo_any_static(((dpa_u_bo_t){.data="H", .size=1})));
  ubo_B = dpa_u_bo_intern(dpa_u_to_a_bo_any_static(((dpa_u_bo_t){.data="B", .size=1})));
  for(int i=0; i<KEYWORD_COUNT; i++){
    if(g_keyword.init[i]){
      dpa_u_a_bo_unique_t ubo = dpa_u_bo_intern(dpa_u_to_a_bo_any_static(((dpa_u_bo_t){.data=g_keyword.init[i], .size=strlen(g_keyword.init[i])})));
      g_keyword.id[i] = ubo;
      dpa_u_map_set(&keyword_map, ubo, i);
    }
  }
  for(int i=1; i<TOKEN_BLOCK_LAST-TOKEN_BLOCK_FIRST; i++){
    tb_list.id[i].open  = dpa_u_bo_intern(dpa_u_to_a_bo_any_static(((dpa_u_bo_t){.data=tb_list.init[i].open , .size=strlen(tb_list.init[i].open )})));
    tb_list.id[i].close = dpa_u_bo_intern(dpa_u_to_a_bo_any_static(((dpa_u_bo_t){.data=tb_list.init[i].close, .size=strlen(tb_list.init[i].close)})));
  }
}

void print_token(struct token* t, int level){
  printf("%03zu:%03zu:%*s ", t->line, t->character, level, "");
  switch(t->type){
    if(0) case TOKEN_SPACE: printf("%-17s", "TOKEN_SPACE");
    if(0){
      if(0) case TOKEN_COMMENT   : printf("%-17s// ", "TOKEN_COMMENT");
      if(0) case TOKEN_STRING    : printf("%-17s`", "TOKEN_STRING");
      if(0) case TOKEN_IDENTIFIER: printf("%-17s", "TOKEN_IDENTIFIER");
      if(0) case TOKEN_KEYWORD   : printf("%-17s", "TOKEN_KEYWORD");
      if(0) case TOKEN_OPERATOR  : printf("%-17s", "TOKEN_OPERATOR");
      struct token_string* ts = (struct token_string*)t;
      dpa_u_bo_t bo = dpa_u_to_bo(ts->value);
      fwrite(bo.data, 1, bo.size, stdout);
      if(t->type == TOKEN_STRING)
        printf("`");
      if(ts->id)
        printf(" (%d)", ts->id);
    }
    if(0) case TOKEN_CHARACTER: {
      printf("%-17s", "TOKEN_CHARACTER");
    }
    if(0) case TOKEN_INTEGER: {
      printf("%-17s", "TOKEN_INTEGER");
      mpz_out_str(stdout, 10, ((struct token_integer*)t)->value);
    }
    puts("");
    if(0) case TOKEN_BLOCK: {
      struct token_block* b = (struct token_block*)t;
      printf(
        "%-17s%.*s %.*s\n", "TOKEN_BLOCK",
        (int)dpa_u_bo_get_size(TB(b->block_type).open), (char*)dpa_u_bo_get_data(TB(b->block_type).open),
        (int)dpa_u_bo_get_size(TB(b->block_type).close), (char*)dpa_u_bo_get_data(TB(b->block_type).close)
      );
      struct token** tokens = dpa_u_list_data(&b->token_list);
      size_t size = dpa_u_list_length(&b->token_list);
      for(size_t i=0; i<size; i++)
        print_token(tokens[i], level+2);
    }
  }
}

int hex2int(dpa_u_unicode_codepoint_t ch){
  if(ch >= '0' && ch <= '9')
    return ch - '0';
  if(ch >= 'a' && ch <= 'f')
    return ch - 'a' + 10;
  if(ch >= 'A' && ch <= 'F')
    return ch - 'A' + 10;
  return -1;
}

void free_token(struct token* t){
  switch(t->type){
    case TOKEN_SPACE: break;
    case TOKEN_COMMENT:
    case TOKEN_IDENTIFIER:
    case TOKEN_OPERATOR:
    case TOKEN_KEYWORD:
    case TOKEN_STRING:
      dpa_u_bo_put(((struct token_string*)t)->value);
      break;
    case TOKEN_CHARACTER: break;
    case TOKEN_INTEGER: {
      mpz_clear(((struct token_integer*)t)->value);
    } break;
    case TOKEN_BLOCK: {
      struct token_block* b = (struct token_block*)t;
      struct token** tokens = dpa_u_list_data(&b->token_list);
      size_t size = dpa_u_list_length(&b->token_list);
      for(size_t i=0; i<size; i++)
        free_token(tokens[i]);
      dpa_u_list_clear(&b->token_list);
    } break;
  }
  free(t);
}

void lex_destroy(struct lexer* lexer){
  struct token** tokens = dpa_u_list_data(&lexer->block.token_list);
  for(size_t i=0, n=dpa_u_list_length(&lexer->block.token_list); i<n; i++)
    free_token(tokens[i]);
  // TODO: token_block
  dpa_u_list_clear(&lexer->block.token_list);
}


bool lex(FILE* f, struct lexer* result){
  bool ret = true;
  
  dpa_u_list(struct token_block*) block_stack = {0};
  struct token_block* c = &result->block;
  c->super.type = TOKEN_BLOCK;
  dpa_u_list_append(&block_stack, 1, &c);

  dpa_u_a_bo_unique_t close_block = {0};

  enum lexer_state {
    LEX_NONE,
    LEX_SPACE,
    LEX_COMMENT,
    LEX_COMMENT_MULTILINE,
    LEX_IDENTIFIER,
    LEX_STRING_QUOTED_MAYBE_INDENTED,
    LEX_STRING_QUOTED,
    LEX_STRING_HEX,
    LEX_STRING_BASE64,
    LEX_CHARACTER,
    LEX_NUMBER_START,
    LEX_NUMBER,
    LEX_OPERATOR,
    LEX_BLOCK_START,
    LEX_BLOCK_END,
  };

  size_t line=1, character=1;
  unsigned char buf[256];
  const unsigned char*const end = buf + sizeof(buf);
  unsigned char* wit = buf;
  int eof = 7;
  int tmp1, tmp2, tmp3;
  dpa_u_list(unsigned char) string = {0};
  dpa_u_list(unsigned char) tmpstr = {0};
  enum lexer_state state = LEX_NONE;
  struct token* current_token = 0;
  struct token_string* last_string_token = 0;

  while(eof){
    int new_len = fread(wit, 1, end-wit, f);
    if(new_len != end-wit){
      if(ferror(f)){
        fprintf(stderr, "lex failed: fread failed\n");
        goto failed;
      }
      eof = 0;
    }
    wit += new_len;
    const unsigned char* rit = buf;
    while(wit-rit > eof){
      const unsigned char*const cit = rit;
      const int max_len = wit - cit;
      const dpa_u_unicode_codepoint_t codepoint = dpa_u_utf8_next_codepoint(&rit, wit);
      if(codepoint == '\r')
        continue;
      if(codepoint == -1){
        fprintf(stderr, "%zu:%zu: Error: Invalid encoding, not valid UTF-8\n", line, character);
        goto failed;
      }
      if(dpa_u_unicode_is_invalid(codepoint)){
        fprintf(stderr, "%zu:%zu: Error: Codepoint U+%"PRIX32" is not allowed in unicode!\n", line, character, codepoint);
        goto failed;
      }
      if(dpa_u_unicode_is_noncharacter(codepoint)){
        fprintf(stderr, "%zu:%zu: Error: Unicode non-character U+%"PRIX32"!\n", line, character, codepoint);
        goto failed;
      }
    reevaluate:;
      // printf("%X\n", codepoint);
      bool reevaluate = false;
      switch(state){
        case LEX_NONE: {
          if(codepoint == '`' || codepoint == '"'){
            // TODO: check if last_token is H or B
            tmp1 = codepoint;
            {
              size_t token_count = dpa_u_list_length(&c->token_list);
              struct token** tokens = dpa_u_list_data(&c->token_list);
              if(token_count && tokens[token_count-1]->type == TOKEN_IDENTIFIER && dpa_u_bo_is_equal(((struct token_string*)(tokens[token_count-1]))->value, ubo_H)){
                tmp2 = -1;
                state = LEX_STRING_HEX;
                free_token(tokens[token_count-1]);
                dpa_u_list_shrink(&c->token_list, 1);
              }else if(token_count && tokens[token_count-1]->type == TOKEN_IDENTIFIER && dpa_u_bo_is_equal(((struct token_string*)(tokens[token_count-1]))->value, ubo_B)){
                tmp2 = 0;
                tmp3 = 0;
                state = LEX_STRING_BASE64;
                free_token(tokens[token_count-1]);
                dpa_u_list_shrink(&c->token_list, 1);
              }else{
                if(max_len >= 2 && cit[1] == '\n'){
                  state = LEX_STRING_QUOTED_MAYBE_INDENTED;
                }else{
                  state = LEX_STRING_QUOTED;
                }
              }
            }
            size_t token_count = dpa_u_list_length(&c->token_list);
            struct token** tokens = dpa_u_list_data(&c->token_list);
            if(token_count && tokens[token_count-1]->type == TOKEN_STRING){
              current_token = tokens[token_count-1];
            }else{
              if(last_string_token){
                dpa_u_bo_t bo = {
                  .size = dpa_u_list_length(&string),
                  .data = dpa_u_list_data(&string),
                };
                dpa_u_a_bo_unique_t ubo = dpa_u_bo_intern(bo);
                ((struct token_string*)last_string_token)->value = ubo;
              }
              dpa_u_list_clear(&string);
              current_token = calloc(1, sizeof(struct token_string));
              last_string_token = (struct token_string*)current_token;
              current_token->type = TOKEN_STRING;
              goto token_start;
            }
            break;
          }
          reevaluate = true;
          if('0' <= codepoint && '9' >= codepoint){
            state = LEX_NUMBER_START;
            current_token = calloc(1, sizeof(struct token_integer));
            current_token->type = TOKEN_INTEGER;
            mpz_init(((struct token_integer*)current_token)->value);
            goto token_start;
          }
          if(max_len >= 2 && !memcmp("//", cit, 2)){
            state = LEX_COMMENT;
            current_token = 0;
            // current_token = calloc(1, sizeof(struct token_string));
            // current_token->type = TOKEN_COMMENT;
            rit =  cit + 2;
            character++;
            goto token_start;
          }
          if(max_len >= 2 && !memcmp("/*", cit, 2)){
            state = LEX_COMMENT_MULTILINE;
            current_token = 0;
            // current_token = calloc(1, sizeof(struct token_string));
            // current_token->type = TOKEN_COMMENT;
            rit =  cit + 2;
            character++;
            goto token_start;
          }
          const enum dpa_u_unicode_category category = dpa_u_unicode_get_category(codepoint);
          const enum dpa_u_unicode_category_major major = category & DPA_U_UNICODE_CATEGORY_MAJOR_MASK;
          if(major == DPA_U_UC_MAJOR_LETTER || category == DPA_U_UC_PUNCTUATION_CONNECTOR || category == DPA_U_UC_NUMBER_LETTER){
            state = LEX_IDENTIFIER;
            current_token = calloc(1, sizeof(struct token_string));
            current_token->type = TOKEN_IDENTIFIER;
            goto token_start;
          }
          if(codepoint == '\n' || codepoint == '\r' || codepoint == '\t' || codepoint == ' ' || major == DPA_U_UC_MAJOR_SEPERATOR){
            state = LEX_SPACE;
            current_token = 0;
            // current_token = calloc(1, sizeof(struct token));
            // current_token->type = TOKEN_SPACE;
            goto token_start;
          }
          if(!dpa_u_bo_is_error(close_block)){
            int size = dpa_u_bo_get_size(close_block);
            const char* data = dpa_u_bo_get_data(close_block);
            if(size <= max_len && !memcmp(cit, data, size)){
              state = LEX_BLOCK_END;
              current_token = 0;
              rit = cit + size;
              goto token_start;
            }
          }
          for(int i=1; i<TOKEN_BLOCK_LAST-TOKEN_BLOCK_FIRST; i++){
            dpa_u_a_bo_unique_t op = tb_list.id[i].open;
            int size = dpa_u_bo_get_size(op);
            const char* data = dpa_u_bo_get_data(op);
            if(size <= max_len && !memcmp(cit, data, size)){
              state = LEX_BLOCK_START;
              struct token_block* tb = calloc(1, sizeof(*tb));
              current_token = &tb->super;
              current_token->type = TOKEN_BLOCK;
              tb->block_type = i + TOKEN_BLOCK_FIRST;
              dpa_u_list_append(&block_stack, 1, &tb);
              close_block = tb_list.id[i].close;
              rit = cit + size;
              goto token_start;
            }
          }
          for(int i=OPERATOR_FIRST; i<OPERATOR_LAST; i++){
            dpa_u_a_bo_unique_t op = g_keyword.id[i];
            if(dpa_u_bo_is_error(op)) continue;
            int size = dpa_u_bo_get_size(op);
            const char* data = dpa_u_bo_get_data(op);
            if(size <= max_len && !memcmp(cit, data, size)){
              if(i == TOKEN_OP_LOGIC_AND && 3 <= max_len && cit[2] == '&')
                continue; // &&& -> & && (instead of && &)
              state = LEX_OPERATOR;
              current_token = calloc(1, sizeof(struct token_string));
              current_token->type = TOKEN_OPERATOR;
              struct token_string* tstr = (struct token_string*)current_token;
              tstr->value = op;
              tstr->id = i;
              rit = cit + size;
              goto token_start;
            }
          }
          fprintf(stderr, "%zu:%zu: unexpected token U+%04X '%.*s'\n", line, character, codepoint, (int)(rit-cit), cit);
          goto failed;
        } break;
        case LEX_SPACE: {
          if(codepoint == '\n' || codepoint == '\r' || codepoint == '\t' || codepoint == ' ')
            break;
          const enum dpa_u_unicode_category category = dpa_u_unicode_get_category(codepoint);
          const enum dpa_u_unicode_category_major major = category & DPA_U_UNICODE_CATEGORY_MAJOR_MASK;
          if(major == DPA_U_UC_MAJOR_SEPERATOR)
            break;
          reevaluate = true;
          goto end_token;
        } break;
        case LEX_COMMENT: {
          if(codepoint == '\n' || codepoint == 0x2028 || codepoint == 0x2029){
            goto end_token;
          }
          break;
        }
        case LEX_COMMENT_MULTILINE: {
          if(max_len >= 2 && !memcmp("*/", cit, 2)){
            rit = cit + 2;
            character += 1;
            goto end_token;
          }
        } break;
        case LEX_STRING_QUOTED_MAYBE_INDENTED: {
          
        } break;
        case LEX_STRING_QUOTED: {
          if(codepoint == '\\'){
            if(max_len < 2){
              fprintf(stderr, "%zu:%zu: Incomplete string literal\n", line, character);
              goto failed;
            }
            rit = cit + 2;
            character += 1;
            unsigned char ch;
            switch(cit[1]){
              case '\n': break;
              {
                if(0) case 'a': ch='\a';
                if(0) case 'b': ch='\b'; 
                if(0) case 'e': ch='\x1B';
                if(0) case 'f': ch='\f';
                if(0) case 'r': ch='\r';
                if(0) case 'n': ch='\n';
                if(0) case 't': ch='\t';
                if(0) case 'v': ch='\v';
                if(0) case 'x': {
                  if(max_len < 4){
                    fprintf(stderr, "%zu:%zu: Incomplete string literal\n", line, character);
                    goto failed;
                  }
                  rit = cit + 4;
                  ch = 0;
                  int x;
                  x = hex2int(cit[2]);
                  if(x == -1) goto bad_escape;
                  ch = x<<4;
                  x = hex2int(cit[3]);
                  if(x == -1) goto bad_escape;
                  ch |= x;
                }
                if(0) case '`': ch='`';
                if(0) case '"': ch='"';
                if(0) case '\\': ch='\\';
                dpa_u_list_append(&string, 1, &ch);
              } break;
              default: bad_escape: {
                fprintf(stderr, "%zu:%zu: Invalid escape sequence '\\%.*s'\n", line, character, (int)(rit-cit), cit);
              } goto failed;
            }
          }else if(codepoint == tmp1){
            goto end_token;
          }else{
            dpa_u_list_append(&string, rit-cit, cit);
          }
        } break;
        case LEX_STRING_BASE64: {
          if(codepoint == tmp1){
            switch(tmp3){
              case 1:
                fprintf(stderr, "%zu:%zu: Base64: unexpected end\n", line, character);
                goto failed;
              case 0:
              case 2:
              case 3:
              case 4:
                if(tmp2 & 0xFF){
                  fprintf(stderr, "%zu:%zu: Base64 decoding error: previous character contained data for next character, but got EOF instead of encoded data!\n", line, character);
                  goto failed;
                }
                break;
            }
            goto end_token;
          }
          if(codepoint == '\n' || codepoint == '\r' || codepoint == '\t' || codepoint == ' ')
            break;
          const enum dpa_u_unicode_category category = dpa_u_unicode_get_category(codepoint);
          const enum dpa_u_unicode_category_major major = category & DPA_U_UNICODE_CATEGORY_MAJOR_MASK;
          if(major == DPA_U_UC_MAJOR_SEPERATOR)
            break;
          int x = 0;
          if(codepoint >= 'A' && codepoint <= 'Z'){
            x = codepoint - 'A';
          }else if(codepoint >= 'a' && codepoint <= 'z'){
            x = codepoint - 'a' + 26;
          }else if(codepoint >= '0' && codepoint <= '9'){
            x = codepoint - '0' + 52;
          }else if(codepoint == '+' || codepoint == '-'){
            x = 62;
          }else if(codepoint == '/' || codepoint == '_'){
            x = 63;
          }else if(codepoint == '='){
            switch(tmp3++){
              case 0:
              case 1:
                fprintf(stderr, "%zu:%zu: Base64 padding shouldn't appear here\n", line, character);
                goto failed;
              case 2:
                tmp3=4;
                if(0) case 3: case 4: {
                  tmp2=0;
                  tmp3=0;
                }
                if(tmp2 & 0xFF){
                  fprintf(stderr, "%zu:%zu: Base64 decoding error: previous character contained data for next character, but got padding bytes instead of encoded data!\n", line, character);
                  goto failed;
                }
            }
            break;
          }else{
            fprintf(stderr, "%zu:%zu: Not a valid base64 digit '\\%.*s'\n", line, character, (int)(rit-cit), cit);
            goto failed;
          }
          switch(tmp3++){
            case 0: {
              tmp2 = x<<2;
            } break;
            case 1: {
              dpa_u_list_append(&string, 1, (unsigned char[]){tmp2 | (x>>4)});
              tmp2 = x<<4;
            }; break;
            case 2: {
              dpa_u_list_append(&string, 1, (unsigned char[]){tmp2 | (x>>2)});
              tmp2 = x<<6;
            }; break;
            case 3: {
              dpa_u_list_append(&string, 1, (unsigned char[]){tmp2 | x});
              tmp2 = 0;
              tmp3 = 0;
            } break;
            case 4:
              fprintf(stderr, "%zu:%zu: Base64 padding too short\n", line, character);
              goto failed;
          }
        } break;
        case LEX_STRING_HEX: {
          if(codepoint == tmp1){
            if(tmp2 != -1){
              fprintf(stderr, "%zu:%zu: Hex string literal must contain an even number of digits\n", line, character);
              goto failed;
            }
            goto end_token;
          }
          if(codepoint == '\n' || codepoint == '\r' || codepoint == '\t' || codepoint == ' ')
            break;
          const enum dpa_u_unicode_category category = dpa_u_unicode_get_category(codepoint);
          const enum dpa_u_unicode_category_major major = category & DPA_U_UNICODE_CATEGORY_MAJOR_MASK;
          if(major == DPA_U_UC_MAJOR_SEPERATOR)
            break;
          int ch = hex2int(codepoint);
          if(ch == -1){
            fprintf(stderr, "%zu:%zu: Not a valid hex digit '\\%.*s'\n", line, character, (int)(rit-cit), cit);
            goto failed;
          }
          if(tmp2 == -1){
            tmp2 = ch;
          }else{
            ch = (tmp2<<4) | ch;
            tmp2 = -1;
            dpa_u_list_append(&string, 1, (unsigned char[]){ch});
          }
        } break;
        case LEX_NUMBER_START: {
          state = LEX_NUMBER;
          if(max_len >= 2){
            tmp2 = 1;
            if(codepoint == '0' && cit[1]=='x'){
              tmp1 = 16;
              rit = cit + 2;
            }else if(codepoint == '0' && cit[1]=='o'){
              tmp1 = 8;
              rit = cit + 2;
            }else if(codepoint == '0' && cit[1]=='b'){
              tmp1 = 2;
              rit = cit + 2;
            }else if(cit[1]=='!'){
              tmp1 = codepoint - '0';
              rit = cit + 2;
              if(tmp1 < 2){
                fprintf(stderr, "%zu:%zu: Integer constant of base %d not allowed\n", line, character, tmp1);
                goto failed;
              }
            }else if(max_len >= 3 && cit[2]=='!' && cit[1] >= '0' && cit[1] <= '9' && codepoint != '0'){
              tmp1 = (codepoint - '0') * 10 + (cit[1] - '0');
              rit = cit + 3;
              if(tmp1 < 2 || tmp1 > 62){
                fprintf(stderr, "%zu:%zu: Integer constant of base %d not allowed\n", line, character, tmp1);
                goto failed;
              }
            }else{
              tmp2 = 0;
              tmp1 = 10;
              goto lex_number;
            }
            break;
          }
          tmp2 = 0;
        } /* fallthrough */
        case LEX_NUMBER: lex_number: {
          int x = 0;
          if(codepoint >= '0' && codepoint <= '9'){
            x = codepoint - '0';
          }else if(codepoint >= 'A' && codepoint <= 'Z'){
            x = codepoint - 'A' + 10;
          }else if(codepoint >= 'a' && codepoint <= 'z'){
            x = codepoint - 'a' + 10;
            if(tmp1 > 36) x += 26;
          }else{
            if(tmp2){
              fprintf(stderr, "%zu:%zu: Integer constant requires at least one digit\n", line, character);
              goto failed;
            }
            reevaluate = true;
            goto end_token;
          }
          if(x > tmp1){
            reevaluate = true;
            goto end_token;
          }
          tmp2 = 0;
          mpz_t* v = &((struct token_integer*)current_token)->value;
          mpz_mul_ui(*v, *v, tmp1);
          mpz_add_ui(*v, *v, x);
        } break;
        case LEX_OPERATOR: goto end_token;
        case LEX_BLOCK_START: {
          size_t size = dpa_u_list_length(&block_stack);
          c = dpa_u_list_data(&block_stack)[size-1];
        } goto end_token;
        case LEX_BLOCK_END: {
          size_t size = dpa_u_list_length(&block_stack);
          if(size < 2){
            fprintf(stderr, "%zu:%zu: LEX_BLOCK_END: underflow\n", line, character);
            goto failed;
          }
          dpa_u_list_shrink(&block_stack, 1);
          c = dpa_u_list_data(&block_stack)[size-2];
          close_block = TB(c->block_type).close;
        } goto end_token;
        case LEX_IDENTIFIER: {
          const enum dpa_u_unicode_category category = dpa_u_unicode_get_category(codepoint);
          const enum dpa_u_unicode_category_major major = category & DPA_U_UNICODE_CATEGORY_MAJOR_MASK;
          if(category == DPA_U_UC_PUNCTUATION_CONNECTOR || major == DPA_U_UC_MAJOR_LETTER || major == DPA_U_UC_MAJOR_NUMBER){
            dpa_u_list_append(&tmpstr, rit-cit, cit);
            break;
          }
          dpa_u_bo_t bo = {
            .size = dpa_u_list_length(&tmpstr),
            .data = dpa_u_list_data(&tmpstr),
          };
          dpa_u_a_bo_unique_t ubo = dpa_u_bo_intern(bo);
          struct token_string* tstr = (struct token_string*)current_token;
          tstr->value = ubo;
          const dpa_u_optional_t opt = dpa_u_map_get(&keyword_map, ubo);
          if(opt.present){
            tstr->super.type = TOKEN_KEYWORD;
            tstr->id = opt.value.u64;
          }
          dpa_u_list_clear(&tmpstr);
          reevaluate = true;
          goto end_token;
        } break;
        case LEX_CHARACTER: break;
        token_start: {
          if(current_token){
            // printf("%zu:%zu: token_start: %d\n", line, character, current_token->type);
            current_token->line = line;
            current_token->character = character;
            dpa_u_list_append(&c->token_list, 1, &current_token);
          }
        } break;
        end_token: {
          // if(current_token) printf("%zu:%zu: complete: %d\n", line, character, current_token->type);
          state = LEX_NONE;
          current_token = 0;
        } break;
      }
      if(reevaluate)
        goto reevaluate;
      // printf("%zu:%zu: %X %02o %c\n", line, character, codepoint, category, codepoint);
      if(codepoint == '\n'){
        line += 1;
        character = 0;
      }
      character += 1;
    }
    memmove(buf, rit, wit-rit);
    wit = buf + (wit-rit);
  }
  
  if(last_string_token){
    dpa_u_bo_t bo = {
      .size = dpa_u_list_length(&string),
      .data = dpa_u_list_data(&string),
    };
    dpa_u_a_bo_unique_t ubo = dpa_u_bo_intern(bo);
    ((struct token_string*)last_string_token)->value = ubo;
  }

  if(dpa_u_list_length(&block_stack) != 1){
    fprintf(stderr, "Error: Unexpected EOF: Not all braces closed\n");
    goto failed;
  }

  switch(state){
    case LEX_SPACE:
    case LEX_COMMENT: break;
    case LEX_IDENTIFIER: {
      dpa_u_bo_t bo = {
        .size = dpa_u_list_length(&tmpstr),
        .data = dpa_u_list_data(&tmpstr),
      };
      dpa_u_a_bo_unique_t ubo = dpa_u_bo_intern(bo);
      ((struct token_string*)current_token)->value = ubo;
      dpa_u_list_clear(&tmpstr);
    } break;
    case LEX_NUMBER: {
      
    } break;
    default: {
      fprintf(stderr, "Error: Unexpected EOF\n");
      goto failed;
    } break;
  }

  if(0) failed: ret = false;

  dpa_u_list_clear(&tmpstr);
  dpa_u_list_clear(&string);

  if(!ret)
    lex_destroy(result);

  return ret;
}
