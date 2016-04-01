#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctype.h>

#include <vector>

#include "Token.h"
#include "Macro.h"
#include "Common.h"
#include "Scan.h"
#include "Parse.h"

int main(int argc, char **argv) {

    std::vector<Token> tokens = scan("test.ccc");
    
    for (Token token : tokens) {
        switch(token.type) {
            case TOKEN_SYMBOL:
                printf("symbol  ");
                break;
            case TOKEN_CHAR:
                printf("char %c  ", token.value._char);
                break;
            case TOKEN_INT:
                printf("int %i  ", token.value._int);
                break;
            case TOKEN_FLOAT:
                printf("float %f  ", token.value._float);
                break;
            case TOKEN_TEXT:
                printf("text %s  ", token.value._text);
                break;
            case TOKEN_STRING:
                printf("string '%s'  ", token.value._string);
                break;
        }
    }
    printf("\n");
    
    std::vector<Expr> expressions = parse(tokens);
    
    printf("\n %i \n", tokens.size());
    
    return 0;
}