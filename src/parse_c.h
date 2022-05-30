#pragma once

#include "token.h"
#include "ast.h"
#include "common.h"

#include "external/vec.h"


static Expr parse_c_stmt(TokenArray* tokens, u32* index);
static Expr parse_c_rvalue(TokenArray* tokens, u32* index, int max_precedence);
static inline int get_c_precedence(Token token);


static ExprArray parse_c(TokenArray* tokens, u32* index) {
    ExprArray expressions = {};
    
    while (index < tokens->len) {
        if (token_is_symbol(tokens->at[*index], SYMBOL_BRACE_END)) {
            break;
        }

        Expr expr = parse_c_stmt(tokens, index);
        //evaluateType(&Expr);
        if (expr.tag != EXPR_NULL)
            array_push(expressions, expr);
    }

    return expressions;
}

static Expr parse_c_struct(TokenArray* tokens, u32* index) {

}

static Expr parse_c_stmt(TokenArray* tokens, u32* index) {
    Expr last_expr;
    last_expr.tag = EXPR_NULL;
    
    for(; *index < tokens->len; ++(*index)) {
        Token token = tokens->at[*index];
        Expr expr;
        
        switch (token.tag) {
            case TOKEN_LABEL:
                if (last_expr.tag == EXPR_DECL_DATATYPE)
                    expr = create_expr_decl_variable(last_expr.datatype, token._string);
                else if (last_expr.tag != EXPR_NULL)
                    runtime_error(token.line_number, "Unexpected token");
                else {
                    if (str_is(token._string, "stuct")) {
                        (*index)++;
                        expr = parse_c_struct(tokens, index);
                    } else {
                        Datatype datatype = create_datatype_label(token._string);
                        if (datatype.tag == TYPE_UNEVALUATED)
                            PARSE_ERROR("Symbol '%.*s' is not a datatype", token.line_number, token._string.len, token._string.at);
                        expr = create_expr_decl_datatype(datatype);
                    }
                }
                break;
            
            case TOKEN_SYMBOL:
                switch(token._symbol) {
                    case SYMBOL_SEMICOLON:
						(*index)++;
                        return last_expr;
                    case SYMBOL_ASSIGN:
                        if (last_expr.tag == EXPR_DECL_VARIABLE) {
                            (*index)++;
                            Expr expr_value = parse_c_rvalue(tokens, index, 15);
                            (*index)--;
                            Datatype datatype = evaluate_type(&expr_value);
                            if (!datatype_implicit_cast_equals(datatype, last_expr.datatype))
                                runtime_error(expr_value.line_number, "datatype of expression does not match with variable");

                            expr = create_expr_decl_variable_assign(last_expr, expr_value);
                        }
                        else
                            runtime_error(token.line_number, "Unexpected token");
                        break;
                    default:
                        if (last_expr.tag != EXPR_NULL)
                            runtime_error_simple("Unexpected token");
                        return parse_c_rvalue(tokens, index, 15);
                }
                break;
            default:
                if (last_expr.tag != EXPR_NULL)
                    runtime_error_simple("Unexpected token");
                return parse_c_rvalue(tokens, index, 15);
        }
        
        expr.line_number = token.line_number;
        evaluate_type(&expr);
        last_expr = expr;
    }
    
    return last_expr;
}

static Expr parse_c_rvalue(TokenArray* tokens, u32* index, int max_precedence) {
    Expr last_expr;
    last_expr.tag = EXPR_NULL;
    
    for(; *index < tokens->len; ++*index) {
        Token token = tokens->at[*index];
        Expr expr;
        
        int precedence = get_c_precedence(token);
        if (precedence > max_precedence)
            return last_expr;
        
        switch(token.tag) {
            case TOKEN_INT_LITERAL:
                expr = create_expr_int_literal(token._int);
                if (last_expr.tag) runtime_error(token.line_number, "Unexpected token (int).");
                break;
            case TOKEN_FLOAT_LITERAL:
                expr = create_expr_float_literal(token._float);
                if (last_expr.tag) runtime_error(token.line_number, "Unexpected token (float).");
                break;
                
            case TOKEN_SYMBOL: 
                switch(token._symbol) {
                    case SYMBOL_SEMICOLON:
                        if (!last_expr.tag) runtime_error(token.line_number, "Unexpected token ';'");
                        return last_expr;
                    case SYMBOL_PARANTHESIS_END:
                        if (!last_expr.tag) runtime_error(token.line_number, "Unexpected token ')'");
                        return last_expr;
                        
                    case SYMBOL_PARANTHESIS_BEGIN: {
                        (*index)++;
                        expr = parse_c_rvalue(tokens, index, precedence);
                        if (last_expr.tag) runtime_error(token.line_number, "Unexpected token '('");
                        break;
                    }
                    case SYMBOL_ADD: {
                        (*index)++;
                        Expr next_expr = parse_c_rvalue(tokens, index, precedence);
                        Expr* expr_a = malloc(sizeof(Expr));
                        Expr* expr_b = malloc(sizeof(Expr));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_ADD, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_SUBTRACT: {
                        (*index)++;
                        Expr next_expr = parse_c_rvalue(tokens, index, precedence);
                        Expr* expr_a = malloc(sizeof(Expr));
                        Expr* expr_b = malloc(sizeof(Expr));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_SUBTRACT, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_MULTIPLY: {
                        (*index)++;
                        Expr next_expr = parse_c_rvalue(tokens, index, precedence);
                        Expr* expr_a = malloc(sizeof(Expr));
                        Expr* expr_b = malloc(sizeof(Expr));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_MULTIPLY, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_DIVIDE: {
                        (*index)++;
                        Expr next_expr = parse_c_rvalue(tokens, index, precedence);
                        Expr* expr_a = malloc(sizeof(Expr));
                        Expr* expr_b = malloc(sizeof(Expr));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_DIVIDE, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_MODULO: {
                        (*index)++;
                        Expr next_expr = parse_c_rvalue(tokens, index, precedence);
                        Expr* expr_a = malloc(sizeof(Expr));
                        Expr* expr_b = malloc(sizeof(Expr));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_MODULO, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    default:
                        runtime_error(token.line_number, "Unexpected token: unkown symbol");
                        break;
                }
                break;
                
            default:
                runtime_error(token.line_number, "Unexpected token.");
                break;
        }
        
        expr.line_number = token.line_number;
        last_expr = expr;
    }
    
    evaluate_type(&last_expr);
    return last_expr;
}

static inline int get_c_precedence(Token token) {
    switch (token.tag) {
        default:
            return 0;
            
        case TOKEN_SYMBOL:
            switch (token._symbol) {
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
