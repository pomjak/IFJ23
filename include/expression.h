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

typedef enum SYMBOL_TYPE
{
    // arithmetic operator symbols
    SYM_PLUS,
    SYM_MINUS,
    SYM_MUL,
    SYM_DIV,
    SYM_CONCAT,

    // relational operator symbols
    SYM_LT,
    SYM_LEQ,
    SYM_GT,
    SYM_GEQ,
    SYM_EQ,
    SYM_NEQ,

    // operand symbols
    SYM_NULL,
    SYM_ID,
    SYM_INT,
    SYM_FLOAT,
    SYM_STRING,

    // parantheses
    SYM_PAR_L,
    SYM_PAR_R,
    SYM_DOLLAR,
    NON_TERMINAL,
    SYM_REDUCE_END
} prec_sym_type_t;

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

    RULE_PARL_E_PARR, // E -> (E)
    RULE_OPERAND,     // E -> id
    RULE_FUNC_CALL,   // E -> id(params)
    RULE_NO_RULE
} prec_rule_t;

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

/***********************
 * Dynamic symbol array *
 ***********************/

typedef struct SYMBOL_ARR
{
    node_t *arr;
    size_t size;
} symbol_arr_t;
/**
 * @brief initializes dynamic symbol array
 *
 * @return node_t* - pointer to array
 */
symbol_arr_t *symbol_arr_init();

/**
 * @brief add node to symbol array
 *
 * @param node
 * @return true - added succefully
 * @return false - node was not added to array
 */
bool symbol_arr_append(symbol_arr_t *arr, node_t node);

/**
 * @brief reverses symbol array
 *
 * @param arr
 */
void symbol_arr_reverse(symbol_arr_t *arr);

/**
 * @brief frees symbol arr
 *
 * @param sym_arr
 */
void symbol_arr_free(symbol_arr_t *sym_arr);

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
 * @brief Get the rule based of symbols on stack
 *
 * @param stack
 * @return prec_rule_t
 */
prec_rule_t get_rule(symstack_t *stack, int *error_code);

/**
 * @brief reduces symbol on stack by rule
 *
 * @param rule
 */
void reduce_by_rule(symstack_t *stack, prec_rule_t rule);

/**
 * @brief reduce symbols on stack
 *
 * @param stack
 */
void reduce(symstack_t *stack, int *err_code);

/**
 * @brief reduce expr to closest possible good state
 *
 * @param stack
 */
void reduce_error(symstack_t *stack);

/**
 * @brief processes expression
 *
 * @return int error code
 */
int expr();

#endif
