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
#include "code_generator.h"

typedef struct expr_res {
    Type expr_type;
    bool nilable;
} expr_res_t;

/**
 * Structure containing the current state of the parser
 */
typedef struct parser_t {
    bool in_function;            // Parser is inside a function call
    bool in_func_head;           // Parser is inside a function declaration
    bool in_func_body;           // Parser is inside function body
    bool in_param;               // Parser should set param type
    bool return_found;           // Flag marking the presence of a return statement in a function
    bool first_stmt;             // Flag marking whether the current statement is the first one of the program or first inside if/while/function body
    uint32_t in_cond;            // Parser is inside a condition statement
    uint32_t in_loop;            // Parser is inside a while loop
    uint32_t param_cnt;          // Current function parameter counter
    uint32_t cond_uid;           // Unique ID of conditional statements
    uint32_t loop_uid;           // Unique ID of while loops
    param_t* current_arg;        // Current function argument list
    token_T curr_tok;            // Currently processed token
    symtab_item_t* lhs_id;       // Identifier on the left hand side of assignment
    symtab_item_t* current_id;   // Identifier of currently processed function
    symtab_item_t* last_func_id; // Identifier of the last loaded function
    symtab_item_t* rhs_id;       // Identifier in the right hand side of assignment
    symtab_t global_symtab;      // Global symbol table
    scope_t stack;               // Stack of local symbol tables, HEAD = current scope
    dstring_t tmp;               // Temporary helper string
    token_buffer_t buffer;       // Double Link List of loaded tokens
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

/**
 * <prog> =>
 * <stmt> <prog>
 * func ID ( <param_list> <func_ret_type> { <func_body> <prog>
 * EOF
 */
Rule prog(Parser* p);
/**
 * <stmt> =>
 * var <define>
 * let <define>
 * ID <expression_type>
 * if <cond_clause> { <block_body> else { <block_body>
 * while EXP { <block_body>
 */
Rule stmt(Parser* p);
/**
 * <define> =>
 * ID <var_def_cont>
 */
Rule define(Parser* p);
/**
 * <var_def_cont> =>
 *  : <type> <opt_assign>
 *  = EXP
 *  = <funccall>
 */
Rule var_def_cont(Parser* p);
/**
 * <opt_assign> =>
 * = EXP
 * = <funccall>
 * ε
 */
Rule opt_assign(Parser* p);
/**
 * <expr_type> =>
 * = EXP
 * = <funccall>
 * ( <arg_list>
 */
Rule expr_type(Parser* p);
/**
 * <cond_clause> =>
 * EXP
 * let ID
 */
Rule cond_clause(Parser* p);
/**
 * arg_list> =>
 * <arg> <arg_next>
 * )
 */
Rule arg_list(Parser* p);
/**
 *<arg_next> =>
 * , <arg> <arg_next>
 * )
 */
Rule arg_next(Parser* p);
/**
 * <arg> =>
 * "ID" <optarg>
 * <literal>
 */
Rule arg(Parser* p);
/**
 * <param_list> =>
 * <param> <param_next>
 * )
 */
Rule param_list(Parser* p);
/**
 * <param_next> =>
 * , <param> <param_next>
 * )
 */
Rule param_next(Parser* p);
/**
 * <param> =>
 * "_" "ID" ":" <type>
 * "ID" "ID" ":" <type>
 */
Rule param(Parser* p);
/**
 * <blk_body> =>
 * <stmt> <blk_body>
 * }
 */
Rule block_body(Parser* p);
/**
 * <func_body> =>
 * <func_stmt> <func_body>
 * }
 */
Rule func_body(Parser* p);
/**
 * <func_stmt> =>
 *  var <def>
 *  let <def>
 *  ID <expression-type>
 *  while EXP { <func_body>
 *  if <cond_clause> { <func_body> else { <func_body>
 *  return <opt_ret>
 */
Rule func_stmt(Parser* p);
/**
 * <func_ret_type> =>
 *  ε
 *  -> <type>
 */
Rule func_ret_type(Parser* p);
/**
 * <opt_ret> ->
 *  EXP
 *  ε
 */
Rule opt_ret(Parser* p);
/**
 * <opt_type> ->
 *  : <type>
 *  ε
 */
Rule opt_type(Parser* p);
/**
 * <type> ->
 *  Int
 *  String
 *  Double
 */
Rule type(Parser* p);
/**
 * <opt_arg> ->
 *  : <term>
 *  ε
 */
Rule opt_arg(Parser* p);
/**
 * <term> ->
 *  ID
 *  <literal>
 */
Rule term(Parser* p);
/**
 * <literal> ->
 *  INT_LITERAL
 *  STR_LITERAL
 *  DOUBLE_LITERAL
 */
Rule literal(Parser* p);
/**
 * Parse only function headers
 * Used only in the first pass through token buffer in order to load
 * function definition into global symtable before calling any of them
 */
Rule func_header(Parser* p);
/**
 * Substitute for 'type' Rule
 * Used in the second run through token buffer in order to avoid
 * redefining function parameter type or return type
 */
Rule type_skip(Parser* p);
/**
 * Substitute for 'param' Rule
 * Used in the second run through token buffer in order to avoid
 * redefining function parameters
 */
Rule param_skip(Parser* p);
/**
 * Substitute for 'param_next' Rule
 * Used in the second run through token buffer
 */
Rule param_next_skip(Parser* p);

/**
 * Substitute for 'param_list' Rule
 * Used in the second run through token buffer
 */
Rule param_list_skip(Parser* p);
/**
 * Substitute for 'func_ret_type' Rule
 * Used in the second run through token buffer
 */
Rule func_ret_type_skip(Parser* p);
/**
 * Skip rule used in the first run through token buffer
 * Enables only parsing of function headers or EOF token
 * Ignores all other tokens, which are parsed in the second run
 */
Rule skip(Parser* p);
/**
 * Parses function calls on the right side of assignments
 */
Rule funccall(Parser* p);
/**
 * Initializes the contents of Parser object
 * @return true on sucess \n
 * @return false on error
 */
bool parser_init(Parser* p);
/**
 * Frees all data allocated by the parser object
 */
void parser_dispose(Parser* p);
/**
 * Fills global symtable with builtin functions before parsing
 * @return true
 * @return false
 */
bool add_builtins(Parser* p);
/**
 * Fills token buffer with tokens loaded by lexical analyzer
 * @return Relevant error code (uint32)
 */
uint32_t parser_fill_buffer(Parser* p);
/**
 * Begins the first run through token buffer which parses function headers
 */
uint32_t parser_get_func_decls(Parser* p);

/**
 * Main parser function
 * @return Relevant error code (uint32)
 */
uint32_t parse();

#endif