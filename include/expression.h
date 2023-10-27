/**
 * @file expression.h
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @brief Expression parsing structures, precedence table and functions declarations
 * @date 2023-10-18
 */

#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "error.h"
#include "lexical_analyzer.h"
#include "symstack.h"

#define PREC_TABLE_SIZE 10
#define MAX_EXPR_SIZE 3

typedef enum PREC_TABLE_OPERATIONS
{
    S, // shift
    R, // reduce
    E, // equal
    X  // error
} prec_table_operation_t;

/**
 * @brief TODO:
 * 1) check and discuss precedence table
 * 2) prototpye function
 */

typedef enum PREC_TAB_INDEX
{
    INDEX_MUL_DIV = 0,
    INDEX_ADD_SUB,
    INDEX_NILL_CHECK,
    INDEX_IDENTIFIER,
    INDEX_FUNCTION_CALL,
    INDEX_RELATIONAL_OP,
    INDEX_LPAR,
    INDEX_RPAR,
    INDEX_NOT_NIL,
    INDEX_DOLLAR
} prec_tab_index_t;

// precedense rules
typedef enum PREC_RULES
{
    // arithmetic rules
    RULE_E_PLUS_E,  // E -> E + E
    RULE_E_MINUS_E, // E -> E - E
    RULE_E_MUL_E,   // E -> E * E
    RULE_E_DIV_E,   // E -> E / E

    // relational rules
    RULE_E_LT_E,     // E -> E < E
    RULE_E_LEQ_E,    // E -> E <= E
    RULE_E_GT_E,     // E -> E > E
    RULE_E_GEQ_E,    // E -> E >= E
    RULE_E_EQ_E,     // E -> E == E
    RULE_E_NEQ_E,    // E -> E != E
    RULE_E_IS_NIL_E, // E -> E ?? E
    RULE_E_NOT_NIL,  // E -> E!

    RULE_PARL_E_PARR, // E -> (E)
    RULE_OPERAND,     // E -> id
    RULE_FUNC_CALL,   // E -> id(params)
    RULE_NO_RULE
} prec_rule_t;

/***********************
 * Dynamic symbol array *
 ***********************/

typedef struct SYMBOL_ARR
{
    symstack_data_t *arr;
    size_t size;
} symbol_arr_t;

/**
 * @brief initializes dynamic symbol array
 */
void symbol_arr_init(symbol_arr_t *new_arr);

/**
 * @brief add node to symbol array
 *
 * @param node
 * @return true - added succefully
 * @return false - node was not added to array
 */
bool symbol_arr_append(symbol_arr_t *sym_arr, symstack_data_t data);

/**
 * @brief copies symbols to array from stack until it finds handle.
 *
 * @param stack
 * @param sym_arr
 */
void symbol_arr_copy_exp_to_arr(symstack_t *stack, symbol_arr_t *sym_arr);

/**
 * @brief moves symbols to array from stack until it finds handle. Removes handle
 *
 * @param stack
 * @param sym_arr
 */
void symbol_arr_move_expr_to_arr(symstack_t *stack, symbol_arr_t *sym_arr);

/**
 * @brief reverses symbol array
 *
 * @param sym_arr
 */
void symbol_arr_reverse(symbol_arr_t *sym_arr);

/**
 * @brief frees symbol arr
 *
 * @param sym_arr
 */
void symbol_arr_free(symbol_arr_t *sym_arr);

/**
 * @brief prints symbol array
 *
 */
void print_symbol_arr(symbol_arr_t *sym_arr);

/***********************************
 * Expression supportive functions *
 **********************************/

/**
 * @brief sets first found error code, else nothing
 *
 * @return new error code
 */
int error_code_handler(int error_code);

/**
 * @brief pushes dollar on stack
 *
 * @param stack
 */
void push_initial_sym(symstack_t *stack);

/**
 * @brief checks if symbol is operand type (token type of INT STRING DOUBLE IDENTIFIER)
 *
 * @param symbol
 * @return true
 * @return false
 */
bool is_operand(symstack_data_t symbol);

/**
 * @brief checks if symbol is operator
 *
 * @param symbol
 * @return true
 * @return false
 */
bool is_binary_operator(symstack_data_t symbol);

/**
 * @brief convert token to precedence table index based on it's data
 *
 * @param token
 * @return prec_tab_index_t - index
 */
prec_tab_index_t convert_token_to_index(token_T token);

/**
 * @brief converts term to precedence table index based on it's data
 *
 * @param data
 * @return prec_tab_index_t - index
 */
prec_tab_index_t convert_term_to_index(symstack_data_t data);

/**
 * @brief Finds and returns the closest terminal from top of the stack
 *
 * @param stack
 * @return node_t* - if null, no treminal was found
 */
node_t *get_closest_terminal(symstack_t *stack);

/**
 * @brief Get the prec table operation object
 *
 * @return prec_table_operation_t
 */
prec_table_operation_t get_prec_table_operation(symstack_t *stack, token_T token);

/**
 * @brief pushes symbol on stack
 *
 * @param stack
 * @param token
 */
void equal_shift(symstack_t *stack, token_T *token);

/**
 * @brief operation shift - sets up handle and pushes symbol on stack
 *
 * @param stack
 * @param token
 */
void shift(symstack_t *stack, token_T *token);

/**
 * @brief choses rule based on given operator
 *
 * @param data
 */
prec_rule_t choose_operator_rule(symstack_data_t data);

/**
 * @brief Get the rule based of symbols on stack
 *
 * @param stack
 * @return prec_rule_t
 */
prec_rule_t get_rule(symbol_arr_t *sym_arr);

/**
 * @brief sets the isTerminal to false of chosen terminal
 *
 * @param term
 */
void push_non_term_on_stack(symstack_t *stack, symstack_data_t *term);

/**
 * @brief pushes result symbol of reduced expression
 *
 * @param stack
 */
void push_reduced_symbol_on_stack(symstack_t *stack, symbol_arr_t *sym_arr, prec_rule_t rule);

/**
 * @brief reduce symbols on stack
 *
 * @param sym_arr
 */
void reduce(symstack_t *stack);

/**
 * @brief report err in expression
 *
 * @param sym_arr
 */
void reduce_error(symstack_t *stack, symbol_arr_t *sym_arr);

/**
 * @brief manages expression error state
 *
 * @param stack
 */
void expr_error(symstack_t *stack);

/**
 * @brief processes expression
 *
 * @return int error code
 */
int expr();

/****************************
 * Reduction rule functions *
 ****************************/

symstack_data_t process_addition(symbol_arr_t *sym_arr);

#endif
