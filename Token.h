#pragma once

#include "Types.h"

enum TokenType {
    TOKEN_SYMBOL,
    TOKEN_CHAR,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_TEXT,
    TOKEN_STRING
};

enum Symbol {
  SYMBOL_INCREASE,
  SYMBOL_DECREASE,
  SYMBOL_PARANTHESIS_BEGIN,
  SYMBOL_PARANTHESIS_END,
  SYMBOL_FUNCTION_CALL_END,
  SYMBOL_DOT,
  SYMBOL_ARROW,
  SYMBOL_UNARY_PLUS,
  SYMBOL_UNARY_MINUS,
  SYMBOL_LOGICAL_NOT,
  SYMBOL_BITWISE_NOT,
  SYMBOL_DEREFERENCE,
  SYMBOL_ADRESS_OF,
  SYMBOL_SIZEOF,
  SYMBOL_MULTIPLY,
  SYMBOL_DIVIDE,
  SYMBOL_MODULO,
  SYMBOL_ADD,
  SYMBOL_SUBTRACT,
  SYMBOL_BITWISE_LEFT_SHIFT,
  SYMBOL_BITWISE_RIGHT_SHIFT,
  SYMBOL_LESS_THAN,
  SYMBOL_MORE_THAN,
  SYMBOL_LESS_OR_EQUAL,
  SYMBOL_MORE_OR_EQUAL,
  SYMBOL_EQUAL,
  SYMBOL_NOT_EQUAL,
  SYMBOL_BITWISE_AND,
  SYMBOL_BITWISE_XOR,
  SYMBOL_BITTWISE_OR,
  SYMBOL_LOGICAL_AND,
  SYMBOL_LOGICAL_OR,
  SYMBOL_ASSIGN,
  SYMBOL_ASSIGN_ADD,
  SYMBOL_ASSIGN_SUBTRACT,
  SYMBOL_ASSIGN_MULTIPLY,
  SYMBOL_ASSIGN_DIVIDE,
  SYMBOL_ASSIGN_MODULO,
  SYMBOL_ASSIGN_SHIFT_LEFT,
  SYMBOL_ASSIGN_SHIFT_RIGHT,
  SYMBOL_ASSIGN_AND,
  SYMBOL_ASSIGN_XOR,
  SYMBOL_SSIGN_OR,
  
  SYMBOL_SEMICOLON
};

union TokenValue{
    Symbol _symbol;
    char _char;
    int _int;
    double _float;
    char* _text;
    char* _string;
};

struct Token {
    TokenValue value;
    TokenType type;
    u32 lineNumber = 0;
};

static Token createTokenSymbol(Symbol value) {
    Token token;
    token.value._symbol = value;
    token.type = TOKEN_SYMBOL;
    return token;
};

static Token createToken(char value) {
    Token token;
    token.value._char = value;
    token.type = TOKEN_CHAR;
    return token;
};

static Token createToken(int value) {
    Token token;
    token.value._int = value;
    token.type = TOKEN_INT;
    return token;
};

static Token createToken(double value) {
    Token token;
    token.value._float = value;
    token.type = TOKEN_FLOAT;
    return token;
};

static Token createTokenText(char* value) {
    Token token;
    token.value._text = value;
    token.type = TOKEN_TEXT;
    return token;
};

static Token createTokenString(char* value) {
    Token token;
    token.value._string = value;
    token.type = TOKEN_STRING;
    return token;
};