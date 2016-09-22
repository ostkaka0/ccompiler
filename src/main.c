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

int main(int argc, char **argv) {

    vec_token_t tokens = scan("test.ccc");
    
	token_t token;
	int i;
    vec_foreach(&tokens, token, i) {
        switch(token.type) {
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
			printf("label %s  ", token._string);
			break;
		case TOKEN_STRING_LITERAL:
			printf("string '%s'  ", token._string);
			break;
        }
    }
    printf("\n");
    
    vec_expr_t expressions = parse(&tokens);
    
    printf("\n %i \n", tokens.length);
    
    return 0;
}
