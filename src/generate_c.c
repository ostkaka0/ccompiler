#include "generate_c.h"

#include <stdlib.h>

static void generate_c_expr(vec_char_t* string_stream, expr_t expr);

char* generate_c(const vec_expr_t ast) {
    vec_char_t string_stream;
    vec_init(&string_stream);

    vec_push_string(&string_stream, "int main() { ");

    for(int i = 0; i < ast.length; i++) {
        generate_c_expr(&string_stream, ast.data[i]);
        vec_push_string(&string_stream, "; ");
    }

    vec_push_string(&string_stream, " return 0; }");
    printf("%x\n", (u64)string_stream.data);
    printf("%s\n", string_stream.data);
    return string_stream.data;
}

static void generate_c_expr(vec_char_t* string_stream, expr_t expr) {
    switch(expr.type) {
case EXPR_INT_LITERAL: {
    char buffer[11];
    snprintf(buffer, 11,"%i",expr._int);
    //itoa(expr._int, buffer, 10);
    vec_push_string(string_stream, buffer);
    } break;

// Operators:
case EXPR_ADD:
case EXPR_SUBTRACT:
case EXPR_MULTIPLY:
case EXPR_DIVIDE:
case EXPR_MODULO: {
    vec_push(string_stream, '(');
    generate_c_expr(string_stream, *expr._child_pair[0]);
    char operator_char = '#';
    switch(expr.type) {
        case EXPR_ADD:      operator_char = '+'; break;
        case EXPR_SUBTRACT: operator_char = '-'; break;
        case EXPR_MULTIPLY: operator_char = '*'; break;
        case EXPR_DIVIDE:   operator_char = '/'; break;
        case EXPR_MODULO:   operator_char = '%'; break;
    }
    vec_push(string_stream, operator_char);
    generate_c_expr(string_stream, *expr._child_pair[1]);
    vec_push(string_stream, ')');
    } break;


// _declarations:
case EXPR_DECL_DATATYPE:
    vec_push_string(string_stream, datatype_to_string(expr.datatype));
    break;
case EXPR_DECL_VARIABLE:
    vec_push_string(string_stream, datatype_to_string(expr.datatype));
    vec_push(string_stream, ' ');
    vec_push_string(string_stream, expr._string);
    break;
case EXPR_DECL_VARIABLE_ASSIGN:
    generate_c_expr(string_stream, *expr._child_pair[0]);
    vec_push_string(string_stream, " = ");
    generate_c_expr(string_stream, *expr._child_pair[1]);
    break;
    }
}
