/**
 * @file parser.c
 * @author Simon Cagala (xcagal00@stud.fit.vutbr.cz)
 * @author Jakub Pomsar (xpomsa00@stud.fit.vutbr.cz)
 * @brief
 * @date 2023-11-09
 */
#include "parser.h"
#include "expression.h"

/* ===================| RULE DEFINITIONS |================= */

/**
 * @brief <prog> -> <stmt> <prog> |
 *                  func ID ( <param_list> <func_ret_type> { <func_body> <prog> |
 *                  EOF
 */
Rule prog(Parser* p) {
    RULE_PRINT("prog");
    uint32_t res, err;
    DEBUG_PRINT("current: %d", p->curr_tok.type);

    switch (p->curr_tok.type) {
    case TOKEN_EOF:
        break;
    case TOKEN_FUNC:
        CHECK_NEWLINE();
        p->in_func_head = true;
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
        p->last_func_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        /* Generate label for function */
        code_generator_function_label_token(p->curr_tok);
        
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_L_PAR);
        /* new scope for parameters */
        add_scope(&p->stack, &err);
        /* Reset param counter before parsing function parameters */
        p->param_cnt = 0;
        GET_TOKEN();
        NEXT_RULE(param_list_skip);

        GET_TOKEN();
        NEXT_RULE(func_ret_type_skip);
        p->in_func_head = false;
        p->in_func_body = true;
        ASSERT_TOK_TYPE(TOKEN_L_BKT);

        GET_TOKEN();
        p->first_stmt = true;
        /* new scope for body */
        add_scope(&p->stack, &err);
        NEXT_RULE(func_body);
        /* pop the parameter scope */
        pop_scope(&p->stack, &err);
        p->in_func_body = false;
        p->first_stmt = false;
        GET_TOKEN();
        NEXT_RULE(prog);
        break;
    default:
        NEXT_RULE(stmt);
        p->first_stmt = false;
        NEXT_RULE(prog);
        break;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <stmt> -> var <define> |
 *                  let <define> |
 *                  ID <expression_type> |
 *                  if <cond_clause> { <block_body> else { <block_body> |
 *                  while EXP { <block_body> |
 */
Rule stmt(Parser* p) {
    RULE_PRINT("stmt");
    uint32_t res, err;

    switch (p->curr_tok.type) {
    case TOKEN_VAR: /* var <define> */
        CHECK_NEWLINE();
        GET_TOKEN();
        NEXT_RULE(define);
        p->lhs_id->is_mutable = true;
        p->lhs_id = NULL;
        break;
    case TOKEN_LET: /* let <define> */
        CHECK_NEWLINE();
        GET_TOKEN();
        NEXT_RULE(define);
        p->lhs_id->is_mutable = false;
        p->lhs_id = NULL;
        break;
    case TOKEN_IDENTIFIER: /* ID<expression_type> */
        CHECK_NEWLINE();
        if (peek_scope(p->stack)) {
            p->current_id = search_scopes(p->stack, &p->curr_tok.value.string_val, &err);
        }
        else {
            p->current_id = NULL;
        }
        /* item wasn't found in any of the local scopes so we search global symtable */
        if (!p->current_id) {
            p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        }
        p->lhs_id = p->current_id;
        GET_TOKEN();
        NEXT_RULE(expr_type);
        p->lhs_id = NULL;
        break;
    case TOKEN_WHILE: /* while EXP { <block_body> */
        CHECK_NEWLINE();
        p->in_loop++;

        if ((res = expr(p))) {
            return res;
        }
        if (p->expr_res.expr_type != bool_) {
            fprintf(stderr, "[ERROR %d] Invalid expression in while condition\n", ERR_INCOMPATIBILE_TYPE);
            return ERR_INCOMPATIBILE_TYPE;
        }
        ASSERT_TOK_TYPE(TOKEN_L_BKT);

        GET_TOKEN();
        add_scope(&p->stack, &err);
        p->first_stmt = true;
        NEXT_RULE(block_body);
        GET_TOKEN();
        p->in_loop--;
        break;
    case TOKEN_IF: /* if <cond_clause> { <block_body> else { <block_body> */
        CHECK_NEWLINE();
        p->in_cond++;
        GET_TOKEN();
        NEXT_RULE(cond_clause);
        ASSERT_TOK_TYPE(TOKEN_L_BKT);
        /* Local scope for if body */
        add_scope(&p->stack, &err);
        GET_TOKEN();
        p->first_stmt = true;
        /* pop local (if) scope inside block body */
        NEXT_RULE(block_body);

        /* Pops local scope created inside condition clause */
        pop_scope(&p->stack, &err);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_ELSE);
        /* adding scope for else */
        add_scope(&p->stack, &err);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_L_BKT);
        GET_TOKEN();
        p->first_stmt = true;
        /* pop local (else) scope inside block body */
        NEXT_RULE(block_body);
        GET_TOKEN();
        p->in_cond--; // condition should be fully parsed by the time we're exiting the switch statement
        break;
    default:
        fprintf(stderr, "[ERROR %d] Unexpected token, var/let/while/if/identifier expected", ERR_SYNTAX);
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <define> -> ID <var_def_cont>
 */
Rule define(Parser* p) {
    RULE_PRINT("define");
    uint32_t res, err;
    DEBUG_PRINT("%s %d", p->curr_tok.value.string_val.str, p->curr_tok.type);

    ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
    if ((p->in_cond > 0) || (p->in_loop > 0) || p->in_func_body) {
        DEBUG_PRINT("in cond, loop or decl");
        if (!peek_scope(p->stack)) {
            fprintf(stderr, "[ERROR %d] Missing local scope in body\n", ERR_INTERNAL);
            return ERR_INTERNAL;
        }
        else {
            p->current_id = symtable_search(p->stack->local_sym, &p->curr_tok.value.string_val, &err);
        }

        if (p->current_id) {
            fprintf(stderr, "[ERROR %d] Variable %s already defined\n", ERR_REDEFINING_VARIABLE, p->current_id->name.str);
            return ERR_REDEFINING_VARIABLE;
        }
        /* Add symbol to local symtable */
        symtable_insert(p->stack->local_sym, &p->curr_tok.value.string_val, &err);

        /* save the added ID to parser data */
        p->current_id = symtable_search(p->stack->local_sym, &p->curr_tok.value.string_val, &err);
        DEBUG_PRINT("%s inserted to local symtab", p->current_id->name.str);
    }
    else {
        DEBUG_PRINT("global scope");
        p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        if (p->current_id) {
            fprintf(stderr, "[ERROR %d] Variable %s already defined\n", ERR_REDEFINING_VARIABLE, p->current_id->name.str);
            return ERR_REDEFINING_VARIABLE;
        }
        symtable_insert(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
    }

    /* save variable id to p.lhs_id for later */
    p->lhs_id = p->current_id;
    GET_TOKEN();
    NEXT_RULE(var_def_cont);

    return EXIT_SUCCESS;
}

/**
 * @brief <var_def_cont> -> : <type> <opt_assign> | = EXP
 */
Rule var_def_cont(Parser* p) {
    RULE_PRINT("var_def_cont");
    uint32_t res, err;

    switch (p->curr_tok.type) {
    case TOKEN_COL:
        GET_TOKEN();
        NEXT_RULE(type);
        NEXT_RULE(opt_assign);
        break;
    case TOKEN_ASS:
        /* Check if the right side of the assignment is a function call */
        GET_TOKEN();
        if (p->curr_tok.type == TOKEN_IDENTIFIER) {
            /* Look for the ID in global table, where all functions are stored */
            if ((p->rhs_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err))) {
                if (p->rhs_id->type == function) {
                    if (p->rhs_id->return_type == nil) {
                        fprintf(stderr, "[ERROR %d] Invalid assignment of void function '%s' to variable '%s' \n", ERR_INCOMPATIBILE_TYPE, p->rhs_id->name.str, p->current_id->name.str);
                        return ERR_INCOMPATIBILE_TYPE;
                    }
                    /* If the variable does not have a specified type, implicitly set it to the return type of rhs function */
                    if (p->lhs_id->type == undefined) {
                        p->lhs_id->type = p->rhs_id->return_type;
                        p->lhs_id->is_nillable = p->rhs_id->is_nillable;
                    }
                    NEXT_RULE(funccall);
                    DEBUG_PRINT("Setting %s to initialized rettype: %d", p->lhs_id->name.str, p->lhs_id->type);
                    p->lhs_id->is_var_initialized = true;
                    return EXIT_SUCCESS;
                }
                /* ID found in global symtab was not a function */
                else {
                    GET_TOKEN();
                    if (p->curr_tok.type == TOKEN_L_PAR) {
                        fprintf(stderr, "[ERROR %d] Undefined function '%s'\n", ERR_UNDEFINED_FUNCTION, p->rhs_id->name.str);
                        return ERR_UNDEFINED_FUNCTION;
                    }
                    tb_prev(&p->buffer);
                    tb_prev(&p->buffer);
                    p->curr_tok = tb_get_token(&p->buffer);
                }
            }
            /* ID not found in global symtab */
            else {
                tb_prev(&p->buffer);
                p->curr_tok = tb_get_token(&p->buffer);
            }
        }
        else {
            tb_prev(&p->buffer);
            p->curr_tok = tb_get_token(&p->buffer);
        }
        DEBUG_PRINT("before expr :: %d", p->curr_tok.type);
        if ((res = expr(p))) {
            return res;
        }

        /* Implicit setting of variable type */
        if (p->expr_res.expr_type == nil) {
            fprintf(stderr, "[ERROR %d] '%s' - cannot implicitly set type from nil expression", ERR_MISSING_TYPE, p->lhs_id->name.str);
            return ERR_MISSING_TYPE;
        }

        p->lhs_id->type = p->expr_res.expr_type;
        p->lhs_id->is_nillable = p->expr_res.nilable;

        DEBUG_PRINT("Setting %s to initialized", p->lhs_id->name.str);
        p->lhs_id->is_var_initialized = true;
        break;

    default:
        fprintf(stderr, "[ERROR %d] Unexpected token after variable identifier '%s'\n", ERR_SYNTAX, p->current_id->name.str);
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <opt_assign> -> = EXP | eps
 */
Rule opt_assign(Parser* p) {
    RULE_PRINT("opt_assign");
    uint32_t res, err;

    if (p->curr_tok.type == TOKEN_ASS) {

        /* Check if the right side of the assignment is a function call */
        GET_TOKEN();
        if (p->curr_tok.type == TOKEN_IDENTIFIER) {
            /* Look for the ID in global table, where all functions are stored */
            if ((p->rhs_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err))) {
                if (p->rhs_id->type == function) {
                    if (p->rhs_id->return_type == nil) {
                        fprintf(stderr, "[ERROR %d] Invalid assignment of void function '%s' to variable '%s' \n", ERR_INCOMPATIBILE_TYPE, p->rhs_id->name.str, p->current_id->name.str);
                        return ERR_INCOMPATIBILE_TYPE;
                    }
                    /* Check type compatibility */
                    else {
                        if (p->current_id->type != p->rhs_id->return_type) {
                            fprintf(stderr, "[ERROR %d] Incompatible types when assigning function '%s' to variable '%s'\n", ERR_INCOMPATIBILE_TYPE, p->rhs_id->name.str, p->current_id->name.str);
                            return ERR_INCOMPATIBILE_TYPE;
                        }
                        if ((!p->current_id->is_nillable) && (p->rhs_id->is_nillable)) {
                            fprintf(stderr, "[ERROR %d] Cannot assign a nillable function to variable '%s'\n", ERR_INCOMPATIBILE_TYPE, p->current_id->name.str);
                            return ERR_INCOMPATIBILE_TYPE;
                        }
                    }
                    NEXT_RULE(funccall);
                    p->lhs_id->is_var_initialized = true;
                    return EXIT_SUCCESS;
                }
                /* ID found in global symtab was not a function */
                else {
                    /* Check if the id isnt attempted to be called as a function */
                    GET_TOKEN();
                    if (p->curr_tok.type == TOKEN_L_PAR) {
                        fprintf(stderr, "[ERROR %d] Undefined function '%s'\n", ERR_UNDEFINED_FUNCTION, p->rhs_id->name.str);
                        return ERR_UNDEFINED_FUNCTION;
                    }
                    tb_prev(&p->buffer);
                    tb_prev(&p->buffer);
                    p->curr_tok = tb_get_token(&p->buffer);
                }
            }
            /* ID not found in global symtab */
            else {
                tb_prev(&p->buffer);
                p->curr_tok = tb_get_token(&p->buffer);
            }
        }
        else {
            tb_prev(&p->buffer);
            p->curr_tok = tb_get_token(&p->buffer);
        }
        DEBUG_PRINT("token before expr() : %d", p->curr_tok.type);
        if ((res = expr(p))) {
            return res;
        }
        if (p->lhs_id->type != p->expr_res.expr_type) {
            if ((p->lhs_id->is_nillable) && (p->expr_res.expr_type == nil)) {
                p->lhs_id->is_var_initialized = true;
                return EXIT_SUCCESS;
            }
            fprintf(stderr, "[ERROR %d] Incompatible types when assigninng to variable '%s'\n", ERR_INCOMPATIBILE_TYPE, p->current_id->name.str);
            return ERR_INCOMPATIBILE_TYPE;
        }
        else {
            if ((!p->lhs_id->is_nillable) && (p->expr_res.nilable)) {
                fprintf(stderr, "[ERROR %d] Invalid assign of nilable expression to '%s'\n", ERR_INCOMPATIBILE_TYPE, p->lhs_id->name.str);
                return ERR_INCOMPATIBILE_TYPE;
            }
        }

        p->lhs_id->is_var_initialized = true;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief <expr_type> -> = EXP | ( <arg_list>
 */
Rule expr_type(Parser* p) {
    RULE_PRINT("expr_type");
    uint32_t res, err;

    switch (p->curr_tok.type) {
    /* If assignment is the next step after loading the identifier, the ID was a variable */
    case TOKEN_ASS:
        if (!p->current_id) {
            fprintf(stderr, "[ERROR %d] Assignment to undefined variable\n", ERR_UNDEFINED_VARIABLE);
            return ERR_UNDEFINED_VARIABLE;
        }
        if ((!p->lhs_id->is_mutable) && (p->lhs_id->is_var_initialized)) {
            fprintf(stderr, "[ERROR %d] Assigning a new value to immutable variable %s\n", ERR_SEMANTIC, p->current_id->name.str);
            return ERR_SEMANTIC;
        }
        /* Check if the right side of the assignment is a function call */
        GET_TOKEN();
        if (p->curr_tok.type == TOKEN_IDENTIFIER) {
            /* Look for the ID in global table, where all functions are stored */
            if ((p->rhs_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err))) {
                if (p->rhs_id->type == function) {
                    if (p->rhs_id->return_type == nil) {
                        fprintf(stderr, "[ERROR %d] Invalid assignment of void function '%s' to variable '%s' \n", ERR_INCOMPATIBILE_TYPE, p->rhs_id->name.str, p->current_id->name.str);
                        return ERR_INCOMPATIBILE_TYPE;
                    }
                    /* Check type compatibility */
                    else {
                        if (p->current_id->type != p->rhs_id->return_type) {
                            fprintf(stderr, "[ERROR %d] Incompatible types when assigning function '%s' to variable '%s'\n", ERR_INCOMPATIBILE_TYPE, p->rhs_id->name.str, p->current_id->name.str);
                            return ERR_INCOMPATIBILE_TYPE;
                        }
                        if ((!p->current_id->is_nillable) && (p->rhs_id->is_nillable)) {
                            fprintf(stderr, "[ERROR %d] Cannot assign a nillable function to variable '%s'\n", ERR_INCOMPATIBILE_TYPE, p->current_id->name.str);
                            return ERR_INCOMPATIBILE_TYPE;
                        }
                    }
                    NEXT_RULE(funccall);
                    p->lhs_id->is_var_initialized = true;
                    return EXIT_SUCCESS;
                }
                /* ID found in global symtab was not a function */
                else {
                    /* Check if the id isnt attempted to be called as a function */
                    GET_TOKEN();
                    if (p->curr_tok.type == TOKEN_L_PAR) {
                        fprintf(stderr, "[ERROR %d] Undefined function '%s'\n", ERR_UNDEFINED_FUNCTION, p->rhs_id->name.str);
                        return ERR_UNDEFINED_FUNCTION;
                    }
                    tb_prev(&p->buffer);
                    tb_prev(&p->buffer);
                    p->curr_tok = tb_get_token(&p->buffer);
                }
            }
            /* ID not found in global symtab */
            else {
                tb_prev(&p->buffer);
                p->curr_tok = tb_get_token(&p->buffer);
            }
        }
        /* Loaded token was not an identifier, roll back and process it as an expression */
        else {
            tb_prev(&p->buffer);
            p->curr_tok = tb_get_token(&p->buffer);
        }
        /* Expression processing */
        if ((res = expr(p))) {
            return res;
        }
        if (p->lhs_id->type != p->expr_res.expr_type) {
            if ((p->lhs_id->is_nillable) && (p->expr_res.expr_type == nil)) {
                p->lhs_id->is_var_initialized = true;
                return EXIT_SUCCESS;
            }
            fprintf(stderr, "[ERROR %d] Incompatible types when assigninng to variable '%s'\n", ERR_INCOMPATIBILE_TYPE, p->current_id->name.str);
            return ERR_INCOMPATIBILE_TYPE;
        }
        else {
            if ((!p->lhs_id->is_nillable) && (p->expr_res.nilable)) {
                fprintf(stderr, "[ERROR %d] Invalid assign of nilable expression to '%s'\n", ERR_INCOMPATIBILE_TYPE, p->lhs_id->name.str);
                return ERR_INCOMPATIBILE_TYPE;
            }
        }
        p->lhs_id->is_var_initialized = true;
        break;
    /* If the loaded ID is followed by opening parentheses the ID should have been a function */
    case TOKEN_L_PAR:
        if (!p->current_id) {
            fprintf(stderr, "[ERROR %d] Attempting to call an undefined function\n", ERR_UNDEFINED_FUNCTION);
            return ERR_UNDEFINED_FUNCTION;
        }
        if (p->current_id->type != function) {
            fprintf(stderr, "[ERROR %d] Identifier '%s' is not a function", ERR_UNDEFINED_FUNCTION, p->current_id->name.str);
            return ERR_UNDEFINED_FUNCTION;
        }
        /* If the ID we're processing is a function, set it to last_func_id and reset current_id */
        p->last_func_id = p->current_id;
        p->current_id = NULL;
        p->in_function = true;

        GET_TOKEN();
        NEXT_RULE(arg_list);
        break;
    default:
        if (!p->current_id) {
            fprintf(stderr, "[ERROR %d] undefined variable\n", ERR_UNDEFINED_VARIABLE);
            return ERR_UNDEFINED_VARIABLE;
        }
        fprintf(stderr, "[ERROR %d] Unexpected token after identifier '%s'", ERR_SYNTAX, p->current_id->name.str);
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <cond_clause> -> EXP | let ID
 */
Rule cond_clause(Parser* p) {
    RULE_PRINT("cond_clause");
    uint32_t res, err;

    /* if let id */
    if (p->curr_tok.type == TOKEN_LET) {
        add_scope(&p->stack, &err);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
        DEBUG_PRINT("if let %s ", p->curr_tok.value.string_val.str);
        if (peek_scope(p->stack)) {
            DEBUG_PRINT("searching local scopes");
            p->current_id = search_scopes(p->stack, &p->curr_tok.value.string_val, &err);
        }
        else {
            p->current_id = NULL;
        }
        /* item wasn't found in any of the local scopes so we search global symtable */
        if (!p->current_id) {
            DEBUG_PRINT("searching global scope");
            p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        }
        if (!p->current_id) {
            fprintf(stderr, "[ERROR %d] Constant %s undefined\n", ERR_UNDEFINED_VARIABLE, p->curr_tok.value.string_val.str);
            return ERR_UNDEFINED_VARIABLE;
        }
        if (p->current_id->is_mutable) {
            fprintf(stderr, "[ERROR %d] Using mutable variable %s in conditional\n", ERR_SEMANTIC, p->current_id->name.str);
            return ERR_SEMANTIC;
        }
        if (!p->current_id->is_nillable) {
            fprintf(stderr, "[ERROR %d] Using a non-nilable constant '%s' in condition statement\n", ERR_SEMANTIC, p->current_id->name.str);
            return ERR_SEMANTIC;
        }

        /* And insert the symbol into the newly created local scope */
        symtable_insert(p->stack->local_sym, &p->current_id->name, &err);
        set_nillable(p->stack->local_sym, &p->current_id->name, false, &err);
        set_type(p->stack->local_sym, &p->current_id->name, p->current_id->type, &err);
        set_mutability(p->stack->local_sym, &p->current_id->name, false, &err);
        symtable_search(p->stack->local_sym, &p->current_id->name, &err)->is_var_initialized = true;
        DEBUG_PRINT("%s inserted into local if scope", p->current_id->name.str);

        GET_TOKEN();
    }
    /* if (EXPR) */
    else {
        tb_prev(&p->buffer);
        p->curr_tok = tb_get_token(&p->buffer);
        if ((res = expr(p))) {
            return res;
        }
        if (p->expr_res.expr_type != bool_) {
            fprintf(stderr, "[ERROR %d] Invalid expression type in condition\n", ERR_INCOMPATIBILE_TYPE);
            return ERR_INCOMPATIBILE_TYPE;
        }
    }

    return EXIT_SUCCESS;
}

/**
 * @brief <arg_list> -> <arg> <arg_next> | )
 */
Rule arg_list(Parser* p) {
    RULE_PRINT("arg_list");
    uint32_t res;
    p->current_arg = p->last_func_id->parameters;
    if (p->curr_tok.type == TOKEN_R_PAR) {
        /* Only check argument count if func has a set number of parameters */
        if (!p->last_func_id->variadic_param) {
            /* Function had parameters declared but no arguments were passed while calling */
            if (p->current_arg != NULL) {
                fprintf(stderr, "[ERROR %d] Missing arguments in function '%s' \n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
                return ERR_FUNCTION_PARAMETER;
            }
        }
        GET_TOKEN();
        return EXIT_SUCCESS;
    }
    /* Only check argument count if func has a set number of parameters */
    if (!p->last_func_id->variadic_param) {
        if (p->current_arg == NULL) {
            fprintf(stderr, "[ERROR %d] Too many arguments in function '%s'\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
            return ERR_FUNCTION_PARAMETER;
        }
    }
    NEXT_RULE(arg);
    GET_TOKEN();
    NEXT_RULE(arg_next);
    return EXIT_SUCCESS;
}

/**
 * @brief <arg_next> -> , <arg> <arg_next> | )
 */
Rule arg_next(Parser* p) {
    RULE_PRINT("arg_next");
    uint32_t res;

    switch (p->curr_tok.type) {
    case TOKEN_COMMA:
        if (!p->last_func_id->variadic_param) {
            if (p->current_arg->next == NULL) {
                fprintf(stderr, "[ERROR %d] Too many arguments in function %s\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
                return ERR_FUNCTION_PARAMETER;
            }
            p->current_arg = p->current_arg->next;
        }
        GET_TOKEN();
        NEXT_RULE(arg);
        GET_TOKEN();
        NEXT_RULE(arg_next);
        break;
    case TOKEN_R_PAR:
        if (!p->last_func_id->variadic_param) {
            if (p->current_arg->next != NULL) {
                fprintf(stderr, "[ERROR %d] Missing arguments in function %s \n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
                return ERR_FUNCTION_PARAMETER;
            }
        }
        GET_TOKEN();
        return EXIT_SUCCESS;
    default:
        fprintf(stderr, "[ERROR %d] Unexpected token in parameters\n", ERR_SYNTAX);
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <arg> -> "ID" <optarg> | <literal>
 */
Rule arg(Parser* p) {
    RULE_PRINT("arg");
    uint32_t res, err;
    if (p->curr_tok.type == TOKEN_IDENTIFIER) {
        /* If the current parameter's label is set to _ the loaded identifier must be a variable passed as an argument */
        if (!dstring_cmp_const_str(&p->current_arg->label, "_")) {
            /* In case of an expression like var x = foo(x) the x argument should be searched in previous scope, not the current one */
            if (p->current_id) {
                if ((!p->current_id->is_var_initialized) && (!dstring_cmp(&p->curr_tok.value.string_val, &p->current_id->name))) {
                    if (peek_scope(p->stack->next)) {
                        p->current_id = search_scopes(p->stack->next, &p->curr_tok.value.string_val, &err);
                    }
                    else {
                        p->current_id = NULL;
                    }
                }
                else {
                    if (peek_scope(p->stack)) {
                        p->current_id = search_scopes(p->stack, &p->curr_tok.value.string_val, &err);
                    }
                    else {
                        p->current_id = NULL;
                    }
                }
            }
            else {
                if (peek_scope(p->stack)) {
                    p->current_id = search_scopes(p->stack, &p->curr_tok.value.string_val, &err);
                }
                else {
                    p->current_id = NULL;
                }
            }
            if (!p->current_id) {
                p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
            }
            if (!p->current_id) {
                fprintf(stderr, "[ERROR %d] Unknown identifier %s\n", ERR_UNDEFINED_VARIABLE, p->curr_tok.value.string_val.str);
                return ERR_UNDEFINED_VARIABLE;
            }
            if (!p->current_id->is_var_initialized) {
                fprintf(stderr, "[ERROR %d] Use of uninitialized variable '%s'\n", ERR_UNDEFINED_VARIABLE, p->current_id->name.str);
                return ERR_UNDEFINED_VARIABLE;
            }
            if (!p->last_func_id->variadic_param) {
            /* Check if the variable is the same type as function parameter */
                if (p->current_id->type != p->current_arg->type) {
                    fprintf(stderr, "[ERROR %d] Function '%s': argument '%s' - invalid type\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str, p->current_id->name.str);
                    return ERR_FUNCTION_PARAMETER;
                }
            }
            return EXIT_SUCCESS;
        }
        /* Assert validity of the label */
        if (dstring_cmp(&p->current_arg->label, &p->curr_tok.value.string_val)) {
            fprintf(stderr, "[ERROR %d] Invalid label '%s' in function '%s'\n", ERR_FUNCTION_PARAMETER, p->curr_tok.value.string_val.str, p->last_func_id->name.str);
            return ERR_FUNCTION_PARAMETER;
        }

        GET_TOKEN();
        NEXT_RULE(opt_arg);
    }
    else {
        if (p->current_arg) {
            /* Throw error if current func arg had a specified label (not _) but none was passed */
            if (dstring_cmp_const_str(&p->current_arg->label, "_")) {
                fprintf(stderr, "[ERROR %d] Missing label '%s' in when calling function '%s'\n", ERR_FUNCTION_PARAMETER, p->current_arg->label.str, p->last_func_id->name.str);
                return ERR_FUNCTION_PARAMETER;
            }
        }
        NEXT_RULE(literal);
        DEBUG_PRINT("after literal::%d", p->curr_tok.type);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <param_list> -> <param> <param_next> | )
 */
Rule param_list(Parser* p) {
    RULE_PRINT("param_list");
    uint32_t res;
    p->in_param = true;
    if (p->curr_tok.type == TOKEN_R_PAR) {
        p->in_param = false;
        return EXIT_SUCCESS;
    }
    NEXT_RULE(param);
    NEXT_RULE(param_next);
    return EXIT_SUCCESS;
}

/**
 * @brief <param_next> -> , <param> <param_next> | )
 */
Rule param_next(Parser* p) {
    RULE_PRINT("param_next");
    uint32_t res;

    switch (p->curr_tok.type) {
    case TOKEN_R_PAR:
        p->in_param = false;
        break;
    case TOKEN_COMMA:
        GET_TOKEN();
        NEXT_RULE(param);
        NEXT_RULE(param_next);
        break;
    default:
        fprintf(stderr, "[ERROR %d] Unexpected token in parameters\n", ERR_SYNTAX);
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <param> ->  "_" "ID" ":" <type> | "ID" "ID" ":" <type>
 */
Rule param(Parser* p) {
    RULE_PRINT("param");
    uint32_t res, err;

    switch (p->curr_tok.type) {
    case TOKEN_UND_SCR:
        dstring_clear(&p->tmp);
        dstring_add_const_str(&p->tmp, "_");
        break;
    case TOKEN_IDENTIFIER:
        /* Store the label into Parser.tmp so we can add it after adding the whole parameter to symtable */
        dstring_clear(&p->tmp);
        dstring_copy(&p->curr_tok.value.string_val, &p->tmp);
        break;
    default:
        fprintf(stderr, "[ERROR %d] Parameter label expected\n", ERR_SYNTAX);
        return ERR_SYNTAX;
    }

    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
    NEW_PARAM();

    set_param_label(&p->global_symtab, &p->last_func_id->name, &p->curr_tok.value.string_val, &p->tmp, &err);
    DEBUG_PRINT("Param label set");
    dstring_clear(&p->tmp);
    dstring_copy(&p->curr_tok.value.string_val, &p->tmp);

    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_COL);

    GET_TOKEN();
    NEXT_RULE(type);

    return EXIT_SUCCESS;
}

/**
 * @brief <blk_body> -> <stmt> <blk_body> | }
 */
Rule block_body(Parser* p) {
    RULE_PRINT("block_body");
    uint32_t res, err;

    if (p->curr_tok.type == TOKEN_R_BKT) {
        DEBUG_PRINT("block_body end }");
        pop_scope(&p->stack, &err);
        return EXIT_SUCCESS;
    }
    else {
        NEXT_RULE(stmt);
        p->first_stmt = false;
        NEXT_RULE(block_body);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <func_body> -> <func_stmt> <func_body> | }
 */
Rule func_body(Parser* p) {
    RULE_PRINT("func_body");
    uint32_t res, err;
    DEBUG_PRINT("token::%d", p->curr_tok.type);
    if (p->curr_tok.type == TOKEN_R_BKT) {
        if (p->in_cond == 0 && p->in_loop == 0) {
            if (p->last_func_id->return_type != nil) {
                if (p->return_found == false) {
                    fprintf(stderr, "[ERROR %d] Missing return statement in function '%s'\n", ERR_RETURN_TYPE, p->last_func_id->name.str);
                    return ERR_RETURN_TYPE;
                }
            }
        }
        p->return_found = false;
        pop_scope(&p->stack, &err);
        DEBUG_PRINT("Closing body");
        return EXIT_SUCCESS;
    }
    else {
        NEXT_RULE(func_stmt);
        p->first_stmt = false;
        NEXT_RULE(func_body);
    }

    return EXIT_SUCCESS;
}

/**
 * @brief <func_stmt> -> var <def> |
 *                       let <def> |
 *                       ID <expression-type> |
 *                       while EXP { <func_body> |
 *                       if <cond_clause> { <func_body> else { <func_body> |
 *                       return <opt_ret>
 */
Rule func_stmt(Parser* p) {
    RULE_PRINT("func_stmt");
    uint32_t res, err;

    switch (p->curr_tok.type) {
    case TOKEN_VAR:
        CHECK_NEWLINE();
        GET_TOKEN();
        NEXT_RULE(define);
        p->lhs_id->is_mutable = true;
        p->lhs_id = NULL;
        break;
    case TOKEN_LET:
        CHECK_NEWLINE();
        GET_TOKEN();
        NEXT_RULE(define);
        p->lhs_id->is_mutable = false;
        p->lhs_id = NULL;
        break;
    case TOKEN_IDENTIFIER:
        CHECK_NEWLINE();
        if (peek_scope(p->stack)) {
            p->current_id = search_scopes(p->stack, &p->curr_tok.value.string_val, &err);
        }
        else {
            p->current_id = NULL;
        }
        /* item wasn't found in any of the local scopes so we search global symtable */
        if (!p->current_id) {
            p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        }
        p->lhs_id = p->current_id;
        GET_TOKEN();
        NEXT_RULE(expr_type);
        p->lhs_id = NULL;
        break;
    case TOKEN_WHILE:
        CHECK_NEWLINE();
        p->in_loop++;
        if ((res = expr(p))) {
            return res;
        }
        ASSERT_TOK_TYPE(TOKEN_L_BKT);
        GET_TOKEN();
        p->first_stmt = true;
        add_scope(&p->stack, &err);
        NEXT_RULE(func_body);
        GET_TOKEN();
        p->in_loop--;
        break;
    case TOKEN_IF:
        CHECK_NEWLINE();
        p->in_cond++;
        /* scope for if body */
        add_scope(&p->stack, &err);
        GET_TOKEN();
        NEXT_RULE(cond_clause);

        ASSERT_TOK_TYPE(TOKEN_L_BKT);

        GET_TOKEN();
        p->first_stmt = true;
        NEXT_RULE(func_body);

        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_ELSE);
        /* else body scope */
        add_scope(&p->stack, &err);

        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_L_BKT);

        GET_TOKEN();
        p->first_stmt = true;
        NEXT_RULE(func_body);
        GET_TOKEN();
        p->in_cond--;
        break;
    case TOKEN_RETURN:
        CHECK_NEWLINE();
        p->return_found = true;
        NEXT_RULE(opt_ret);
        break;
    default:
        fprintf(stderr, "[ERROR %d] Unexpected token %d\n", ERR_SYNTAX, p->curr_tok.type);
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <func_ret_type> =>  eps | -> <type>
 */
Rule func_ret_type(Parser* p) {
    RULE_PRINT("func_ret_type");
    uint32_t res, err;

    if (p->curr_tok.type == TOKEN_RET_VAL) {
        GET_TOKEN();
        NEXT_RULE(type);
    }
    else {
        set_return_type(&p->global_symtab, &p->last_func_id->name, nil, &err);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <opt_ret> -> EXP | eps
 */
Rule opt_ret(Parser* p) {
    RULE_PRINT("opt_ret");
    uint32_t res;

    if (p->last_func_id->return_type == nil) {
        GET_TOKEN();
        if (!p->curr_tok.preceding_eol) {
            if (p->curr_tok.type != TOKEN_R_BKT) {
                fprintf(stderr, "[ERROR %d] Unexpected expression in return from void function\n", ERR_FUNCTION_RETURN);
                return ERR_FUNCTION_RETURN;
            }
        }
    }
    else {
        if (p->buffer.runner->next->token.type == TOKEN_R_BKT) {
            fprintf(stderr, "[ERROR %d] Missing expression in return statement\n", ERR_FUNCTION_RETURN);
            return ERR_FUNCTION_RETURN;
        }
        if ((res = expr(p))) {
            return res;
        }
        if (p->last_func_id->return_type != p->expr_res.expr_type) {
            fprintf(stderr, "[ERROR %d] Function %s: Invalid return expression type\n", ERR_RETURN_TYPE, p->last_func_id->name.str);
            return ERR_RETURN_TYPE;
        }
    }
    code_generator_return();
    return EXIT_SUCCESS;
}

/**
 * @brief <opt_type> ->  : <type> | eps
 */
Rule opt_type(Parser* p) {
    RULE_PRINT("opt_type");
    uint32_t res;

    if (p->curr_tok.type == TOKEN_COL) {
        GET_TOKEN();
        NEXT_RULE(type);
    }
    /* Token was not a colon -> eps so return without loadnig any more tokens */
    return EXIT_SUCCESS;
}

/**
 * @brief <type> -> Int | String | Double
 */
Rule type(Parser* p) {
    RULE_PRINT("type");
    uint32_t err;

    switch (p->curr_tok.type) {
    case TOKEN_DT_INT:
        if (p->in_func_head) {
            if (p->in_param) {
                DEBUG_PRINT("Set Param Label");
                set_param_type(&p->global_symtab, &p->last_func_id->name, &p->tmp, integer, &err);
                set_param_nil(&p->global_symtab, &p->last_func_id->name, &p->tmp, p->curr_tok.value.is_nilable, &err);
            }
            else {
                DEBUG_PRINT("Set function return type Int");
                set_return_type(&p->global_symtab, &p->last_func_id->name, integer, &err);
                set_nillable(&p->global_symtab, &p->last_func_id->name, p->curr_tok.value.is_nilable, &err);
            }
        }
        else {
            DEBUG_PRINT("Set var type");
            p->current_id->type = integer;
            p->current_id->is_nillable = p->curr_tok.value.is_nilable;
            /* Implicitly set variable to initalized (to nil) if nillable */
            if (p->current_id->is_nillable)
                p->current_id->is_var_initialized = true;
        }
        GET_TOKEN();
        break;

    case TOKEN_DT_DOUBLE:
        if (p->in_func_head) {
            if (p->in_param) {
                DEBUG_PRINT("Set Param Label");
                set_param_type(&p->global_symtab, &p->last_func_id->name, &p->tmp, double_, &err);
                set_param_nil(&p->global_symtab, &p->last_func_id->name, &p->tmp, p->curr_tok.value.is_nilable, &err);
            }
            else {
                DEBUG_PRINT("Set function return type Double");
                set_return_type(&p->global_symtab, &p->last_func_id->name, double_, &err);
                set_nillable(&p->global_symtab, &p->last_func_id->name, p->curr_tok.value.is_nilable, &err);
            }
        }
        else {
            p->current_id->type = double_;
            p->current_id->is_nillable = p->curr_tok.value.is_nilable;
            /* Implicitly set variable to initalized (to nil) if nillable */
            if (p->current_id->is_nillable)
                p->current_id->is_var_initialized = true;
        }
        GET_TOKEN();
        break;

    case TOKEN_DT_STRING:
        if (p->in_func_head) {
            if (p->in_param) {
                DEBUG_PRINT("Set Param Label");
                set_param_type(&p->global_symtab, &p->last_func_id->name, &p->tmp, string, &err);
                set_param_nil(&p->global_symtab, &p->last_func_id->name, &p->tmp, p->curr_tok.value.is_nilable, &err);
            }
            else if (p->current_id) {
                DEBUG_PRINT("Set variable '%s' type String", p->current_id->name.str);
                p->current_id->type = string;
                p->current_id->is_nillable = p->curr_tok.value.is_nilable;
            }
            else {
                DEBUG_PRINT("Set function return type String");
                set_return_type(&p->global_symtab, &p->last_func_id->name, string, &err);
                set_nillable(&p->global_symtab, &p->last_func_id->name, p->curr_tok.value.is_nilable, &err);
            }
        }
        else {
            p->current_id->type = string;
            p->current_id->is_nillable = p->curr_tok.value.is_nilable;
            /* Implicitly set variable to initalized (to nil) if nillable */
            if (p->current_id->is_nillable)
                p->current_id->is_var_initialized = true;
        }
        GET_TOKEN();
        break;
    default:
        fprintf(stderr, "[ERROR %d] Type specifier expected\n", ERR_SYNTAX);
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <opt_arg> -> : <term> | eps
 */
Rule opt_arg(Parser* p) {
    RULE_PRINT("opt_arg");
    uint32_t res;

    if (p->curr_tok.type == TOKEN_COL) {
        GET_TOKEN();
        NEXT_RULE(term);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <term> -> ID | literal
 */
Rule term(Parser* p) {
    RULE_PRINT("term");
    uint32_t res, err;

    if (p->curr_tok.type == TOKEN_IDENTIFIER) {
        if (p->current_id) {
            if ((!p->current_id->is_var_initialized) && (!dstring_cmp(&p->curr_tok.value.string_val, &p->current_id->name))) {
                if (peek_scope(p->stack->next)) {
                    p->current_id = search_scopes(p->stack->next, &p->curr_tok.value.string_val, &err);
                }
                else {
                    p->current_id = NULL;
                }
            }
            else {
                if (peek_scope(p->stack)) {
                    p->current_id = search_scopes(p->stack, &p->curr_tok.value.string_val, &err);
                }
                else {
                    p->current_id = NULL;
                }
            }
        }
        else {
            if (peek_scope(p->stack)) {
                p->current_id = search_scopes(p->stack, &p->curr_tok.value.string_val, &err);
            }
            else {
                p->current_id = NULL;
            }
        }
        if (!p->current_id) {
            p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        }
        if (!p->current_id) {
            fprintf(stderr, "[ERROR %d] Unknown identifier %s\n", ERR_UNDEFINED_VARIABLE, p->curr_tok.value.string_val.str);
            return ERR_UNDEFINED_VARIABLE;
        }
        DEBUG_PRINT("Id found : %s", p->current_id->name.str);
        if (p->current_id->type != p->current_arg->type) {
            fprintf(stderr, "[ERROR %d] Invalid type of identifier %s in function %s\n", ERR_FUNCTION_PARAMETER, p->current_id->name.str, p->last_func_id->name.str);
            return ERR_FUNCTION_PARAMETER;
        }
    }
    else {
        NEXT_RULE(literal);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <literal> -> INT_LIT | STR_LIT | DBL_LIT
 */
Rule literal(Parser* p) {
    RULE_PRINT("literal");
    DEBUG_PRINT("Before switch::%d", p->curr_tok.type);
    switch (p->curr_tok.type) {
    case TOKEN_INT:
        if (!p->last_func_id->variadic_param) {
            if (p->current_arg->type != integer) {
                fprintf(stderr, "[ERROR %d] Invalid argument type(int) in fuction %s\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
                return ERR_FUNCTION_PARAMETER;
            }
        }
        /* generate term value */
        break;
    case TOKEN_DBL:
        if (!p->last_func_id->variadic_param) {
            if (p->current_arg->type != double_) {
                fprintf(stderr, "[ERROR %d] Invalid argument type(double) in fuction %s\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
                return ERR_FUNCTION_PARAMETER;
            }
        }
        break;
    case TOKEN_STRING:
        if (!p->last_func_id->variadic_param) {
            if (p->current_arg->type != string) {
                fprintf(stderr, "[ERROR %d] Invalid argument type(string) in fuction %s\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
                return ERR_FUNCTION_PARAMETER;
            }
        }
        break;
    default:
        fprintf(stderr, "[ERROR 2] Invalid literal\n");
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Rule to fill out global symtab with function declarations
 *        during the first run through the token buffer
 */
Rule func_header(Parser* p) {
    RULE_PRINT("func_header");
    uint32_t res, err;

    p->in_func_head = true;
    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);

    symtable_insert(&p->global_symtab, &p->curr_tok.value.string_val, &err);
    if (err == SYMTAB_ERR_ITEM_ALREADY_STORED) {
        fprintf(stderr, "[ERROR %d] Redeclaration of function %s\n", ERR_REDEFINING_VARIABLE, p->curr_tok.value.string_val.str);
        return ERR_REDEFINING_VARIABLE;
    }
    p->last_func_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
    p->last_func_id->type = function;

    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_L_PAR);

    GET_TOKEN();
    NEXT_RULE(param_list);

    GET_TOKEN();
    NEXT_RULE(func_ret_type);
    p->in_func_head = false;
    return EXIT_SUCCESS;
}

/**
 * @brief Substitute for type Rule during second run through token buffer
 */
Rule type_skip(Parser* p) {
    RULE_PRINT("type_skip");
    uint32_t  err;

    switch (p->curr_tok.type) {
    case TOKEN_DT_INT:
        if (p->in_param)
            set_type(p->stack->local_sym, &p->current_id->name, integer, &err);
        GET_TOKEN();
        break;
    case TOKEN_DT_DOUBLE:
        if (p->in_param)
            set_type(p->stack->local_sym, &p->current_id->name, double_, &err);
        GET_TOKEN();
        break;
    case TOKEN_DT_STRING:
        if (p->in_param)
            set_type(p->stack->local_sym, &p->current_id->name, string, &err);
        GET_TOKEN();
        break;
    default:
        fprintf(stderr, "[ERROR %d] Type specifier expected\n", ERR_SYNTAX);
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Substitute for param Rule for second run through token buffer
 */
Rule param_skip(Parser* p) {
    RULE_PRINT("param_skip");
    uint32_t res, err;

    if (p->curr_tok.type != TOKEN_UND_SCR && p->curr_tok.type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "[ERROR %d] Parameter label expected\n", ERR_SYNTAX);
        return ERR_SYNTAX;
    }
    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
    symtable_insert(p->stack->local_sym, &p->curr_tok.value.string_val, &err);
    p->current_id = symtable_search(p->stack->local_sym, &p->curr_tok.value.string_val, &err);
    p->current_id->is_var_initialized = true;
    /* Generate func parameter and increase counter before the next one */
    code_generator_param_map(p->current_id->name.str, p->param_cnt);
    p->param_cnt++;
    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_COL);

    GET_TOKEN();
    NEXT_RULE(type_skip);
    return EXIT_SUCCESS;
}

/**
 * @brief Substitute for param_next Rule for second run through token buffer
 */
Rule param_next_skip(Parser* p) {
    RULE_PRINT("param_next_skip");
    uint32_t res;

    switch (p->curr_tok.type) {
    case TOKEN_R_PAR:
        p->in_param = false;
        break;
    case TOKEN_COMMA:
        GET_TOKEN();
        NEXT_RULE(param_skip);
        NEXT_RULE(param_next_skip);
        break;
    default:
        fprintf(stderr, "[ERROR %d] Unexpected token in parameters\n", ERR_SYNTAX);
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Substitute for param_list Rule for second run through token buffer
 */
Rule param_list_skip(Parser* p) {
    RULE_PRINT("param_list_skip");
    uint32_t res;
    p->in_param = true;
    if (p->curr_tok.type == TOKEN_R_PAR) {
        p->in_param = false;
        return EXIT_SUCCESS;
    }
    NEXT_RULE(param_skip);
    NEXT_RULE(param_next_skip);
    return EXIT_SUCCESS;
}

/**
 * @brief Substitute for func_ret_type Rule for second run through token buffer
 */
Rule func_ret_type_skip(Parser* p) {
    RULE_PRINT("func_ret_type_skip");
    uint32_t res;

    if (p->curr_tok.type == TOKEN_RET_VAL) {
        GET_TOKEN();
        NEXT_RULE(type_skip);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Skips all tokens except for func and EOF in first run through the token buffer
 */
Rule skip(Parser* p) {
    uint32_t res;
    if (p->curr_tok.type == TOKEN_EOF) {
        DEBUG_PRINT("leaving with EOF");
        return EXIT_SUCCESS;
    }
    if (p->curr_tok.type == TOKEN_FUNC) {
        NEXT_RULE(func_header);
        GET_TOKEN();
        NEXT_RULE(skip);
        return EXIT_SUCCESS;
    }
    DEBUG_PRINT("skipping token: %d", p->curr_tok.type);
    GET_TOKEN();
    NEXT_RULE(skip);

    return EXIT_SUCCESS;
}

/*
    The purpose of the above skip rules is to be able to parse only function headers during the first run through tok buf
    without the risk of 'redefine' error during the second run
*/

/**
 * @brief Rule to parse function calls on the rhs of assignments
 */
Rule funccall(Parser* p) {
    RULE_PRINT("funccall");
    uint32_t res;
    symtab_item_t* temp;

    /* Current token must be an identifier stored in p->rhs_id (already parsed in the rule calling this function) */
    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_L_PAR);

    temp = p->last_func_id;
    p->last_func_id = p->rhs_id;
    GET_TOKEN();
    NEXT_RULE(arg_list);
    p->last_func_id = temp;
    return EXIT_SUCCESS;
}

/* ======================================================== */

/**
 * @brief Initializes data needed by the parser
 *
 * @param p - Parser object
 * @return true on success
 */
bool parser_init(Parser* p) {
    uint32_t symtab_err;

    /* Global symbol table initialization */
    symtable_init(&p->global_symtab, &symtab_err);
    if (symtab_err)
        return false;

    init_scope(&p->stack);
    dstring_init(&p->tmp);
    tb_init(&p->buffer);

    p->current_arg = NULL;
    p->current_id = NULL;
    p->last_func_id = NULL;
    p->lhs_id = NULL;
    p->rhs_id = NULL;
    p->in_cond = 0;
    p->in_func_head = false;
    p->in_func_body = false;
    p->in_function = false;
    p->return_found = false;
    p->first_stmt = true;
    p->in_loop = 0;
    p->in_param = false;
    p->expr_res.expr_type = undefined;
    p->expr_res.nilable = false;
    p->param_cnt = 0;
    return true;
}

/**
 * @brief Frees all data allocated by the parser
 * @param p
 */
void parser_dispose(Parser* p) {
    uint32_t err;
    dstring_free(&p->tmp);
    symtable_dispose(&p->global_symtab);
    dispose_scope(&p->stack, &err);
    tb_dispose(&p->buffer);
}

/**
 * @brief Fill global symtable with builtin functions
 *
 * @param p Parser object
 * @return true on success, otherwise false
 */
bool add_builtins(Parser* p) {
    uint32_t st_err;
    dstring_t builtin_id;
    dstring_t param_name;
    dstring_t label_name;
    dstring_init(&builtin_id);
    dstring_init(&param_name);
    dstring_init(&label_name);

    // readString() -> String?
    SET_BUILTIN("readString", string, true);
    // readInt() -> Int?
    SET_BUILTIN("readInt", integer, true);
    // readDouble() -> Double?
    SET_BUILTIN("readDouble", double_, true);
    // write() // TODO params maybe
    SET_BUILTIN("write", nil, false);
    /* Allow the write function to have any number of parameters passed to it */
    symtable_search(&p->global_symtab, &builtin_id, &st_err)->variadic_param = true;
    ADD_BUILTIN_PARAM("_", "_", undefined, false);
    // Int2Double(_ term : Int) -> Double
    SET_BUILTIN("Int2Double", double_, false);
    ADD_BUILTIN_PARAM("_", "term", integer, false);
    // Double2Int(_ term: Double) -> Int
    SET_BUILTIN("Double2Int", integer, false);
    ADD_BUILTIN_PARAM("_", "term", double_, false);
    // length(_ s: String) -> Int
    SET_BUILTIN("length", integer, false);
    ADD_BUILTIN_PARAM("_", "s", string, false);
    // substring(of s: String, startingAt i : Int, endingBefore j : Int) -> String?
    SET_BUILTIN("substring", string, true);
    ADD_BUILTIN_PARAM("of", "s", string, false);
    ADD_BUILTIN_PARAM("startingAt", "i", integer, false);
    ADD_BUILTIN_PARAM("endingBefore", "j", integer, false);
    code_generator_substring();
    // ord(_ c: String) -> Int
    SET_BUILTIN("ord", integer, false);
    ADD_BUILTIN_PARAM("_", "c", string, false);
    code_generator_function_ord();
    // chr(_ i: Int) -> String
    SET_BUILTIN("chr", string, false);
    ADD_BUILTIN_PARAM("_", "i", integer, false);

    dstring_free(&builtin_id);
    dstring_free(&param_name);
    dstring_free(&label_name);
    if (st_err) {
        return false;
    }
    return true;
}

/**
 * @brief Loads all tokens from input and fills out the token buffer with them
 *
 * @param p Parser structure
 * @return int relevant return code
 */
uint32_t parser_fill_buffer(Parser* p) {
    int res;
    do {
        if ((res = get_token(&p->curr_tok)))
            return res;
        tb_push(&p->buffer, p->curr_tok);

    } while (p->curr_tok.type != TOKEN_EOF);

    p->buffer.runner = p->buffer.head;
    return ERR_NO_ERR;
}

/**
 * @brief Iterates through the token buffer skips all tokens except
 *        for function declarations
 *
 * @param p Parser object
 * @return relevant error code or 0 on success
 */
uint32_t parser_get_func_decls(Parser* p) {
    uint32_t res;
    /* Get the first token */
    p->curr_tok = tb_get_token(&p->buffer);
    if (p->curr_tok.type == TOKEN_UNDEFINED)
        return ERR_INTERNAL;

    NEXT_RULE(skip);
    return EXIT_SUCCESS;
}

/**
 * @brief Parser entry point
 *
 * @return uint32 0 on success, otherwise relevant return code
 */
uint32_t parse() {
    uint32_t res = 0;
    Parser p;

    /* Initialize Parser structure */
    if (!parser_init(&p)) {
        fprintf(stderr, "[ERROR %d] Initializing parser data failed\n", ERR_INTERNAL);
        return ERR_INTERNAL;
    }
    DEBUG_PRINT("parser initialized");
    
    /* Generate header before generating any other code*/
    code_generator_prolog();

    /* Add builtin functions to the global symtable */
    if (!add_builtins(&p)) {
        parser_dispose(&p);
        fprintf(stderr, "[ERROR %d] adding builtin functions failed\n", ERR_INTERNAL);
        return ERR_INTERNAL;
    }
    DEBUG_PRINT("builtins added");

    /* Load tokens from input and fill the token buffer */
    if ((res = parser_fill_buffer(&p))) {
        parser_dispose(&p);
        fprintf(stderr, "[ERROR %d] loading tokens to buffer failed\n", res);
        return res;
    }
    DEBUG_PRINT("buffer filled");

    /* First run through the token buffer to collect all function declaration */
    if ((res = parser_get_func_decls(&p))) {
        parser_dispose(&p);
        return res;
    }
    DEBUG_PRINT("First token buffer iteration finished");

    /* Reset runner to the beggining of the list after first run */
    p.buffer.runner = p.buffer.head;
    p.curr_tok = tb_get_token(&p.buffer);
    if ((p.curr_tok.type == TOKEN_UNDEFINED) && (p.curr_tok.value.int_val == 0)) {
        fprintf(stderr, "[ERROR %d] Getting first token from buffer failed\n", ERR_INTERNAL);
        return ERR_INTERNAL;
    };

    /* Start recursive descend */
    if ((res = prog(&p))) {
        WARNING_PRINT("prog not 0");
        parser_dispose(&p);
        return res;
    }

    parser_dispose(&p);
    return EXIT_SUCCESS;
}