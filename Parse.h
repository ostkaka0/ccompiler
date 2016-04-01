#pragma once

#include "Token.h"
#include "Expr.h"
#include "Common.h"

static Expr parseStmt(std::vector<Token>& tokens, u32& index);
static Expr parseRvalue(std::vector<Token>& tokens, u32& index, int maxPrecedence);
static inline int getPrecedence(Token& token);


static std::vector<Expr> parse(std::vector<Token>& tokens) {
    std::vector<Expr> expressions;
    u32 index = 0;
    
    while (index < tokens.size()) {
        Expr expr = parseStmt(tokens, index);
        //evaluateType(&expr);
        if (expr.type != EXPR_NULL)
            expressions.push_back(expr);
    }
    
    return expressions;
}

static Expr parseStmt(std::vector<Token>& tokens, u32& index) {
    Expr lastExpr;
    lastExpr.type = EXPR_NULL;
    
    for(; index < tokens.size(); ++index) {
        Token token = tokens[index];
        Expr expr;
        
        switch (token.type) {
            case TOKEN_TEXT:
                if (lastExpr.type == EXPR_DECL_DATATYPE) 
                    expr = createExprDeclVariable(lastExpr.datatype, token.value._text);
                else if (lastExpr.type != EXPR_NULL)
                    runtimeError(token.lineNumber, "Unexpected token");
                else
                    expr = createExprDeclDatatype(createDatatype(token.value._text));
                break;
            
            case TOKEN_SYMBOL:
                switch(token.value._symbol) {
                    case SYMBOL_SEMICOLON:
                        return lastExpr;
                    case SYMBOL_ASSIGN:
                        if (lastExpr.type == EXPR_DECL_VARIABLE) {
                            index++;
                            Expr exprValue = parseRvalue(tokens, index, 15);
                            index--;
                            Datatype datatype = evaluateType(&exprValue);
                            if (datatype != lastExpr.datatype)
                                runtimeError(exprValue.lineNumber, "Datatype does not match with declaration");
                            expr = createExprDeclVariableAssign(lastExpr, exprValue);
                        }
                        else
                            runtimeError(token.lineNumber, "Unexpected token");
                        break;
                    default:
                        if (lastExpr.type != EXPR_NULL)
                            runtimeError("Unexpected token");
                        return parseRvalue(tokens, index, 15);
                }
                break;
            default:
                if (lastExpr.type != EXPR_NULL)
                    runtimeError("Unexpected token");
                return parseRvalue(tokens, index, 15);
        }
        
        expr.lineNumber = token.lineNumber;
        evaluateType(&expr);
        lastExpr = expr;
    }
    
    return lastExpr;
}

static Expr parseRvalue(std::vector<Token>& tokens, u32& index, int maxPrecedence) {
    Expr lastExpr;
    lastExpr.type = EXPR_NULL;
    
    for(; index < tokens.size(); ++index) {
        Token token = tokens[index];
        Expr expr;
        
        int precedence = getPrecedence(token);
        if (precedence > maxPrecedence)
            return lastExpr;
        
        switch(token.type) {
            case TOKEN_INT:
                expr = createExprInteger(token.value._int);
                if (lastExpr.type) runtimeError(token.lineNumber, "Unexpected token.");
                break;
            case TOKEN_FLOAT:
                expr = createExprFloat(token.value._float);
                if (lastExpr.type) runtimeError(token.lineNumber, "Unexpected token.");
                break;
                
            case TOKEN_SYMBOL: 
                switch(token.value._symbol) {
                    case SYMBOL_SEMICOLON:
                        if (!lastExpr.type) runtimeError(token.lineNumber, "Unexpected token ';'");
                        return lastExpr;
                    case SYMBOL_PARANTHESIS_END:
                        if (!lastExpr.type) runtimeError(token.lineNumber, "Unexpected token ')'");
                        return lastExpr;
                        
                    case SYMBOL_PARANTHESIS_BEGIN: {
                        index++;
                        expr = parseRvalue(tokens, index, precedence);
                        if (lastExpr.type) runtimeError(token.lineNumber, "Unexpected token '('");
                        break;
                    }
                    case SYMBOL_ADD: {
                        index++;
                        Expr nextExpr = parseRvalue(tokens, index, precedence);
                        expr = createExprOperator(EXPR_ADD, new Expr(lastExpr), new Expr(nextExpr));
                        index--;
                        break;
                    }
                    case SYMBOL_SUBTRACT: {
                        index++;
                        Expr nextExpr = parseRvalue(tokens, index, precedence);
                        expr = createExprOperator(EXPR_SUBTRACT, new Expr(lastExpr), new Expr(nextExpr));
                        index--;
                        break;
                    }
                    case SYMBOL_MULTIPLY: {
                        index++;
                        Expr nextExpr = parseRvalue(tokens, index, precedence);
                        expr = createExprOperator(EXPR_MULTIPLY, new Expr(lastExpr), new Expr(nextExpr));
                        index--;
                        break;
                    }
                    case SYMBOL_DIVIDE: {
                        index++;
                        Expr nextExpr = parseRvalue(tokens, index, precedence);
                        expr = createExprOperator(EXPR_DIVIDE, new Expr(lastExpr), new Expr(nextExpr));
                        index--;
                        break;
                    }
                    case SYMBOL_MODULO: {
                        index++;
                        Expr nextExpr = parseRvalue(tokens, index, precedence);
                        expr = createExprOperator(EXPR_MODULO, new Expr(lastExpr), new Expr(nextExpr));
                        index--;
                        break;
                    }
                    default:
                        runtimeError(token.lineNumber, "Unexpected token.");
                        break;
                }
                break;
                
            default:
                runtimeError(token.lineNumber, "Unexpected token.");
                break;
        }
        
        expr.lineNumber = token.lineNumber;
        lastExpr = expr;
    }
    
    evaluateType(&lastExpr);
    return lastExpr;
}

static inline int getPrecedence(Token& token) {
    switch (token.type) {
        default:
            return 0;
            
        case TOKEN_SYMBOL:
            switch (token.value._symbol) {
                case SYMBOL_ADD:
                case SYMBOL_SUBTRACT:
                    return 4;
                    
                case SYMBOL_MULTIPLY:
                case SYMBOL_DIVIDE:
                case SYMBOL_MODULO:
                    return 3;
                
                default:
                    return 0;
            }
            break;
        
    }
}