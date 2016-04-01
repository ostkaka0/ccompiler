#pragma once

#include "Macro.h"
#include "Token.h"

static std::vector<Token> scan(const char* path) {
    std::vector<Token> tokens;
    
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Failed to open file '%s'!", path);
        return {};
    }
    DEFER( fclose(file); )
    
    char buffer[2048];
    size_t index = 0;
    u32 lineNumber = 1;
    
    {
        int res = fread(buffer, sizeof(char), 2048, file);
        if (res != 2048) // Mark EOF(end of file)
            buffer[res] = EOF;
    }
    
    while (true) {

        if (index >= 1024) {
          memcpy(buffer, &buffer[index], 2048-index);
          int res = fread(&buffer[2048-index], sizeof(char), index, file);
          if (res != index) // Mark EOF(end of file)
              buffer[2048-index+res] = EOF;
          index = 0;
        }
        
        Token token;
        size_t length = 1;
        
        char c = buffer[index];
        
        // Skip whitespace
        while (isspace(c)) {
            if (c == '\n')
                lineNumber++;
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
                    runtimeError(lineNumber, "Name is longer than max length.");
            }
            while (isalpha(c) || c == '_');
            char* text = new char[length+1];
            text[length] = '\0';
            memcpy(text, &buffer[index], length * sizeof(char));
            token = createTokenText(text);
        }
        // Integer, Float
        else if (isdigit(c) || c == '.') {
            length = 0;
            
            while (isdigit(c)) {
                length++;
                c = buffer[index+length];
                if (index + length >= 2048) // TODO: Compile error
                    runtimeError(lineNumber, "Digit is longer than max length.");
            }
            
            if (c == '.') {
                do {
                    length++;
                    c = buffer[index+length];
                    if (index + length >= 2048) // TODO: Compile error
                        runtimeError(lineNumber, "Digit is longer than max length.");
                } while(isdigit(c) || c == '.');
                token = createToken((double)atof(&buffer[index]));
            }
            else
                token = createToken((int)atol(&buffer[index]));
        }
        // Symbol:
        else if (ispunct(c)) {
            switch(c) {
                case '+':
                    if (buffer[index+1] == '+') {
                        token = createTokenSymbol(SYMBOL_INCREASE);
                        length = 2;
                        break;
                    }
                    token = createTokenSymbol(SYMBOL_ADD);
                    break;
                case '-':
                    if (buffer[index+1] == '+') {
                        token = createTokenSymbol(SYMBOL_DECREASE);
                        length = 2;
                        break;
                    }
                    token = createTokenSymbol(SYMBOL_SUBTRACT);
                    break;
                case '(':
                    token = createTokenSymbol(SYMBOL_PARANTHESIS_BEGIN);
                    break;
                case ')':
                    token = createTokenSymbol(SYMBOL_PARANTHESIS_END);
                    break;
                case '*':
                    token = createTokenSymbol(SYMBOL_MULTIPLY);
                    break;
                case '/': // TODO: Comments
                    token = createTokenSymbol(SYMBOL_DIVIDE);
                    break;
                case '%': // TODO: Comments
                    token = createTokenSymbol(SYMBOL_MODULO);
                    break;
                case ';':
                    token = createTokenSymbol(SYMBOL_SEMICOLON);
                    break;
                case '=':
                    token = createTokenSymbol(SYMBOL_ASSIGN);
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
        token.lineNumber = lineNumber;
        tokens.push_back(token);
    }
    
    printf("Lines: %i\n", lineNumber);
    
    return tokens;
}