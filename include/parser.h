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



/**
 * Structure containing the current state of the parser
 */
typedef struct parser_t {
    dstring_t tmp;             // Temporary helper string
    bool in_function;          // Parser is inside a function call
    bool in_declaration;       // Parser is inside a function declaration
    bool in_cond;              // Parser is inside a condition statement
    bool in_loop;              // Parser is inside a while loop
    bool in_param;             // Parser should set param type
    token_T curr_tok;          // Currently processed token
    symtab_item_t* current_id; // Identifier of currently processed function
    symtab_item_t* left_id;    // Identifier of left-side variable
    symtab_item_t* right_id;   // Identifier of right-side function
    symtab_t global_symtab;    // Global symbol table
    scope_t local_symtab;      // Local symbol table
} Parser;

// --------------------------------------------------------------------------------------
/*
 Macros used only for adding builtin functions 
 It's not really necessary to have a different macro just for builtins                
 however I wanted to reduce the verbosity of that function                            
*/

#define SET_BUILTIN(_func_name, _ret_type, _nilable)                                                                   \
    dstring_clear(&builtin_id);                                                                                        \
    dstring_add_const_str(&builtin_id, _func_name);                                                                    \
    symtable_insert(&p->global_symtab, &builtin_id, &st_err);                                                          \
    set_type(&p->global_symtab, &builtin_id, function, &st_err);                                                       \
    set_return_type(&p->global_symtab, &builtin_id, _ret_type, &st_err);                                               \
    set_nillable(&p->global_symtab, &builtin_id, _nilable, &st_err);                                                   \
    set_func_definition(&p->global_symtab, &builtin_id, true, &st_err)

#define ADD_BUILTIN_PARAM(_label, _param_name, _param_type, _nilable)                                                  \
    dstring_clear(&label_name);                                                                                        \
    dstring_clear(&param_name);                                                                                        \
    dstring_add_const_str(&label_name, _label);                                                                        \
    dstring_add_const_str(&param_name, _param_name);                                                                   \
    add_param(&p->global_symtab, &builtin_id, &param_name, &st_err);                                                   \
    set_param_label(&p->global_symtab, &builtin_id, &param_name, &label_name, &st_err);                                \
    set_param_type(&p->global_symtab, &builtin_id, &param_name, _param_type, &st_err);                                 \
    set_param_nil(&p->global_symtab, &builtin_id, &param_name, _nilable, &st_err)

// -------------------------------------------------------------------------------------

/* Adding new parameters should only be accesible from function declaration rule, thus the scope should always be global, 
   and the function we want to add parameters to should be stored in Parser.current_id, the name of the parameter should be
   the last loaded token (identifier), thus Parser.curr_tok.value.string_val is the name of the param  
*/

#define NEW_PARAM()                                                                                                    \
    add_param(&p->global_symtab, &p->current_id->name, &p->curr_tok.value.string_val, &err);                           \
    switch (err) {                                                                                                     \
        case SYMTAB_ERR_ITEM_NOT_FOUND:                                                                                \
            print_error(ERR_UNDEFINED_FUNCTION, "Identifier %s not defined", &p->current_id->name.str);                \
            return ERR_UNDEFINED_FUNCTION;                                                                             \
        case SYMTAB_ERR_ITEM_NOT_FUNCTION:                                                                             \
            print_error(ERR_SEMANTIC, "Identifier %s is not a function", &p->current_id->name.str);                    \
            return ERR_SEMANTIC;                                                                                       \
        default: break;                                                                                                \
    }

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
Rule nilable(Parser* p);
Rule opt_arg(Parser* p);
Rule term(Parser* p);
Rule literal(Parser* p);

/**
 * @brief Initializes data needed by the parser
 *
 * @param p - Parser object
 * @return true on success
 */
bool parser_init(Parser* p);

/**
 * @brief Frees all data allocated by the parser
 * @param p
 */
void parser_dispose(Parser* p);

/**
 * @brief Parser entry point
 * 
 * @return uint ret_code
 */
unsigned int parse();

/**
 * @brief Checks whether currently processed ID is a function ID 
 * 
 * @param p 
 * @return true if function 
 * @return false otherwise
 */
bool is_function(Parser* p);

#endif