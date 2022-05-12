#ifndef PARSE_H
#define PARSE_H

#include "token.h"
#include "ast.h"
#include "common.h"

#include "external/vec.h"


static expr_t parse_c_stmt(vec_token_t* tokens, u32* index);
static expr_t parse_c_rvalue(vec_token_t* tokens, u32* index, int max_precedence);
static inline int get_c_precedence(token_t token);


static vec_expr_t parse_c(vec_token_t* tokens) {
    vec_expr_t expressions;
	vec_init(&expressions);
    u32 index = 0;
    
    while (index < tokens->length) {
        expr_t expr = parse_c_stmt(tokens, &index);
        //evaluateType(&expr_t);
        if (expr.type != EXPR_NULL)
            vec_push(&expressions, expr);
    }
    
    return expressions;
}

static expr_t parse_c_stmt(vec_token_t* tokens, u32* index) {
    expr_t last_expr;
    last_expr.type = EXPR_NULL;
    
    for(; *index < tokens->length; ++(*index)) {
        token_t token = tokens->data[*index];
        expr_t expr;
        
        switch (token.type) {
            case TOKEN_LABEL:
                if (last_expr.type == EXPR_DECL_DATATYPE)
                    expr = create_expr_decl_variable(last_expr.datatype, token._string);
                else if (last_expr.type != EXPR_NULL)
                    runtime_error(token.line_number, "Unexpected token");
                else {
                    datatype_t datatype = create_datatype_label(token._string);
                    if (datatype.type == TYPE_UNEVALUATED)
                        PARSE_ERROR("Symbol '%s' is not a datatype", token.line_number, token._string);
                    expr = create_expr_decl_datatype(datatype);
                }
                break;
            
            case TOKEN_SYMBOL:
                switch(token._symbol) {
                    case SYMBOL_SEMICOLON:
						(*index)++;
                        return last_expr;
                    case SYMBOL_ASSIGN:
                        if (last_expr.type == EXPR_DECL_VARIABLE) {
                            (*index)++;
                            expr_t expr_value = parse_c_rvalue(tokens, index, 15);
                            (*index)--;
                            datatype_t datatype = evaluate_type(&expr_value);
                            if (!datatype_implicit_cast_cmp(datatype, last_expr.datatype))
                                runtime_error(expr_value.line_number, "datatype of expression does not match with variable");

                            expr = create_expr_decl_variable_assign(last_expr, expr_value);
                        }
                        else
                            runtime_error(token.line_number, "Unexpected token");
                        break;
                    default:
                        if (last_expr.type != EXPR_NULL)
                            runtime_error_simple("Unexpected token");
                        return parse_c_rvalue(tokens, index, 15);
                }
                break;
            default:
                if (last_expr.type != EXPR_NULL)
                    runtime_error_simple("Unexpected token");
                return parse_c_rvalue(tokens, index, 15);
        }
        
        expr.line_number = token.line_number;
        evaluate_type(&expr);
        last_expr = expr;
    }
    
    return last_expr;
}

static expr_t parse_c_rvalue(vec_token_t* tokens, u32* index, int max_precedence) {
    expr_t last_expr;
    last_expr.type = EXPR_NULL;
    
    for(; *index < tokens->length; ++*index) {
        token_t token = tokens->data[*index];
        expr_t expr;
        
        int precedence = get_c_precedence(token);
        if (precedence > max_precedence)
            return last_expr;
        
        switch(token.type) {
            case TOKEN_INT_LITERAL:
                expr = create_expr_int_literal(token._int);
                if (last_expr.type) runtime_error(token.line_number, "Unexpected token (int).");
                break;
            case TOKEN_FLOAT_LITERAL:
                expr = create_expr_float_literal(token._float);
                if (last_expr.type) runtime_error(token.line_number, "Unexpected token (float).");
                break;
                
            case TOKEN_SYMBOL: 
                switch(token._symbol) {
                    case SYMBOL_SEMICOLON:
                        if (!last_expr.type) runtime_error(token.line_number, "Unexpected token ';'");
                        return last_expr;
                    case SYMBOL_PARANTHESIS_END:
                        if (!last_expr.type) runtime_error(token.line_number, "Unexpected token ')'");
                        return last_expr;
                        
                    case SYMBOL_PARANTHESIS_BEGIN: {
                        (*index)++;
                        expr = parse_c_rvalue(tokens, index, precedence);
                        if (last_expr.type) runtime_error(token.line_number, "Unexpected token '('");
                        break;
                    }
                    case SYMBOL_ADD: {
                        (*index)++;
                        expr_t next_expr = parse_c_rvalue(tokens, index, precedence);
						expr_t* expr_a = malloc(sizeof(expr_t));
						expr_t* expr_b = malloc(sizeof(expr_t));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_ADD, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_SUBTRACT: {
                        (*index)++;
                        expr_t next_expr = parse_c_rvalue(tokens, index, precedence);
						expr_t* expr_a = malloc(sizeof(expr_t));
						expr_t* expr_b = malloc(sizeof(expr_t));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_SUBTRACT, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_MULTIPLY: {
                        (*index)++;
                        expr_t next_expr = parse_c_rvalue(tokens, index, precedence);
						expr_t* expr_a = malloc(sizeof(expr_t));
						expr_t* expr_b = malloc(sizeof(expr_t));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_MULTIPLY, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_DIVIDE: {
                        (*index)++;
                        expr_t next_expr = parse_c_rvalue(tokens, index, precedence);
						expr_t* expr_a = malloc(sizeof(expr_t));
						expr_t* expr_b = malloc(sizeof(expr_t));
						*expr_a = last_expr, *expr_b = next_expr;
                        expr = create_expr_operator_pair(EXPR_DIVIDE, expr_a, expr_b);
                        (*index)--;
                        break;
                    }
                    case SYMBOL_MODULO: {
                        (*index)++;
                        expr_t next_expr = parse_c_rvalue(tokens, index, precedence);
						expr_t* expr_a = malloc(sizeof(expr_t));
						expr_t* expr_b = malloc(sizeof(expr_t));
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

static inline int get_c_precedence(token_t token) {
    switch (token.type) {
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
