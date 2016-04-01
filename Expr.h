#pragma once

#include "Types.h"
#include "Datatype.h"

struct Expr;

enum ExprType {
    // Null expression:
    EXPR_NULL = 0,
    // Consts:
    EXPR_INTEGER,
    EXPR_FLOAT,
    
    // Operators:
    EXPR_ADD,
    EXPR_SUBTRACT,
    EXPR_MULTIPLY,
    EXPR_DIVIDE,
    EXPR_MODULO,
    
    // Declarations:
    EXPR_DECL_DATATYPE,
    EXPR_DECL_VARIABLE,
    EXPR_DECL_VARIABLE_ASSIGN,
};

union ExprValue {
    s64 _int;
    double _float;
    char* _name;
    Expr* _childPair[2];
    Expr* _childSingle;
};

// Expression
struct Expr {
    ExprValue value;
    ExprType type;
    u32 lineNumber = 0;
    Datatype datatype;
};

Expr createExprInteger(s64 value) {
    Expr expr;
    expr.value._int = value;
    expr.type = EXPR_INTEGER;
    expr.datatype.type = TYPE_INT;
    return expr;
}

Expr createExprFloat(double value) {
    Expr expr;
    expr.value._float = value;
    expr.type = EXPR_FLOAT;
    expr.datatype.type = TYPE_FLOAT32;
    return expr;
}

Expr createExprOperator(ExprType type, Expr* childA, Expr* childB) {
    Expr expr;
    expr.value._childPair[0] = childA;
    expr.value._childPair[1] = childB;
    expr.type = type;
    return expr;
}

Expr createExprOperator(ExprType type, Expr* child) {
    Expr expr;
    expr.value._childSingle = child;
    expr.type = type;
    return expr;
}

Expr createExprDeclDatatype(Datatype datatype) {
    Expr expr;
    expr.type = EXPR_DECL_DATATYPE;
    expr.datatype = datatype;
    return expr;
}

Expr createExprDeclVariable(Datatype datatype, char* name) {
    Expr expr;
    expr.type = EXPR_DECL_VARIABLE;
    expr.datatype = datatype;
    expr.value._name = name;
    return expr;
}

Expr createExprDeclVariableAssign(Expr exprVariableDeclaration, Expr exprValue) {
    Expr expr;
    expr.type = EXPR_DECL_VARIABLE_ASSIGN;
    expr.value._childPair[0] = new Expr(exprVariableDeclaration);
    expr.value._childPair[1] = new Expr(exprValue);
    return expr;
}

static Datatype evaluateType(Expr* expr) {
    if (expr->datatype.type != TYPE_UNEVALUATED)
        return expr->datatype;
    
    switch (expr->type) {
        case EXPR_ADD:
        case EXPR_SUBTRACT:
        case EXPR_MULTIPLY:
        case EXPR_DIVIDE:
        case EXPR_MODULO: 
        case EXPR_DECL_VARIABLE_ASSIGN: {
            Datatype a = evaluateType(expr->value._childPair[0]);
            Datatype b = evaluateType(expr->value._childPair[1]);
            
            // TODO: Implicit cast int to/from s8,s16,s32,s64. size_t,uint to/from u8,u16,u32,u64.
            
            if (a != b)
                runtimeError(expr->value._childPair[1]->lineNumber, "Type does not match");
            
            expr->datatype = a;
            break;
        }
        
        default:
            runtimeError("Unhandled expression type in evaluateType(...)");
            break;
    }
    
    if (expr->datatype.type == TYPE_UNEVALUATED)
        runtimeError(expr->lineNumber, "Unevaluated type");
    
    return expr->datatype;
}