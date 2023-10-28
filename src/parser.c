#include "parser.h"

#define GET_TOKEN()                                                 \
    if ((res = get_token(&p->curr_tok)) != ERR_NO_ERR) return res 

#define ASSERT_TOK_TYPE(_type)                                      \
    if (p->curr_tok.type != _type) return ERR_SYNTAX

#define NEXT_RULE(_rule)                                            \
    if ((res = _rule(p)) != ERR_NO_ERR) return res

/**
 * @brief Initializes data needed by the parser
 *
 * @param p
 * @return bool
 */
static bool parser_init(Parser *p) {
    unsigned symtab_err;
    symtable_init(&p->global_symtab, &symtab_err);
    symtable_init(&p->local_symtab, &symtab_err);
    p->current_id = NULL;
    p->left_id = NULL;
    p->right_id = NULL;
    p->in_cond = false;
    p->in_declaration = false;
    p->in_function = false;
    p->in_loop = false;

    int err_ret;
    _add_builtins(p, &err_ret);
    if (err_ret) return false;
}
/**
 * @brief Free all data allocated by the parser
 *
 * @param p
 */
static void parser_dispose(Parser *p) {
    symtable_dispose(&p->global_symtab);
    symtable_dispose(&p->local_symtab);
}

static void _add_builtins(Parser *p, int *err) { return; }

int parse() {
    unsigned ret_code;
    Parser parse_data;
    if (ret_code = prog(&parse_data)) return ret_code;
    parser_dispose(&parse_data);
    return EXIT_SUCCESS;
}

//  Rule definitions

/**
 * @brief <prog> -> <stmt-list>
 */
static Rule prog(Parser *p) {
    unsigned res;
    GET_TOKEN();

    switch (p->curr_tok.type) {
        case TOKEN_EOF:
            return EXIT_SUCCESS;
        case TOKEN_FUNC:
            break;
        default:
            break;
    }
}

/**
 * @brief <stmt> -> <var-def> | <while_loop> | <condition>
 */
static Rule stmt(Parser *p) {
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_VAR:
            break;
        case TOKEN_LET:
            break;
        case TOKEN_IDENTIFIER:
            break;
        case TOKEN_WHILE:
            break;
        case TOKEN_IF:
            break;
        default:
            return ERR_SYNTAX;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief <define> -> ID <var_def_cont>
 */
static Rule define(Parser *p) {
    unsigned res;

    ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
    // symtable stuff
    NEXT_RULE(var_def_cont);

    return EXIT_SUCCESS;
}

/**
 * @brief <var_def_cont> -> : <type> <opt_assign> | <asign_exp>
 */
static Rule var_def_cont(Parser *p) {
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_COL:
            break;
        case TOKEN_ASS:
            break;
        default:
            return ERR_SYNTAX;
    }
}

/**
 * @brief <opt_assign> -> <assign_exp> | eps
 */
static Rule opt_assign(Parser *p) {
    unsigned res;

    ASSERT_TOK_TYPE(TOKEN_ASS);
    // expression

    return res;
}

/**
 * @brief <expr_type> -> <assign_exp> | ( <arg_list>
 */
static Rule expr_type(Parser *p) {
    unsigned res;

    switch (p->curr_tok.type) {
        case TOKEN_ASS:
            break;
        case TOKEN_L_PAR:
            GET_TOKEN();
            NEXT_RULE(arg_list);
        default:
            return ERR_SYNTAX;
    }
}

/**
 * @brief <cond_clause> -> EXP | let ID
 */
static Rule cond_clause(Parser *p) {
    unsigned res;

    if (p->curr_tok.type == TOKEN_LET) {
        GET_TOKEN();
        ASSERT_TOK_TYPE(TOKEN_IDENTIFIER);
        // mby symtable stuff?
        return EXIT_SUCCESS;
    } 
    else {
        // expressions
        return EXIT_SUCCESS;
    }
}

/**
 * @brief <arg_list> -> <arg> <arg_next> | )
 */
static Rule arg_list(Parser *p) {
    unsigned res;

    if(p->curr_tok.type == TOKEN_R_PAR) {
        return EXIT_SUCCESS;
    }
    else {
        NEXT_RULE(arg);
    }
}

/**
 * @brief <arg_next> -> , <arg> <arg_next> | )
 */
static Rule arg_next(Parser *p) {
    unsigned res;

    switch (p->curr_tok.type)
    {
    case TOKEN_COMMA:
        GET_TOKEN();
        NEXT_RULE(arg);
    case TOKEN_R_PAR:
        return EXIT_SUCCESS;    
    default:
        return ERR_SYNTAX;
    }
}

/**
 * @brief <arg> -> "ID" <optarg> | <literal>
 */
static Rule arg(Parser *p) {
    unsigned res;

    if(p->curr_tok.type == TOKEN_IDENTIFIER) {
        // symtable stuff?
        GET_TOKEN();
        NEXT_RULE(opt_arg);
    }
    else {
        NEXT_RULE(literal);
    }
}

/**
 * @brief <param_list> -> <param> <param_next>
 */
static Rule param_list(Parser *p) {
    unsigned res;

    if(p->curr_tok.type == TOKEN_R_PAR) {
        return EXIT_SUCCESS;
    }
    else {
        NEXT_RULE(param);
    }
}

/**
 * @brief <param_next> -> , <param> <param_next> | )
 */
static Rule param_next(Parser *p) {
    unsigned res;

    switch (p->curr_tok.type)
    {
    case TOKEN_R_PAR:
        return EXIT_SUCCESS;
    case TOKEN_COMMA:
        GET_TOKEN();
        NEXT_RULE(param);
    default:
        return ERR_SYNTAX;
    }
}

/**
 * @brief <param> -> <param_label> ID : <type>
 */
static Rule param(Parser *p);

/**
 * @brief <blk_body> -> <stmt> <blk_body> | }
 */
static Rule block_body(Parser *p);

/**
 * @brief <func_body> -> <func_stmt> <func_body> | }
 */
static Rule func_body(Parser *p);

/**
 * @brief <func_stmt> -> <var_def> | <id_construct> | while EXP { <func_body> |
 * if <cond_clause> { <func_body> else { <func_body> | return <opt_ret>
 */
static Rule func_stmt(Parser *p);

/**
 * @brief <func_ret_type> -> eps | -> <type>
 */
static Rule func_ret_type(Parser *p);

/**
 * @brief <opt_ret> -> EXP | eps
 */
static Rule opt_ret(Parser *p);

/**
 * @brief <opt_type> ->  : <type> | eps
 */
static Rule opt_type(Parser *p);

/**
 * @brief <type> -> Int<nilable> | String<nilable> | Double<nilable>
 */
static Rule type(Parser *p);

/**
 * @brief <nilable> -> ? | eps
 */
static Rule nilable(Parser *p);

/**
 * @brief <opt_arg> -> : <term> | eps
 */
static Rule opt_arg(Parser *p);

/**
 * @brief <term> -> ID | literal
 */
static Rule term(Parser *p);

/**
 * @brief <literal> -> INT_LIT | STR_LIT | DBL_LIT
 */
static Rule literal(Parser *p);