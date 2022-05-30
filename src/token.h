#ifndef TOKEN_H
#define TOKEN_H

#include "core/types.h"
#include "core/array.h"
#include "external/vec.h"
#include "core/str.h"

typedef enum {
    TOKEN_NULL,
    TOKEN_INVALID,
    TOKEN_SYMBOL,
    TOKEN_CHAR_LITERAL,
    TOKEN_INT_LITERAL,
    TOKEN_FLOAT_LITERAL,
    TOKEN_STRING_LITERAL,
    TOKEN_LABEL
} TokenTag;

typedef enum {
    SYMBOL_INCREASE,
    SYMBOL_DECREASE,
    SYMBOL_PARANTHESIS_BEGIN,
    SYMBOL_PARANTHESIS_END,
    SYMBOL_BRACE_BEGIN,
    SYMBOL_BRACE_END,
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

    SYMBOL_COLON_COLON,
    SYMBOL_COLON,
    SYMBOL_COLON_ASSIGN,

    SYMBOL_SEMICOLON
} Symbol;

// typedef union {
//     Symbol _symbol;
//     char _char;
//     int _int;
//     float _float;
//     char* _string;
// } token_value_t;

typedef struct {
    union {
        Symbol _symbol;
		char _char;
		int _int;
		float _float;
        Str _string;
	};
    TokenTag tag;
    int line_number;
} Token;
typedef Array(Token) TokenArray;

static Token create_token_symbol(Symbol value) {
    return (Token){
        .tag = TOKEN_SYMBOL,
        ._symbol = value,
        .line_number = 0,
    };
};

static Token create_token_char_literal(char value) {
    Token token;
    token._char = value;
    token.tag = TOKEN_CHAR_LITERAL;
	token.line_number = 0;
    return token;
};

static Token create_token_int(int value) {
    Token token;
    token._int = value;
    token.tag = TOKEN_INT_LITERAL;
	token.line_number = 0;
    return token;
};

static Token create_token_float(float value) {
    Token token;
    token._float = value;
    token.tag = TOKEN_FLOAT_LITERAL;
	token.line_number = 0;
    return token;
};

static Token create_token_label(Str value) {
    Token token;
    token._string = value;
    token.tag = TOKEN_LABEL;
	token.line_number = 0;
    return token;
};

static Token create_token_string_literal(Str value) {
    Token token;
    token._string = value;
    token.tag = TOKEN_STRING_LITERAL;
	token.line_number = 0;
    return token;
};

static bool token_is_symbol(Token token, Symbol symbol) {
    if (token.tag != TOKEN_SYMBOL) return false;
    return (token._symbol == symbol);
}

#endif // TOKEN_H
