/**
 * @file parser.h
 * @author Simon Cagala (xcagal00@stud.fit.vutbr.cz)
 * @brief
 * @date 2023-11-09
 */
#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "debug.h"
#include "error.h"
#include "lexical_analyzer.h"
#include "scope.h"
#include "symtable.h"
#include "token_buffer.h"

typedef struct expr_res {
    Type expr_type;
    bool nilable;
} expr_res_t;

/**
 * Structure containing the current state of the parser
 */
typedef struct parser_t {
    bool in_function;            // Parser is inside a function call
    bool in_func_head;         // Parser is inside a function declaration
    bool in_func_body;
    bool in_param;               // Parser should set param type
    bool return_found;
    bool first_stmt;            
    uint32_t in_cond;                // Parser is inside a condition statement
    uint32_t in_loop;                // Parser is inside a while loop
    param_t* current_arg;        // Current function argument list
    token_T curr_tok;            // Currently processed token
    symtab_item_t* lhs_id;       // Identifier on the left hand side of assignment
    symtab_item_t* current_id;   // Identifier of currently processed function
    symtab_item_t* last_func_id; // Identifier of the last loaded function
    symtab_item_t* rhs_id;       // Identifier in the right hand side of assignment
    symtab_t global_symtab;      // Global symbol table
    scope_t stack;               // Stack of local symbol tables, HEAD = current scope
    dstring_t tmp;               // Temporary helper string
    token_buffer_t buffer;       // List of tokens
    expr_res_t expr_res;         // Type of reduced expression 
} Parser;

/* ============================================| MACROS |============================================================ */

/* Add a new builtin function to global symtable, only use in add_builtins() */
#define SET_BUILTIN(_func_name, _ret_type, _nilable)                                                                   \
    dstring_clear(&builtin_id);                                                                                        \
    dstring_add_const_str(&builtin_id, _func_name);                                                                    \
    symtable_insert(&p->global_symtab, &builtin_id, &st_err);                                                          \
    set_type(&p->global_symtab, &builtin_id, function, &st_err);                                                       \
    set_return_type(&p->global_symtab, &builtin_id, _ret_type, &st_err);                                               \
    set_nillable(&p->global_symtab, &builtin_id, _nilable, &st_err);                                                   \
    set_func_definition(&p->global_symtab, &builtin_id, true, &st_err)

/* Add a parameter to a builtin function, only use in add_builtins() */
#define ADD_BUILTIN_PARAM(_label, _param_name, _param_type, _nilable)                                                  \
    dstring_clear(&label_name);                                                                                        \
    dstring_clear(&param_name);                                                                                        \
    dstring_add_const_str(&label_name, _label);                                                                        \
    dstring_add_const_str(&param_name, _param_name);                                                                   \
    add_param(&p->global_symtab, &builtin_id, &param_name, &st_err);                                                   \
    set_param_label(&p->global_symtab, &builtin_id, &param_name, &label_name, &st_err);                                \
    set_param_type(&p->global_symtab, &builtin_id, &param_name, _param_type, &st_err);                                 \
    set_param_nil(&p->global_symtab, &builtin_id, &param_name, _nilable, &st_err)

/* Adding new parameters should only be accesible from function declaration rule, thus the scope should always be global,
   and the function we want to add parameters to should be stored in Parser.current_id, the name of the parameter should be
   the last loaded token (identifier), thus Parser.curr_tok.value.string_val is the name of the param
*/
/* Add a new parameter to the last loaded function */
#define NEW_PARAM()                                                                                                    \
    add_param(&p->global_symtab, &p->last_func_id->name, &p->curr_tok.value.string_val, &err);                         \
    DEBUG_PRINT("NEW_PARAM: func = %s", p->last_func_id->name.str);                                                    \
    DEBUG_PRINT("NEW_PARAM: err = %d", err);                                                                           \
    switch (err) {                                                                                                     \
        case SYMTAB_ERR_ITEM_NOT_FOUND:                                                                                \
            print_error(ERR_UNDEFINED_FUNCTION, "Identifier not defined\n");                                           \
            return ERR_UNDEFINED_FUNCTION;                                                                             \
        case SYMTAB_ERR_ITEM_NOT_FUNCTION:                                                                             \
            print_error(ERR_SEMANTIC, "Identifier not a function\n");                                                  \
            return ERR_SEMANTIC;                                                                                       \
        default: break;                                                                                                \
    }

/* Load the next token or return an error code */
#define GET_TOKEN()                                                                                                    \
    tb_next(&p->buffer);                                                                                               \
    p->curr_tok = tb_get_token(&p->buffer);                                                                            \
    if((p->curr_tok.type == TOKEN_UNDEFINED) && (p->curr_tok.value.int_val == 0))                                      \
        return ERR_INTERNAL

/* Check if last loaded token is the correct type */
#define ASSERT_TOK_TYPE(_type)                                                                                         \
    if (p->curr_tok.type != _type) {                                                                                   \
        fprintf(stderr, "[ERROR 2] Unexpected token %d, expected %d\n", p->curr_tok.type, _type);                      \
        return ERR_SYNTAX;                                                                                             \
    }                                                                                     


/* Go to next rule, return (with a relevant exit code) if the exit code of the next rule was not 0 */
#define NEXT_RULE(_rule)                                                                                               \
    if ((res = _rule(p)) != ERR_NO_ERR)                                                                                \
    return res

/* Check if a new statement is on a separate line from the previous one */
#define CHECK_NEWLINE()                                                                                                \
    if ((!p->first_stmt) && (!p->curr_tok.preceding_eol)) {                                                            \
        DEBUG_PRINT("first:%d | precEOL : %d", p->first_stmt, p->curr_tok.preceding_eol);                              \
        fprintf(stderr, "[ERROR %d] New statement not on a separate line\n", ERR_SYNTAX);                              \
        return ERR_SYNTAX;                                                                                             \
    }

/* ================================================================================================================== */

typedef unsigned int Rule;

Rule prog(Parser* p);
Rule stmt(Parser* p);
Rule define(Parser* p);
Rule var_def_cont(Parser* p);
Rule opt_assign(Parser* p);
Rule expr_type(Parser* p);
Rule cond_clause(Parser* p);
Rule arg_list(Parser* p);
Rule arg_next(Parser* p);
Rule arg(Parser* p);
Rule param_list(Parser* p);
Rule param_next(Parser* p);
Rule param(Parser* p);
Rule block_body(Parser* p);
Rule func_body(Parser* p);
Rule func_stmt(Parser* p);
Rule func_ret_type(Parser* p);
Rule opt_ret(Parser* p);
Rule opt_type(Parser* p);
Rule type(Parser* p);
Rule opt_arg(Parser* p);
Rule term(Parser* p);
Rule literal(Parser* p);
Rule func_decl(Parser* p);
Rule skip(Parser* p);
Rule type_skip(Parser* p);
Rule func_ret_type_skip(Parser* p);
Rule param_list_skip(Parser* p);
Rule param_next_skip(Parser* p);
Rule param_skip(Parser* p);
Rule funccall(Parser* p);
bool parser_init(Parser* p);
bool add_builtins(Parser* p);
void parser_dispose(Parser* p);
uint32_t parser_fill_buffer(Parser* p);
uint32_t parse();

#endif