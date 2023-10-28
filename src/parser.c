#include "parser.h"

#define GET_TOKEN()                                   \
    if((res = get_token(&p->curr_tok)) != ERR_NO_ERR) \         
        return res                                    \

#define ASSERT_TOK_TYPE(_type)                        \
    if(&p->curr_tok.type != _type)                    \
        return ERR_SYNTAX                             \

/**
 * @brief Initializes data needed by the parser
 * 
 * @param p 
 * @return bool 
 */
static bool parser_init(Parser *p) 
{
    symtable_init(&p->global_symtab);
    symtable_init(&p->local_symtab);
    p->current_id = NULL;
    p->left_id = NULL;
    p->right_id = NULL;
    p->in_cond = false;
    p->in_declaration = false;
    p->in_function = false;
    p->in_loop = false;

    int err_ret;
    _add_builtins(p, &err_ret);
    if(err_ret) return false;
}
/**
 * @brief Free all data allocated by the parser
 * 
 * @param p 
 */
static void parser_dispose(Parser *p)
{
    symtable_dispose(&p->global_symtab);
    symtable_dispose(&p->local_symtab);
}

static void _add_builtins(Parser *p, int *err)
{
    return;
}

int parse()
{
    unsigned ret_code;
    Parser parse_data;

    if(ret_code = prog(&parse_data)) return ret_code;

    return EXIT_SUCCESS;
}


//  Rule definitions

/**
 * @brief <prog> -> <stmt-list>
 */
static Rule prog(Parser *p) {
    return stmt_list(p);
}

/**
 * @brief <stmt-list> -> <stmt> <stmt-list> | <func-def> <stmt-list> | EOF
 */
static Rule stmt_list(Parser *p);

/**
 * @brief <stmt> -> <var-def> | <id_construct> | <while_loop> | <condition>
 */
static Rule stmt(Parser *p);

/**
 * @brief <var-def> -> <mutb> ID <var_def_cont>
 */
static Rule var_def(Parser *p);

/**
 * @brief <var_def_cont> -> : <type> <opt_assign> | <asign_exp>
 */
static Rule var_def_cont(Parser *p);

/**
 * @brief <opt_assign> -> <assign_exp> | eps
 */
static Rule opt_assign(Parser *p);

/**
 * @brief <id_construct> -> ID <expr_type>
 */
static Rule id_contruct(Parser *p);

/**
 * @brief <expr_type> -> <assign_exp> | ( <arg_list> 
 */
static Rule expr_type(Parser *p);

/**
 * @brief <while_loop> -> while EXP { <blk_body>
 */
static Rule while_loop(Parser *p);

/**
 * @brief <condition> -> if <cond_clause> { <blk_body> else { <blk_body>
 */
static Rule condition(Parser *p);

/**
 * @brief <cond_clause> -> EXP | let ID
 */
static Rule cond_clause(Parser *p);

/**
 * @brief <arg_list> -> <arg> <arg_next>
 */
static Rule arg_list(Parser *p);

/**
 * @brief <arg_next> -> , <arg> <arg_next> | )
 */
static Rule arg_next(Parser *p);

/**
 * @brief <arg> -> ID <opt_arg> | <literal>
 */
static Rule arg(Parser *p);

/**
 * @brief <func_def> -> func ID( <param_list> <func_ret_type> { <func_body>
 */
static Rule func_def(Parser *p);

/**
 * @brief <param_list> -> <param> <param_next>
 */
static Rule param_list(Parser *p);

/**
 * @brief <param_next> -> , <param> <param_next> | )
 */
static Rule param_next(Parser *p);

/**
 * @brief <param> -> <param_label> ID : <type>
 */
static Rule param(Parser *p);

/**
 * @brief <param_label> -> _ | PARAM_ID
 */
static Rule param_label(Parser *p);

/**
 * @brief <blk_body> -> <stmt> <blk_body> | }
 */
static Rule block_body(Parser *p);

/**
 * @brief <func_body> -> <func_stmt> <func_body> | }
 */
static Rule func_body(Parser *p);

/**
 * @brief <func_stmt> -> <var_def> | <id_construct> | while EXP { <func_body> | if <cond_clause> { <func_body> else { <func_body> | return <opt_ret>
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
 * @brief <mutable> -> let | var
 */
static Rule mutable(Parser *p);

/**
 * @brief <literal> -> INT_LIT | STR_LIT | DBL_LIT
 */
static Rule literal(Parser *p);