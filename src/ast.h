#ifndef AST_H
#define AST_H

#include "core/types.h"
#include "datatype.h"
#include "core/array.h"
#include "external/vec.h"

struct Expr;

typedef enum {
    // Null expression:
    EXPR_NULL = 0,

    EXPR_IDENTIFIER,

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
    EXPR_DECL_CONST,
    EXPR_DECL_STRUCT,
    EXPR_DECL_PROCEDURE,
} ExprTag;

typedef union {
    i64 _int;
    double _float;
    char* _str;
    struct Expr* _child_pair[2];
    struct Expr* _child;
} ExprValue;

// expression
typedef struct Expr {
    //ExprValue value;
	union {
        i64 _int;
		double _float;
		char* _string;
        struct Expr* _child_pair[2];
        struct Expr* _child;
	};
    ExprTag type;
    int line_number;
    datatype_t datatype;
} Expr;
typedef Array(Expr) ExprArray;

static Expr create_expr_int_literal(i64 value) {
    Expr expr;
    expr._int = value;
    expr.type = EXPR_INT_LITERAL;
    expr.datatype.type = TYPE_INT;
    expr.line_number = -1;
    return expr;
}

static Expr create_expr_float_literal(float value) {
    Expr expr;
    expr._float = value;
    expr.type = EXPR_FLOAT32_LITERAL;
    expr.datatype.type = TYPE_FLOAT32;
    expr.line_number = -1;
    return expr;
}

static Expr create_expr_operator_pair(ExprTag type, Expr* child_a, Expr* child_b) {
    Expr expr;
    expr._child_pair[0] = child_a;
    expr._child_pair[1] = child_b;
    expr.type = type;
	expr.datatype.type = TYPE_UNEVALUATED;
    expr.line_number = -1;
    return expr;
}

static Expr create_expr_operator(ExprTag type, Expr* child) {
    Expr expr;
    expr._child = child;
    expr.type = type;
	expr.datatype.type = TYPE_UNEVALUATED;
    expr.line_number = -1;
    return expr;
}

static Expr create_expr_decl_datatype(datatype_t datatype) {
    Expr expr;
    expr.type = EXPR_DECL_DATATYPE;
    expr.datatype = datatype;
    expr.line_number = -1;
    return expr;
}

static Expr create_expr_decl_variable(datatype_t datatype, char* name) {
    Expr expr;
    expr.type = EXPR_DECL_VARIABLE;
    expr.datatype = datatype;
    expr._string = name;
    expr.line_number = -1;
    return expr;
}

static Expr create_expr_decl_variable_assign(Expr expr_variable_declaration, Expr expr_value) {
    Expr expr;
    expr.type = EXPR_DECL_VARIABLE_ASSIGN;
	expr.datatype.type = TYPE_UNEVALUATED;
    expr._child_pair[0] = malloc(sizeof(Expr));//new Expr(expr_variable_declaration);
    expr._child_pair[1] = malloc(sizeof(Expr));//new Expr(expr_value);
    *expr._child_pair[0] = expr_variable_declaration;
    *expr._child_pair[1] = expr_value;
    expr.line_number = expr_variable_declaration.line_number;
    return expr;
}

static datatype_t evaluate_type(Expr* expr) {
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
            
            // TODO: Implicit cast int to/from s8,s16,s32,i64. size_t,uint to/from u8,u16,u32,u64.
            
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
