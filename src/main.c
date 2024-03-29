#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "external/vec.h"

#include "token.h"
#include "macro.h"
#include "common.h"
#include "scan.h"
#include "parse.h"
#include "parse_c.h"
#include "generate_c.h"

#include "generate_c.c"

#define vec_foreach(v, var, iter)\
  if  ( (v)->len > 0 )\
  for ( (iter) = 0;\
        (iter) < (v)->len && (((var) = (v)->at[(iter)]), 1);\
        ++(iter))

/*
const char* g_operators[] = {
    "&", "^", "|", "+", "-", "*", "/", "%",
    "!", "~",
    "=", "+=", "&=", "^=", "|=", "+=", "-=", "*=", "/=", "%=",
    "==", "!=", "<", ">" "<=", ">=",
    "||", "&&", "||=", "&&=",
    "(", ")", "{", "}", "[", "]",
    ".", ",", ":", ";",
    "#", "@", "?", "\\", "<=>"
};*/

int main(int argc, char **argv) {
    temp_storage_init(4 * MB);
    PARSE_ERROR("The cow is hungry #%i", -1, 1337);

    TokenArray tokens = scan("test.ccc", 4);
    
    Token token;
    int i;
    vec_foreach(&tokens, token, i) {
        switch(token.tag) {
        case TOKEN_SYMBOL:
           printf("symbol  ");
           break;
        case TOKEN_CHAR_LITERAL:
			printf("char %c  ", token._char);
			break;
		case TOKEN_INT_LITERAL:
			printf("int %i  ", token._int);
			break;
		case TOKEN_FLOAT_LITERAL:
			printf("float %f  ", token._float);
			break;
		case TOKEN_LABEL:
            printf("label %s.*  ", token._string.at, token._string.len);
			break;
		case TOKEN_STRING_LITERAL:
			printf("string '%s'  ", token._string);
			break;
        }
    }
    printf("\n");
    
    u32 parse_idx = 0;
    ExprArray expressions = parse_c(&tokens, &parse_idx);
    Str c_code = generate_c(expressions);
    printf("%x\n", (u64)c_code.at);
    printf("%s\n", c_code.cstr);
    printf("c-code: \n%s", c_code.cstr);
    
    printf("\n %i \n", tokens.len);
    
    return 0;
}
