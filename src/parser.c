/**
 * @file parser.c
 * @author Simon Cagala (xcagal00@stud.fit.vutbr.cz)
 * @brief
 * @date 2023-11-09
 */
#include "parser.h"

#define GET_TOKEN()                                                                                                    \
    if ((res = get_token(&p->curr_tok)) != ERR_NO_ERR)                                                                 \
    return res

#define ASSERT_TOK_TYPE(_type)                                                                                         \
    if (p->curr_tok.type != _type)                                                                                     \
    return ERR_SYNTAX

#define NEXT_RULE(_rule)                                                                                               \
    if ((res = _rule(p)) != ERR_NO_ERR)                                                                                \
    return res

bool parser_init(Parser* p) {
    unsigned symtab_err;

    // Global symbol table initialization
    symtable_init(&p->global_symtab, &symtab_err);
    if (symtab_err) {
        return false;
    }

    // Scope (stack of local symbol tables) initialization
    init_scope(p->local_symtab);

    p->current_id = NULL;
    p->left_id = NULL;
    p->right_id = NULL;
    p->in_cond = false;
    p->in_declaration = false;
    p->in_function = false;
    p->in_loop = false;
}

void parser_dispose(Parser* p) {
    unsigned err;
    symtable_dispose(&p->global_symtab);
    dispose_scope(p->local_symtab, &err);
}

/**
 * @brief Fill global symtable with builtin functions
 *
 * @param p Parser object
 * @return true on success, otherwise false
 */
static bool add_builtins(Parser* p) {
    unsigned int st_err;
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
    // substring(of s: String, startingAt i : Int, endingBefore j : Int) ->
    // String?
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

unsigned int parse() {
    unsigned res;
    Parser p;

    // Initialize Parser structure
    if (!parser_init(&p)) {
        print_error(ERR_INTERNAL, "Error occured while initializing parser data");
        return ERR_INTERNAL;
    }
    // Add builtin functions to the global symtable
    if (!add_builtins(&p)) {
        parser_dispose(&p);
        print_error(ERR_INTERNAL, "Error occured while adding builtin functions");
        return ERR_INTERNAL;
    }

    /* Get the first token */
    if (res = get_token(&p.curr_tok)) {
        parser_dispose(&p);
        return res;
    }

    /* Start recursive descend */
    if (res = prog(&p)) {
        parser_dispose(&p);
        return res;
    }

    parser_dispose(&p);
    return EXIT_SUCCESS;
}

// ==================
//  Rule definitions
// ==================
/**
 * @brief <prog> -> <stmt> <prog> |
 *                  func ID ( <param_list> <func_ret_type> { <func_body> <prog> |
 *                  EOF
 */
static Rule prog(Parser* p) {
    DEBUG_PRINT("---Prog---");
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_EOF: break;
        case TOKEN_FUNC:
            p->in_declaration = true;
            GET_TOKEN();
            ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
            /* TODO CHECK ID IN SYMTABLE */

            GET_TOKEN();
            ASSERT_TOK_TYPE(TOKEN_L_PAR);
            GET_TOKEN();
            NEXT_RULE(param_list);

            GET_TOKEN();
            NEXT_RULE(func_ret_type);
            /* Don't have to get the next token here, after checking the return type, <nilable> is called and loads a new token either way */
            ASSERT_TOK_TYPE(TOKEN_R_BKT);
            GET_TOKEN();
            NEXT_RULE(func_body);
            
            p->in_declaration = false;
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
static Rule stmt(Parser* p) {
    DEBUG_PRINT("---Statement---");
    unsigned res;
    unsigned err;

    switch (p->curr_tok.type) {
        case TOKEN_VAR: /* var <define> */
            GET_TOKEN();
            NEXT_RULE(define);
            p->current_id->is_mutable = true;
            break;
        case TOKEN_LET: /* let <define> */
            GET_TOKEN();
            NEXT_RULE(define);
            p->current_id->is_mutable = false;
            break;
        case TOKEN_IDENTIFIER: /* ID<expression_type> */

            if (peek_scope(p->local_symtab)) {
                p->current_id = search_scopes(p->local_symtab, &p->curr_tok.value.string_val, &err);
            } else {
                p->current_id = NULL;
            }
            /* item wasn't found in any of the local scopes so we search global symtable */
            if (!p->current_id) {
                p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
            }
            GET_TOKEN();
            NEXT_RULE(expr_type);
            break;
        case TOKEN_WHILE: /* while EXP { <block_body> */
            p->in_loop = true;

            /* TODO EXPRESSION PROCESSING HERE */

            GET_TOKEN();
            ASSERT_TOK_TYPE(TOKEN_R_BKT);
            GET_TOKEN();
            NEXT_RULE(block_body);
            p->in_loop = false;
            break;
        case TOKEN_IF: /* if <cond_clause> { <block_body> else { <block_body> */
            p->in_cond = true;
            GET_TOKEN();
            NEXT_RULE(cond_clause);

            GET_TOKEN();
            ASSERT_TOK_TYPE(TOKEN_L_BKT);
            GET_TOKEN();
            NEXT_RULE(block_body);
            GET_TOKEN();
            ASSERT_TOK_TYPE(TOKEN_ELSE);
            GET_TOKEN();
            ASSERT_TOK_TYPE(TOKEN_L_BKT);
            GET_TOKEN();
            NEXT_RULE(block_body);
            p->in_cond = false; // condition should be fully parsed by the time we're exiting the switch statement
            break;
        default: print_error(ERR_SYNTAX, "Unexpected token, var/let/while/if/identifier expected"); return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <define> -> ID <var_def_cont>
 */
static Rule define(Parser* p) {
    DEBUG_PRINT("---Define---");
    unsigned res;
    unsigned err;

    ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
    if (p->in_cond || p->in_loop || p->in_function) {
        /* search local */
        p->current_id = search_scopes(p->local_symtab, &p->curr_tok.value.string_val, &err);
        if (p->current_id) {
            return ERR_UNDEFINED_FUNCTION;
        }
        /* Add symbol to local symtable */
        symtable_insert(p->local_symtab->local_sym, &p->curr_tok.value.string_val, &err);
        /* TODO switch for symtable error */
        p->current_id = symtable_search(p->local_symtab->local_sym, &p->curr_tok.value.string_val, &err);
        /* TODO ERR CHECK */
    } else {
        p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
        if (p->current_id) {
            return ERR_UNDEFINED_FUNCTION;
        }
        symtable_insert(p->local_symtab->local_sym, &p->curr_tok.value.string_val, &err);
        /* TOTO symtable error check */
        p->current_id = symtable_search(&p->global_symtab, &p->curr_tok.value.string_val, &err);
    }

    GET_TOKEN();
    NEXT_RULE(var_def_cont);

    return EXIT_SUCCESS;
}

/**
 * @brief <var_def_cont> -> : <type> <opt_assign> | = EXP
 */
static Rule var_def_cont(Parser* p) {
    DEBUG_PRINT("---VarDefCont---");
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_COL:
            GET_TOKEN();
            NEXT_RULE(type);
            break;
        case TOKEN_ASS: break;
        default: print_error(ERR_SYNTAX, "Unexpected token, : or = expected"); return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <opt_assign> -> = EXP | eps
 */
static Rule opt_assign(Parser* p) {
    DEBUG_PRINT("---OptAssign---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_ASS) {
        // TODO expressions
    } else {
        return EXIT_SUCCESS;
    }

    return res;
}

/**
 * @brief <expr_type> -> = EXP | ( <arg_list>
 */
static Rule expr_type(Parser* p) {
    DEBUG_PRINT("---ExpressionType---");
    unsigned res;

    switch (p->curr_tok.type) {
            /* If the previously loaded identifier was't found in any symtable we have to determenine whether to return ERR_UNDEFINED_VARIABLE or 
                ERR_UNDEFINED_FUNCTION in this rule */
        case TOKEN_ASS:
            /* If assignment is the next step after loading the identifier, the ID was a variable */
            DEBUG_PRINT("<expr_type> EXPRESSION");
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
                return ERR_UNDEFINED_FUNCTION;
            }
            GET_TOKEN();
            NEXT_RULE(arg_list);
            break;
        default: print_error(ERR_SYNTAX, "Unexpected token, = or (  expected"); return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <cond_clause> -> EXP | let ID
 */
static Rule cond_clause(Parser* p) {
    DEBUG_PRINT("---CondClause---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_LET) {
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
        // mby symtable stuff?
        return EXIT_SUCCESS;
    } else {
        // expressions
        return EXIT_SUCCESS;
    }
}

/**
 * @brief <arg_list> -> <arg> <arg_next> | )
 */
static Rule arg_list(Parser* p) {
    DEBUG_PRINT("---ArgList---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_R_PAR) {
        return EXIT_SUCCESS;
    }
    NEXT_RULE(arg);
    GET_TOKEN();
    NEXT_RULE(arg_next);
    return EXIT_SUCCESS;
}

/**
 * @brief <arg_next> -> , <arg> <arg_next> | )
 */
static Rule arg_next(Parser* p) {
    DEBUG_PRINT("---ArgNext---")
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_COMMA:
            GET_TOKEN();
            NEXT_RULE(arg);
            break;
        case TOKEN_R_PAR: return EXIT_SUCCESS;
        default: print_error(ERR_SYNTAX, "Unexpected token, comma or ) expected"); return ERR_SYNTAX;
    }
}

/**
 * @brief <arg> -> "ID" <optarg> | <literal>
 */
static Rule arg(Parser* p) {
    DEBUG_PRINT("---Arg---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_IDENTIFIER) {
        // symtable stuff?
        GET_TOKEN();
        NEXT_RULE(opt_arg);
    } else {
        NEXT_RULE(literal);
    }
};

/**
 * @brief <param_list> -> <param> <param_next> | )
 */
static Rule param_list(Parser* p) {
    DEBUG_PRINT("---ParamList---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_R_PAR) {
        return EXIT_SUCCESS;
    }
    NEXT_RULE(param);
    GET_TOKEN();
    NEXT_RULE(param_next);
    return EXIT_SUCCESS;
}

/**
 * @brief <param_next> -> , <param> <param_next> | )
 */
static Rule param_next(Parser* p) {
    DEBUG_PRINT("---ParamNext---");
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_R_PAR: break;
        case TOKEN_COMMA:
            GET_TOKEN();
            NEXT_RULE(param);
            GET_TOKEN();
            NEXT_RULE(param_next);
            break;
        default: print_error(ERR_SYNTAX, "Unexpected Token, ) or , expected"); return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <param> ->  "_" "ID" ":" <type> | "ID" "ID" ":" <type>
 */
static Rule param(Parser* p) {
    DEBUG_PRINT("---Param---");
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_UND_SCR:
        case TOKEN_IDENTIFIER: break;
        default: print_error(ERR_SYNTAX, "Unexpected token, identifier or _ expected"); return ERR_SYNTAX;
    }
    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
    // symtable ID stuff
    GET_TOKEN();
    ASSERT_TOK_TYPE(TOKEN_COL);

    GET_TOKEN();
    NEXT_RULE(type);

    return EXIT_SUCCESS;
}

/**
 * @brief <blk_body> -> <stmt> <blk_body> | }
 */
static Rule block_body(Parser* p) {
    unsigned res;

    if (p->curr_tok.type == TOKEN_R_BKT) {
        return EXIT_SUCCESS;
    } else {
        NEXT_RULE(stmt);
        GET_TOKEN();
        NEXT_RULE(block_body);
    }
}

/**
 * @brief <func_body> -> <func_stmt> <func_body> | }
 */
static Rule func_body(Parser* p) {
    DEBUG_PRINT("---FuncBody---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_R_BKT) {
        return EXIT_SUCCESS;
    } else {
        NEXT_RULE(func_stmt);
        GET_TOKEN();
        NEXT_RULE(func_body);
    }
}

/**
 * @brief <func_stmt> -> var <def> |
 *                       let <def> |
 *                       ID <expression-type> | 
 *                       while EXP { <func_body> |
 *                       if <cond_clause> { <func_body> else { <func_body> |
 *                       return <opt_ret>
 */
static Rule func_stmt(Parser* p) {
    DEBUG_PRINT("---FuncStatement---");
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_VAR:
        case TOKEN_LET:
            GET_TOKEN();
            NEXT_RULE(define);
            break;
        case TOKEN_IDENTIFIER:
            GET_TOKEN();
            NEXT_RULE(expr_type);
            break;
        case TOKEN_WHILE:
            /* TODO EXPRESSION */

            GET_TOKEN();
            ASSERT_TOK_TYPE(TOKEN_L_BKT);
            GET_TOKEN();
            NEXT_RULE(func_body);
            break;
        case TOKEN_IF:
            GET_TOKEN();
            NEXT_RULE(cond_clause);

            GET_TOKEN();
            ASSERT_TOK_TYPE(TOKEN_L_BKT);

            GET_TOKEN();
            NEXT_RULE(func_body);

            GET_TOKEN();
            ASSERT_TOK_TYPE(TOKEN_ELSE);
            GET_TOKEN();
            ASSERT_TOK_TYPE(TOKEN_L_BKT);
            GET_TOKEN();
            NEXT_RULE(func_body);
            break;
        case TOKEN_RETURN:
            GET_TOKEN();
            NEXT_RULE(opt_ret);
            break;
        default: return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <func_ret_type> =>  eps | -> <type>
 */
static Rule func_ret_type(Parser* p) {
    DEBUG_PRINT("---FuncRetType---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_SUB) {
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_GT);
        GET_TOKEN();
        NEXT_RULE(type);
    } else {
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <opt_ret> -> EXP | eps
 */
static Rule opt_ret(Parser* p) {
    DEBUG_PRINT("---OptRet---");
    unsigned res;
    return EXIT_SUCCESS;
} /* TODO EXPRESSIONS */

/**
 * @brief <opt_type> ->  : <type> | eps
 */
static Rule opt_type(Parser* p) {
    DEBUG_PRINT("---OptType---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_COL) {
        GET_TOKEN();
        NEXT_RULE(type);
    } else {
        return EXIT_SUCCESS;
    }
}

/**
 * @brief <type> -> Int<nilable> | String<nilable> | Double<nilable>
 */
static Rule type(Parser* p) {
    DEBUG_PRINT("---Type---");
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_DT_INT:
            p->current_id->type = integer;
            GET_TOKEN();
            NEXT_RULE(nilable);
            break;
        case TOKEN_DT_DOUBLE:
            p->current_id->type = double_;
            GET_TOKEN();
            NEXT_RULE(nilable);
            break;
        case TOKEN_DT_STRING:
            p->current_id->type = string;
            GET_TOKEN();
            NEXT_RULE(nilable);
            break;
        default: return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <nilable> -> ? | eps
 */
static Rule nilable(Parser* p) {
    DEBUG_PRINT("---Nilable---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_NIL_CHECK) {
        p->current_id->is_nillable = true;
        GET_TOKEN();
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <opt_arg> -> : <term> | eps
 */
static Rule opt_arg(Parser* p) {
    DEBUG_PRINT("---OptArg---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_COL) {
        GET_TOKEN();
        NEXT_RULE(term);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <term> -> ID | literal
 */
static Rule term(Parser* p) {
    DEBUG_PRINT("---Term---");
    unsigned res;

    if (p->curr_tok.type == TOKEN_IDENTIFIER) {
        /* Check id in symtable */
    } else {
        NEXT_RULE(literal);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <literal> -> INT_LIT | STR_LIT | DBL_LIT
 */
static Rule literal(Parser* p) {
    DEBUG_PRINT("---Literal---");
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_INT:
            /* generate term value */
            break;
        case TOKEN_DBL: break;
        case TOKEN_STRING: break;
        default: return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}