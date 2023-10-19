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
static Rule prog();
static Rule stmt_list();
static Rule stmt();
static Rule var_def();
static Rule var_def_cont();
static Rule opt_assign();
static Rule id_contruct();
static Rule expr_type();
static Rule while_loop();
static Rule condition();
static Rule cond_clause();
static Rule arg_list();
static Rule arg_next();
static Rule arg();
static Rule func_def();
static Rule param_list();
static Rule param_next();
static Rule param();
static Rule param_label();
static Rule block_body();
static Rule func_body();
static Rule func_stmt();
static Rule func_ret_type();
static Rule opt_ret();