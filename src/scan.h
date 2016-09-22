#ifndef SCAN_H
#define SCAN_H

#include <stdio.h>
#include <ctype.h>

#include "macro.h"
#include "token.h"
#include "external/vec.h"

static vec_token_t scan(const char* path) {
    vec_token_t tokens;
	vec_init(&tokens);
    
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Failed to open file '%s'!", path);
        return tokens;
    }
    
    char buffer[2048];
    size_t index = 0;
    int line_number = 1;
    
    {
        int res = fread(buffer, sizeof(char), 2048, file);
        if (res != 2048) // Mark EOF(end of file)
            buffer[res] = EOF;
    }
    
    for (;;) {

        if (index >= 1024) {
          memcpy(buffer, &buffer[index], 2048-index);
          int res = fread(&buffer[2048-index], sizeof(char), index, file);
          if (res != index) // Mark EOF(end of file)
              buffer[2048-index+res] = EOF;
          index = 0;
        }
        
        token_t token;
        size_t length = 1;
        
        char c = buffer[index];
        
        // Skip whitespace
        while (isspace(c)) {
            if (c == '\n')
                line_number++;
            c = buffer[++index];
        }
        
        // End of file
        if (c == EOF || c == 0)
            break;
        
        printf("%c ", c);
        
        // Text/Name
        if (isalpha(c) || c == '_') {
            length = 0;
            do {
                length++;
                c = buffer[index+length];
                if (index + length >= 2048) // TODO: Compile error
                    runtime_error(line_number, "Name is longer than max length.");
            }
            while (isalpha(c) || c == '_');
            char* text = malloc(length+1);
            text[length] = '\0';
            memcpy(text, &buffer[index], length * sizeof(char));
            token = create_token_label(text);
        }
        // _int, Float
        else if (isdigit(c) || c == '.') {
            length = 0;
            
            while (isdigit(c)) {
                length++;
                c = buffer[index+length];
                if (index + length >= 2048) // TODO: Compile error
                    runtime_error(line_number, "Digit is longer than max length.");
            }
            
            if (c == '.') {
                do {
                    length++;
                    c = buffer[index+length];
                    if (index + length >= 2048) // TODO: Compile error
                        runtime_error(line_number, "Digit is longer than max length.");
                } while(isdigit(c) || c == '.');
                token = create_token_float(atof(&buffer[index]));
            }
            else
                token = create_token_int(atoi(&buffer[index]));
        }
        // Symbol:
        else if (ispunct(c)) {
            switch(c) {
                case '+':
                    if (buffer[index+1] == '+') {
                        token = create_token_symbol(SYMBOL_INCREASE);
                        length = 2;
                        break;
                    }
                    token = create_token_symbol(SYMBOL_ADD);
                    break;
                case '-':
                    if (buffer[index+1] == '+') {
                        token = create_token_symbol(SYMBOL_DECREASE);
                        length = 2;
                        break;
                    }
                    token = create_token_symbol(SYMBOL_SUBTRACT);
                    break;
                case '(':
                    token = create_token_symbol(SYMBOL_PARANTHESIS_BEGIN);
                    break;
                case ')':
                    token = create_token_symbol(SYMBOL_PARANTHESIS_END);
                    break;
                case '*':
                    token = create_token_symbol(SYMBOL_MULTIPLY);
                    break;
                case '/': // TODO: Comments
                    token = create_token_symbol(SYMBOL_DIVIDE);
                    break;
                case '%': // TODO: Comments
                    token = create_token_symbol(SYMBOL_MODULO);
                    break;
                case ';':
                    token = create_token_symbol(SYMBOL_SEMICOLON);
                    break;
                case '=':
                    token = create_token_symbol(SYMBOL_ASSIGN);
                    break;
                
                default:
                    break;
            }
        }
        
        /*if (strcmp(buffer[index], "++") == 0)
            token = { SYMBOL_INCREASE, TOKEN_OPERATOR }; length = 2;
        else if (strcmp(data[index], "--") == 0)
            token = { SYMBOL_DECREASE, TOKEN_OPERATOR }; length = 2;
        else if (strcmp(data[index], "(") == 0)
            token = { SYMBOL_PARANTHESIS_BEGIN, TOKEN_OPERATOR };
        else if (strcmp(data[index], ")") == 0)
            token = { SYMBOL_PARANTHESIS_END, TOKEN_OPERATOR };
        else if (strcmp(data[index], "+") == 0)
            token = { SYMBOL_ADD, TOKEN_OPERATOR };
        else if (strcmp(data[index], "-") == 0)
            token = { SYMBOL_SUBTRACT, TOKEN_OPERATOR };
        else if (strcmp(data[index], "*") == 0)
            token = { SYMBOL_MULTIPLY, TOKEN_OPERATOR };
        else if (strcmp(data[index], "/") == 0)
            token = { SYMBOL_DIVIDE, TOKEN_OPERATOR };
        else if (data[index], '"' {
            size_t index2 = index;
            bool backslash = false;
            while(data[index2+1] != '"' || backslash) {
                index2++;
                if (backslash)
                    backslash = false;
                else if(data[index2] == '\\')
                    backslash = true;
            }
            length = index2 - index;
        }*/
            
        index += length;
        token.line_number = line_number;
        vec_push(&tokens, token);
    }
    
    printf("Lines: %i\n", line_number);
    
	fclose(file);
    return tokens;
}

#endif // SCAN_H