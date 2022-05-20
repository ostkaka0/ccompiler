#include "generate_c.h"
#include "core/array.h"
#include "core/str.h"

#include <stdlib.h>

static void generate_c_expr(Str* str, Expr expr);


void array_push_string(u8Array* string_stream, const char* string) {
    char* c = (char*)string;
    while(*c) {
        array_push(*string_stream, *c);
        c++;
    }
}

Str generate_c(const ExprArray ast) {
    Str r = str_empty(temp_allocator);

    r = str_concat(r, str("int main() { "), temp_allocator);

    for(int i = 0; i < ast.len; i++) {
        generate_c_expr(&r, ast.at[i]);
        r = str_concat(r, str("; "), temp_allocator);
    }

    r = str_concat(r, str(" return 0; }"), temp_allocator);
    return r;
}

static void generate_c_expr(Str* s, Expr expr) {
    switch(expr.tag) {
    case EXPR_INT_LITERAL: {
        *s = str_print(temp_allocator, "%.*s%i", s->len, s->at, expr._int);
        break;

    // Operators:
    case EXPR_ADD:
    case EXPR_SUBTRACT:
    case EXPR_MULTIPLY:
    case EXPR_DIVIDE:
    case EXPR_MODULO: {
        *s = str_concat_char(*s, '(', temp_allocator);
        generate_c_expr(s, *expr._child_pair[0]);
        char operator_char = '#';
        switch(expr.tag) {
            case EXPR_ADD:      operator_char = '+'; break;
            case EXPR_SUBTRACT: operator_char = '-'; break;
            case EXPR_MULTIPLY: operator_char = '*'; break;
            case EXPR_DIVIDE:   operator_char = '/'; break;
            case EXPR_MODULO:   operator_char = '%'; break;
        }
        *s = str_concat_char(*s, operator_char, temp_allocator);
        generate_c_expr(s, *expr._child_pair[1]);
        *s = str_concat_char(*s, ')', temp_allocator);
        } break;


    // _declarations:
    case EXPR_DECL_DATATYPE:
        *s = str_concat(*s, datatype_to_str_as_c(expr.datatype), temp_allocator);
        break;
    case EXPR_DECL_VARIABLE:
        *s = str_concat(*s, datatype_to_str_as_c(expr.datatype), temp_allocator);
        *s = str_concat_char(*s, ' ', temp_allocator);
        *s = str_concat(*s, expr._string, temp_allocator);
        break;
    case EXPR_DECL_VARIABLE_ASSIGN:
        generate_c_expr(s, *expr._child_pair[0]);
        *s = str_concat(*s, str(" = "), temp_allocator);
        generate_c_expr(s, *expr._child_pair[1]);
        break;
        }
    }
 }
