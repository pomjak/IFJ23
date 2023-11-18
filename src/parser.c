/**
 * @file parser.c
 * @author Simon Cagala (xcagal00@stud.fit.vutbr.cz)
 * @author Jakub Pomsar (xpomsa00@stud.fit.vutbr.cz)
 * @brief
 * @date 2023-11-09
 */
#include "parser.h"

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

    switch (p->curr_tok.type)
    {
    case TOKEN_EOF:
        break;
    case TOKEN_FUNC:
        p->in_declaration = true;
        tb_next(&p->buffer);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);

        // p->last_func_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        // p->last_func_id->type = function;

        tb_next(&p->buffer);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_L_PAR);

        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(param_list_skip);

        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(func_ret_type_skip);
        ASSERT_TOK_TYPE(TOKEN_L_BKT);

        tb_next(&p->buffer);
        GET_TOKEN();
        add_scope(&p->stack, &err);
        NEXT_RULE(func_body);
        pop_scope(&p->stack, &err);

        p->in_declaration = false;
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(prog);
        break;
    default:
        NEXT_RULE(stmt);
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
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(define);
        p->current_id->is_mutable = true;
        break;
    case TOKEN_LET: /* let <define> */
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(define);
        p->current_id->is_mutable = false;
        break;
    case TOKEN_IDENTIFIER: /* ID<expression_type> */
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
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(expr_type);
        break;
    case TOKEN_WHILE: /* while EXP { <block_body> */
        p->in_loop++;

        /* TODO EXPRESSION PROCESSING HERE */

        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_R_BKT);
        GET_TOKEN();
        NEXT_RULE(block_body);
        p->in_loop--;
        break;
    case TOKEN_IF: /* if <cond_clause> { <block_body> else { <block_body> */
        p->in_cond++;
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(cond_clause);
        DEBUG_PRINT("cond_clause finished");

        tb_next(&p->buffer);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_L_BKT);

        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(block_body);

        tb_next(&p->buffer);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_ELSE);
        add_scope(&p->stack,&err);//adding scope for else
        tb_next(&p->buffer);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_L_BKT);
        DEBUG_PRINT("{ start of body");

        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(block_body);

        tb_next(&p->buffer);
        GET_TOKEN();
        p->in_cond--; // condition should be fully parsed by the time we're exiting the switch statement
        break;
    default:
        fprintf(stderr, "[ERROR %d] Unexpected token, var/let/while/if/identifier expected", ERR_SYNTAX

        );
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
    if ((p->in_cond > 0) || (p->in_loop > 0) || p->in_declaration)
    {
        DEBUG_PRINT("define : in cond, loop or decl");
        if (!peek_scope(p->stack)) {
            return ERR_INTERNAL; //TODO;
        }
        else {
            p->current_id = symtable_search(p->stack->local_sym, &p->curr_tok.value.string_val, &err);
        }

        // /* search local */
        // DEBUG_PRINT("before search scopes");
        // p->current_id = search_scopes(p->stack, &p->curr_tok.value.string_val, &err);
        // DEBUG_PRINT("after search scopes");
        if (p->current_id) {
            fprintf(stderr, "[ERROR %d] Variable %s already defined\n", ERR_REDEFINING_VARIABLE, p->current_id->name.str);
            return ERR_REDEFINING_VARIABLE;
        }
        /* Add symbol to local symtable */
        symtable_insert(p->stack->local_sym, &p->curr_tok.value.string_val, &err);
        /* TODO switch for symtable error */
        p->current_id = symtable_search(p->stack->local_sym, &p->curr_tok.value.string_val, &err);
        DEBUG_PRINT("%s inserted to local symtab", p->current_id->name.str);
        /* TODO ERR CHECK */
    }
    else {
        DEBUG_PRINT("global scope");
        p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        if (p->current_id) {

            return ERR_UNDEFINED_FUNCTION;
        }

        symtable_insert(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
    }

    tb_next(&p->buffer);
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
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(type);
        NEXT_RULE(opt_assign);
        break;
    case TOKEN_ASS:
        //  EXP
        break;
    default:
        fprintf(stderr, "[ERROR %d] Unexpected token after variable identifier", ERR_SYNTAX);
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
        // TODO expressions
    }
    /* epsilon */
    return EXIT_SUCCESS;
}

/**
 * @brief <expr_type> -> = EXP | ( <arg_list>
 */
Rule expr_type(Parser* p) {
    RULE_PRINT("expr_type");
    uint32_t res, err;

    switch (p->curr_tok.type) {
            /* If the previously loaded identifier was't found in any symtable we have to
            determenine whether to return ERR_UNDEFINED_VARIABLE or
                ERR_UNDEFINED_FUNCTION in this rule */
    case TOKEN_ASS:
        /* If assignment is the next step after loading the identifier, the ID was a variable */
        if (!p->current_id) {
            return ERR_UNDEFINED_VARIABLE;
        }
        switch (p->current_id->type) {
        case integer:
        case double_:
        case string: break;
        default: return ERR_UNDEFINED_VARIABLE; break;
        }
    case TOKEN_L_PAR:
        /* If the loaded ID is followed by opening parentheses the ID should have been a function */
        if (!p->current_id) {
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

        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(arg_list);
        break;
    default:
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

    if (p->curr_tok.type == TOKEN_LET) {
        DEBUG_PRINT("Before getting ID tok");
        tb_next(&p->buffer);
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
        /* Add a local scope for the body of the if statement */
        add_scope(&p->stack, &err);
        DEBUG_PRINT("Local scope for if created");
        /* And insert the symbol into the newly created local scope */
        symtable_insert(p->stack->local_sym, &p->current_id->name, &err);
        set_nillable(p->stack->local_sym, &p->current_id->name, false, &err);
        set_type(p->stack->local_sym, &p->current_id->name, p->current_id->type, &err);
        set_mutability(p->stack->local_sym, &p->current_id->name, false, &err);
        DEBUG_PRINT("%s inserted into local if scope", p->current_id->name.str);

    }
    else {
     // expressions
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief <arg_list> -> <arg> <arg_next> | )
 */
Rule arg_list(Parser* p) {
    RULE_PRINT("arg_list");
    uint32_t res, err;
    p->current_arg = p->last_func_id->parameters;
    if (p->curr_tok.type == TOKEN_R_PAR) {
        /* Function had parameters declared but no arguments were passed while calling */
        if (p->current_arg != NULL) {
            fprintf(stderr, "[ERROR %d] Missing arguments in function %s \n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
            return ERR_FUNCTION_PARAMETER;
        }
        tb_next(&p->buffer);
        GET_TOKEN();
        return EXIT_SUCCESS;
    }
    if (p->current_arg == NULL) {
        fprintf(stderr, "[ERROR %d] Too many arguments in function %s\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
        return ERR_FUNCTION_PARAMETER;
    }
    NEXT_RULE(arg);
    tb_next(&p->buffer);
    GET_TOKEN();

    NEXT_RULE(arg_next);
    return EXIT_SUCCESS;
}

/**
 * @brief <arg_next> -> , <arg> <arg_next> | )
 */
Rule arg_next(Parser* p) {
    RULE_PRINT("arg_next");
    uint32_t res, err;

    switch (p->curr_tok.type) {
    case TOKEN_COMMA:

        if (p->current_arg->next == NULL) {
            fprintf(stderr, "[ERROR %d] Too many arguments in function %s\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
            return ERR_FUNCTION_PARAMETER;
        }
        p->current_arg = p->current_arg->next;
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(arg);
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(arg_next);
        break;
    case TOKEN_R_PAR:
        if (p->current_arg->next != NULL) {
            fprintf(stderr, "[ERROR %d] Missing arguments in function %s \n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
            return ERR_FUNCTION_PARAMETER;
        }
        tb_next(&p->buffer);
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
            if (peek_scope(p->stack)) {
                p->current_id = search_scopes(p->stack, &p->curr_tok.value.string_val, &err);
            }
            if (!p->current_id) {
                p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
            }
            if (!p->current_id) {
                fprintf(stderr, "[ERROR %d] Unknown identifier %s\n", ERR_UNDEFINED_VARIABLE, p->curr_tok.value.string_val.str);
                return ERR_UNDEFINED_VARIABLE;
            }

            /* Check if the variable is the same type as function parameter */
            if (p->current_id->type != p->current_arg->type) {
                fprintf(stderr, "[ERROR %d] Function %s: Invalid argument type in %s\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str, p->current_id->name.str);
                return ERR_FUNCTION_PARAMETER;
            }
            // TODO maybe get token here
            return EXIT_SUCCESS;
        }

        /* Assert validity of the label */
        if (dstring_cmp(&p->current_arg->label, &p->curr_tok.value.string_val)) {
            fprintf(stderr, "[ERROR %d] Unknown label %s in function %s\n", ERR_SEMANTIC, p->curr_tok.value.string_val.str, p->last_func_id->name.str);
            return ERR_SEMANTIC;
        }

        // symtable stuff?
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(opt_arg);
    }
    else {
        NEXT_RULE(literal);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <param_list> -> <param> <param_next> | )
 */
Rule param_list(Parser* p) {
    RULE_PRINT("param_list");
    uint32_t res, err;
    p->in_param = true;
    if (p->curr_tok.type == TOKEN_R_PAR) {
        p->in_param = false;
        return EXIT_SUCCESS;
    }
    NEXT_RULE(param);
    // GET_TOKEN();
    NEXT_RULE(param_next);
    return EXIT_SUCCESS;
}

/**
 * @brief <param_next> -> , <param> <param_next> | )
 */
Rule param_next(Parser* p) {
    RULE_PRINT("param_next");
    uint32_t res, err;

    switch (p->curr_tok.type) {
    case TOKEN_R_PAR: p->in_param = false; break;
    case TOKEN_COMMA:
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(param);
        // GET_TOKEN();
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

    tb_next(&p->buffer);
    GET_TOKEN();
    DEBUG_PRINT("GET_TOKEN, ID expected");
    ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
    NEW_PARAM();

    set_param_label(&p->global_symtab, &p->last_func_id->name, &p->curr_tok.value.string_val, &p->tmp, &err);
    DEBUG_PRINT("Param label set");
    dstring_clear(&p->tmp);
    dstring_copy(&p->curr_tok.value.string_val, &p->tmp);

    tb_next(&p->buffer);
    GET_TOKEN();
    DEBUG_PRINT("GET_TOKEN, colon expected");
    ASSERT_TOK_TYPE(TOKEN_COL);

    tb_next(&p->buffer);
    GET_TOKEN();
    DEBUG_PRINT("GET_TOKEN, goto type");
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
        pop_scope(&p->stack,&err);
        return EXIT_SUCCESS;
    }
    else {
        NEXT_RULE(stmt);
        // GET_TOKEN();
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

    if (p->curr_tok.type == TOKEN_R_BKT) {
        DEBUG_PRINT("}");
        // pop_scope(&p->stack, &err);
        return EXIT_SUCCESS;
    }
    else {
        NEXT_RULE(func_stmt);
        // tb_next(&p->buffer);
        // GET_TOKEN();
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
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(define);
        p->current_id->is_mutable = true;
        break;
    case TOKEN_LET:
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(define);
        p->current_id->is_mutable = false;
        break;
    case TOKEN_IDENTIFIER:
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
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(expr_type);
        break;
    case TOKEN_WHILE:
        /* TODO EXPRESSION */
        tb_next(&p->buffer);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_L_BKT);
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(func_body);
        break;
    case TOKEN_IF:
        p->in_cond++;
        add_scope(&p->stack, &err);
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(cond_clause);

        tb_next(&p->buffer);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_L_BKT);

        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(func_body);

        tb_next(&p->buffer);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_ELSE);
        // ?? check if if was declared with LET ID ??

        tb_next(&p->buffer);
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_L_BKT);

        tb_next(&p->buffer);
        GET_TOKEN();
        add_scope(&p->stack, &err);
        NEXT_RULE(func_body);
        p->in_cond--;
        break;
    case TOKEN_RETURN:
        tb_next(&p->buffer);
        GET_TOKEN();
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
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(type);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <opt_ret> -> EXP | eps
 */
Rule opt_ret(Parser* p) {
    RULE_PRINT("opt_ret");
    uint32_t res, err;
    return EXIT_SUCCESS;
} /* TODO EXPRESSIONS */

/**
 * @brief <opt_type> ->  : <type> | eps
 */
Rule opt_type(Parser* p) {
    RULE_PRINT("opt_type");
    uint32_t res, err;

    if (p->curr_tok.type == TOKEN_COL) {
        tb_next(&p->buffer);
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
    uint32_t res, err;

    switch (p->curr_tok.type) {
    case TOKEN_DT_INT:
        DEBUG_PRINT("case TOKEN_DT_INT");
        if (p->in_declaration) {
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
        }
        tb_next(&p->buffer);
        GET_TOKEN();
        break;

    case TOKEN_DT_DOUBLE:
        DEBUG_PRINT("case TOKEN_DT_DOUBLE");
        if (p->in_declaration) {
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
        }
        tb_next(&p->buffer);
        GET_TOKEN();
        break;

    case TOKEN_DT_STRING:
        DEBUG_PRINT("case TOKEN_DT_STRING");
        if (p->in_declaration) {
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
        }
        tb_next(&p->buffer);
        GET_TOKEN();
        break;
    default:
        DEBUG_PRINT("default");
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
    uint32_t res, err;

    if (p->curr_tok.type == TOKEN_COL) {
        tb_next(&p->buffer);
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
        if (peek_scope(p->stack)) {
            DEBUG_PRINT("Searching %s in local scopes", p->curr_tok.value.string_val.str);
            p->current_id = search_scopes(p->stack, &p->curr_tok.value.string_val, &err);
        }
        if (!p->current_id) {
            DEBUG_PRINT("Searching %s in global scope", p->curr_tok.value.string_val.str);
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
    uint32_t res, err;

    switch (p->curr_tok.type) {
    case TOKEN_INT:
        if (p->current_arg->type != integer) {
            fprintf(stderr, "[ERROR %d] Invalid argument type(int) in fuction %s\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
            return ERR_FUNCTION_PARAMETER;
        }
        /* generate term value */
        break;
    case TOKEN_DBL:

        if (p->current_arg->type != double_) {
            fprintf(stderr, "[ERROR %d] Invalid argument type(double) in fuction %s\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
            return ERR_FUNCTION_PARAMETER;
        }
        break;
    case TOKEN_STRING:
        if (p->current_arg->type != string) {
            fprintf(stderr, "[ERROR %d] Invalid argument type(string) in fuction %s\n", ERR_FUNCTION_PARAMETER, p->last_func_id->name.str);
            return ERR_FUNCTION_PARAMETER;
        }
        break;
    default: return ERR_SYNTAX;
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

    p->in_declaration = true;
    tb_next(&p->buffer);
    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);

    symtable_insert(&p->global_symtab, &p->curr_tok.value.string_val, &err);
    if (err == SYMTAB_ERR_ITEM_ALREADY_STORED) {
        print_error(ERR_SEMANTIC, "Function already declared");
        return ERR_SEMANTIC;
    }
    p->last_func_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
    p->last_func_id->type = function;

    tb_next(&p->buffer);
    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_L_PAR);

    tb_next(&p->buffer);
    GET_TOKEN();
    NEXT_RULE(param_list);

    tb_next(&p->buffer);
    GET_TOKEN();
    NEXT_RULE(func_ret_type);
    p->in_declaration = false;
    DEBUG_PRINT("after func_ret_type; token = %d", p->curr_tok.type);
    return EXIT_SUCCESS;
}

Rule type_skip(Parser* p) {
    RULE_PRINT("type_skip");
    uint32_t res;

    switch (p->curr_tok.type)
    {
    case TOKEN_DT_INT:
    case TOKEN_DT_DOUBLE:
    case TOKEN_DT_STRING:
        tb_next(&p->buffer);
        GET_TOKEN();
        break;
    default:
        fprintf(stderr, "[ERROR %d] Type specifier expected\n", ERR_SYNTAX);
        return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

Rule param_skip(Parser* p) {
    RULE_PRINT("param_skip");
    uint32_t res;

    if (p->curr_tok.type != TOKEN_UND_SCR && p->curr_tok.type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "[ERROR %d] Parameter label expected\n", ERR_SYNTAX);
        return ERR_SYNTAX;
    }
    tb_next(&p->buffer);
    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);

    tb_next(&p->buffer);
    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_COL);

    tb_next(&p->buffer);
    GET_TOKEN();
    NEXT_RULE(type_skip);
    return EXIT_SUCCESS;
}

Rule param_next_skip(Parser* p) {
    RULE_PRINT("param_next_skip");
    uint32_t res;

    switch (p->curr_tok.type)
    {
    case TOKEN_R_PAR:
        break;
    case TOKEN_COMMA:
        tb_next(&p->buffer);
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

Rule param_list_skip(Parser* p) {
    RULE_PRINT("param_list_skip");
    uint32_t res;

    if (p->curr_tok.type == TOKEN_R_PAR) {
        return EXIT_SUCCESS;
    }
    NEXT_RULE(param_skip);
    NEXT_RULE(param_next_skip);
    return EXIT_SUCCESS;
}

Rule func_ret_type_skip(Parser* p) {
    RULE_PRINT("func_ret_type_skip");
    uint32_t res;

    if (p->curr_tok.type == TOKEN_RET_VAL) {
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(type_skip);
    }
    return EXIT_SUCCESS;
}

Rule skip(Parser* p) {
    uint32_t res;
    if (p->curr_tok.type == TOKEN_EOF) {
        DEBUG_PRINT("leaving with EOF");
        return EXIT_SUCCESS;
    }
    if (p->curr_tok.type == TOKEN_FUNC) {
        NEXT_RULE(func_header);
        tb_next(&p->buffer);
        GET_TOKEN();
        NEXT_RULE(skip);
        return EXIT_SUCCESS;
    }
    DEBUG_PRINT("skipping token: %d", p->curr_tok.type);
    tb_next(&p->buffer);
    GET_TOKEN();
    NEXT_RULE(skip);
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
    if (symtab_err) {
        return false;
    }

    /* Scope (stack of local symbol tables) initialization */
    init_scope(&p->stack);
    dstring_init(&p->tmp);

    /* Initialize token buffer */
    tb_init(&p->buffer);

    p->current_arg = NULL;
    p->current_id = NULL;
    p->last_func_id = NULL;
    p->in_cond = 0;
    p->in_declaration = false;
    p->in_function = false;
    p->in_loop = 0;
    p->in_param = false;
    p->type_expr = undefined;
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
    // tb_pop(&p->buffer);
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
    // ord(_ c: String) -> Int
    SET_BUILTIN("ord", integer, false);
    ADD_BUILTIN_PARAM("_", "c", string, false);
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
    uint32_t res;
    Parser p;

    /* Initialize Parser structure */
    if (!parser_init(&p)) {
        fprintf(stderr, "[ERROR %d] Initializing parser data failed\n", ERR_INTERNAL);
        return ERR_INTERNAL;
    }
    DEBUG_PRINT("parser initialized");
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
        ERROR_PRINT("parser_get_func_decl not 0");
        parser_dispose(&p);
        return res;
    }

    p.buffer.runner = p.buffer.head;

    p.curr_tok = tb_get_token(&p.buffer);                                                                     \
        if ((p.curr_tok.type == TOKEN_UNDEFINED) && (p.curr_tok.value.int_val == 0))                                      \
            return ERR_INTERNAL;

        /* Start recursive descend */
    if ((res = prog(&p))) {
        WARNING_PRINT("prog not 0");
        parser_dispose(&p);
        return res;
    }

    parser_dispose(&p);
    return EXIT_SUCCESS;
}