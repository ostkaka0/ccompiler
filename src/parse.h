#ifndef PARSE_H
#define PARSE_H

#include "token.h"
#include "ast.h"
#include "common.h"


static Expr parse_stmt(TokenArray* tokens, u32* index);
static Expr parse_rvalue(TokenArray* tokens, u32* index, int max_precedence);
static inline int get_precedence(Token token);


static ExprArray parse(TokenArray* tokens) {
    ExprArray expressions = {};
    u32 index = 0;
    
    while (index < tokens->len) {
        Expr expr = parse_stmt(tokens, &index);
        //evaluateType(&Expr);
        if (expr.tag != EXPR_NULL)
            array_push(expressions, expr);
    }
    
    return expressions;
}

static Expr parse_stmt(TokenArray* tokens, u32* index) {
    Expr last_expr;
    last_expr.tag = EXPR_NULL;
    
    for(; *index < tokens->len; ++(*index)) {
        Token token = tokens->at[*index];
        Expr expr;
        
        switch (token.tag) {
            case TOKEN_LABEL:
                if (last_expr.tag == EXPR_NULL) {
                    expr = (Expr){
                        .tag = EXPR_IDENTIFIER,
                        .line_number = -1,
                        ._string = token._string,
                    };
                } else {
                    PARSE_ERROR("Unexpected symbol '%s'", token.line_number, token._string);
                }
                break;
            
            case TOKEN_SYMBOL:
                switch(token._symbol) {
                    case SYMBOL_SEMICOLON:
						(*index)++;
                        return last_expr;
                    case SYMBOL_COLON_COLON:
                        if (last_expr.tag == EXPR_IDENTIFIER) {
                            (*index)++;
                            Expr expr_value = parse_rvalue(tokens, index, 15);
                        }

                        break;
                    case SYMBOL_ASSIGN:
                        if (last_expr.tag == EXPR_DECL_VARIABLE) {
                            (*index)++;
                            Expr expr_value = parse_rvalue(tokens, index, 15);
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
                        return parse_rvalue(tokens, index, 15);
                }
                break;
            default:
                if (last_expr.tag != EXPR_NULL)
                    runtime_error_simple("Unexpected token");
                return parse_rvalue(tokens, index, 15);
        }
        
        expr.line_number = token.line_number;
        evaluate_type(&expr);
        last_expr = expr;
    }
    
    return last_expr;
}

static Expr parse_struct_decl(TokenArray* tokens, u32* index) {
    //if (!token_equals(tokens->at[*index], create_token_symbol(SYMBOL_BRACE_BEGIN))) runtime_error(token.line_number, "Unexpected token.");
    //(*index)++;

}

static Expr parse_rvalue(TokenArray* tokens, u32* index, int max_precedence) {
    Expr last_expr;
    last_expr.tag = EXPR_NULL;
    
    for(; *index < tokens->len; ++*index) {
        Token token = tokens->at[*index];
        Expr expr;
        
        int precedence = get_precedence(token);
        if (precedence > max_precedence)
            return last_expr;
        
        switch(token.tag) {
            case TOKEN_LABEL:
                if (str_is(token._string, "struct")) {
                    (*index)++;
                    parse_struct_decl(tokens, index);

                } else {
                    runtime_error(token.line_number, "Unexpected token.");
                }
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
                        expr = parse_rvalue(tokens, index, precedence);
                        if (last_expr.tag) runtime_error(token.line_number, "Unexpected token '('");
                        break;
                    }
                    case SYMBOL_ADD: {
                        (*index)++;
                        Expr next_expr = parse_rvalue(tokens, index, precedence);
                        Expr* expr_a = malloc(sizeof(Expr));
                        Expr* expr_b = malloc(sizeof(Expr));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_ADD, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_SUBTRACT: {
                        (*index)++;
                        Expr next_expr = parse_rvalue(tokens, index, precedence);
                        Expr* expr_a = malloc(sizeof(Expr));
                        Expr* expr_b = malloc(sizeof(Expr));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_SUBTRACT, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_MULTIPLY: {
                        (*index)++;
                        Expr next_expr = parse_rvalue(tokens, index, precedence);
                        Expr* expr_a = malloc(sizeof(Expr));
                        Expr* expr_b = malloc(sizeof(Expr));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_MULTIPLY, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_DIVIDE: {
                        (*index)++;
                        Expr next_expr = parse_rvalue(tokens, index, precedence);
                        Expr* expr_a = malloc(sizeof(Expr));
                        Expr* expr_b = malloc(sizeof(Expr));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_DIVIDE, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_MODULO: {
                        (*index)++;
                        Expr next_expr = parse_rvalue(tokens, index, precedence);
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


static Expr parse_const_decl(TokenArray* tokens, u32* index, char* str) {
    assert(tokens->at[*index]._symbol = SYMBOL_COLON_COLON);
    (*index)++;
    Token token = tokens->at[*index];
    //if (token.)

}

static inline int get_precedence(Token token) {
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

#endif // PARSE_H
