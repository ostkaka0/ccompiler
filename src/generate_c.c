#include "generate_c.h"
#include "core/array.h"

#include <stdlib.h>

static void generate_c_expr(u8Array* string_stream, Expr expr);


void array_push_string(u8Array* string_stream, const char* string) {
    char* c = (char*)string;
    while(*c) {
        array_push(*string_stream, *c);
        c++;
    }
}

char* generate_c(const ExprArray ast) {
    u8Array string_stream = {};

    array_push_string(&string_stream, "int main() { ");

    for(int i = 0; i < ast.len; i++) {
        generate_c_expr(&string_stream, ast.at[i]);
        array_push_string(&string_stream, "; ");
    }

    array_push_string(&string_stream, " return 0; }");
    return string_stream.at;
}

static void generate_c_expr(u8Array* string_stream, Expr expr) {
    switch(expr.type) {
case EXPR_INT_LITERAL: {
    char buffer[11];
    snprintf(buffer, 11,"%i",expr._int);
    //itoa(expr._int, buffer, 10);
    array_push_string(string_stream, buffer);
    } break;

// Operators:
case EXPR_ADD:
case EXPR_SUBTRACT:
case EXPR_MULTIPLY:
case EXPR_DIVIDE:
case EXPR_MODULO: {
    array_push(*string_stream, '(');
    generate_c_expr(string_stream, *expr._child_pair[0]);
    char operator_char = '#';
    switch(expr.type) {
        case EXPR_ADD:      operator_char = '+'; break;
        case EXPR_SUBTRACT: operator_char = '-'; break;
        case EXPR_MULTIPLY: operator_char = '*'; break;
        case EXPR_DIVIDE:   operator_char = '/'; break;
        case EXPR_MODULO:   operator_char = '%'; break;
    }
    array_push(*string_stream, operator_char);
    generate_c_expr(string_stream, *expr._child_pair[1]);
    array_push(*string_stream, ')');
    } break;


// _declarations:
case EXPR_DECL_DATATYPE:
    array_push_string(string_stream, datatype_to_string(expr.datatype));
    break;
case EXPR_DECL_VARIABLE:
    array_push_string(string_stream, datatype_to_string(expr.datatype));
    array_push(*string_stream, ' ');
    array_push_string(string_stream, expr._string);
    break;
case EXPR_DECL_VARIABLE_ASSIGN:
    generate_c_expr(string_stream, *expr._child_pair[0]);
    array_push_string(string_stream, " = ");
    generate_c_expr(string_stream, *expr._child_pair[1]);
    break;
    }
}
