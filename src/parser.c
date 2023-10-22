#include "parser.h"

/**
 * @brief Initializes data needed by the parser
 * 
 * @param p 
 * @return int 
 */
static int parser_init(Parser *p) 
{
    symtable_init(&p->global_symtab);
    symtable_init(&p->local_symtab);
}
/**
 * @brief Free all data allocated by the parser
 * 
 * @param p 
 */
static inline void parser_dispose(Parser *p)
{
    symtable_dispose(&p->global_symtab);
    symtable_dispose(&p->local_symtab);
}

static inline void _add_builtins(Parser *p, int *err)
{
    return;
}

int parse()
{
    return;
}


//  Rule definitions

/**
 * @brief <prog> -> <stmt-list>
 */
static Rule prog();

/**
 * @brief <stmt-list> -> <stmt> <stmt-list> | <func-def> <stmt-list> | EOF
 */
static Rule stmt_list();

/**
 * @brief <stmt> -> <var-def> | <id_construct> | <while_loop> | <condition>
 */
static Rule stmt();

/**
 * @brief <var-def> -> <mutb> ID <var_def_cont>
 */
static Rule var_def();

/**
 * @brief <var_def_cont> -> : <type> <opt_assign> | <asign_exp>
 */
static Rule var_def_cont();

/**
 * @brief <opt_assign> -> <assign_exp> | eps
 */
static Rule opt_assign();

/**
 * @brief <id_construct> -> ID <expr_type>
 */
static Rule id_contruct();

/**
 * @brief <expr_type> -> <assign_exp> | ( <arg_list> 
 */
static Rule expr_type();

/**
 * @brief <while_loop> -> while EXP { <blk_body>
 */
static Rule while_loop();

/**
 * @brief <condition> -> if <cond_clause> { <blk_body> else { <blk_body>
 */
static Rule condition();

/**
 * @brief <cond_clause> -> EXP | let ID
 */
static Rule cond_clause();

/**
 * @brief <arg_list> -> <arg> <arg_next>
 */
static Rule arg_list();

/**
 * @brief <arg_next> -> , <arg> <arg_next> | )
 */
static Rule arg_next();

/**
 * @brief <arg> -> ID <opt_arg> | <literal>
 */
static Rule arg();

/**
 * @brief <func_def> -> func ID( <param_list> <func_ret_type> { <func_body>
 */
static Rule func_def();

/**
 * @brief <param_list> -> <param> <param_next>
 */
static Rule param_list();

/**
 * @brief <param_next> -> , <param> <param_next> | )
 */
static Rule param_next();

/**
 * @brief <param> -> <param_label> ID : <type>
 */
static Rule param();

/**
 * @brief <param_label> -> _ | PARAM_ID
 */
static Rule param_label();

/**
 * @brief <blk_body> -> <stmt> <blk_body> | }
 */
static Rule block_body();

/**
 * @brief <func_body> -> <func_stmt> <func_body> | }
 */
static Rule func_body();

/**
 * @brief <func_stmt> -> <var_def> | <id_construct> | while EXP { <func_body> | if <cond_clause> { <func_body> else { <func_body> | return <opt_ret>
 */
static Rule func_stmt();

/**
 * @brief <func_ret_type> -> eps | -> <type>
 */
static Rule func_ret_type();

/**
 * @brief <opt_ret> -> EXP | eps
 */
static Rule opt_ret();

/**
 * @brief <opt_type> ->  : <type> | eps
 */
static Rule opt_type();

/**
 * @brief <type> -> Int<nilable> | String<nilable> | Double<nilable>
 */
static Rule type();

/**
 * @brief <nilable> -> ? | eps
 */
static Rule nilable();

/**
 * @brief <opt_arg> -> : <term> | eps
 */
static Rule opt_arg();

/**
 * @brief <term> -> ID | literal
 */
static Rule term();

/**
 * @brief <mutable> -> let | var
 */
static Rule mutable();

/**
 * @brief <literal> -> INT_LIT | STR_LIT | DBL_LIT
 */
static Rule literal();