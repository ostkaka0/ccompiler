#ifndef AST_H
#define AST_H

#include "types.h"
#include "datatype.h"
#include "external/vec.h"

struct expr_t_tag;

typedef enum {
    // Null expression:
    EXPR_NULL = 0,
    // Literals:
    EXPR_INT_LITERAL,
    EXPR_FLOAT32_LITERAL,
    EXPR_FLOAT64_LITERAL,
    
    // Operators:
    EXPR_ADD,
    EXPR_SUBTRACT,
    EXPR_MULTIPLY,
    EXPR_DIVIDE,
    EXPR_MODULO,
    
    // _declarations:
    EXPR_DECL_DATATYPE,
    EXPR_DECL_VARIABLE,
    EXPR_DECL_VARIABLE_ASSIGN,
} expr_type_t;

typedef union {
    s64 _int;
    double _float;
    char* _str;
    struct expr_t_tag* _child_pair[2];
    struct expr_t_tag* _child;
} expr_value_t;

// expression
typedef struct expr_t_tag {
    //expr_value_t value;
	union {
		s64 _int;
		double _float;
		char* _string;
		struct expr_t_tag* _child_pair[2];
		struct expr_t_tag* _child;
	};
    expr_type_t type;
    int line_number;
    datatype_t datatype;
} expr_t;
typedef vec_t(expr_t) vec_expr_t;

static expr_t create_expr_int_literal(s64 value) {
    expr_t expr;
    expr._int = value;
    expr.type = EXPR_INT_LITERAL;
    expr.datatype.type = TYPE_INT;
    expr.line_number = -1;
    return expr;
}

static expr_t create_expr_float_literal(float value) {
    expr_t expr;
    expr._float = value;
    expr.type = EXPR_FLOAT32_LITERAL;
    expr.datatype.type = TYPE_FLOAT32;
    expr.line_number = -1;
    return expr;
}

static expr_t create_expr_operator_pair(expr_type_t type, expr_t* child_a, expr_t* child_b) {
    expr_t expr;
    expr._child_pair[0] = child_a;
    expr._child_pair[1] = child_b;
    expr.type = type;
	expr.datatype.type = TYPE_UNEVALUATED;
    expr.line_number = -1;
    return expr;
}

static expr_t create_expr_operator(expr_type_t type, expr_t* child) {
    expr_t expr;
    expr._child = child;
    expr.type = type;
	expr.datatype.type = TYPE_UNEVALUATED;
    expr.line_number = -1;
    return expr;
}

static expr_t create_expr_decl_datatype(datatype_t datatype) {
    expr_t expr;
    expr.type = EXPR_DECL_DATATYPE;
    expr.datatype = datatype;
    expr.line_number = -1;
    return expr;
}

static expr_t create_expr_decl_variable(datatype_t datatype, char* name) {
    expr_t expr;
    expr.type = EXPR_DECL_VARIABLE;
    expr.datatype = datatype;
    expr._string = name;
    expr.line_number = -1;
    return expr;
}

static expr_t create_expr_decl_variable_assign(expr_t expr_variable_declaration, expr_t expr_value) {
    expr_t expr;
    expr.type = EXPR_DECL_VARIABLE_ASSIGN;
	expr.datatype.type = TYPE_UNEVALUATED;
    expr._child_pair[0] = malloc(sizeof(expr_t));//new expr_t(expr_variable_declaration);
    expr._child_pair[1] = malloc(sizeof(expr_t));//new expr_t(expr_value);
    *expr._child_pair[0] = expr_variable_declaration;
    *expr._child_pair[1] = expr_value;
    expr.line_number = expr_variable_declaration.line_number;
    return expr;
}

static datatype_t evaluate_type(expr_t* expr) {
    if (expr->datatype.type != TYPE_UNEVALUATED)
        return expr->datatype;
    
    switch (expr->type) {
        case EXPR_ADD:
        case EXPR_SUBTRACT:
        case EXPR_MULTIPLY:
        case EXPR_DIVIDE:
        case EXPR_MODULO: 
        case EXPR_DECL_VARIABLE_ASSIGN: {
            datatype_t a = evaluate_type(expr->_child_pair[0]);
            datatype_t b = evaluate_type(expr->_child_pair[1]);
            
            // TODO: Implicit cast int to/from s8,s16,s32,s64. size_t,uint to/from u8,u16,u32,u64.
            
            if (!datatype_implicit_cast_cmp(a, b))
                runtime_error(expr->_child_pair[1]->line_number, "Type does not match");
            
            expr->datatype = a;
            break;
        }
        
        default:
            ERROR("Could not evaluate expression");
            break;
    }
    
    if (expr->datatype.type == TYPE_UNEVALUATED)
        runtime_error(expr->line_number, "Unevaluated type");
    
    return expr->datatype;
}

#endif // AST_H
