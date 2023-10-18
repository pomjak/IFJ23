#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "error.h"

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

// RO - Relational Operators , FC - Function Call
const prec_table_operation_t prec_tab[PREC_TABLE_SIZE][PREC_TABLE_SIZE] =
    {
        /*      | / * | + - | ?? | i | FC | RO | ( | ) | ! | $ | */
        /* / * */ {R, R, R, S, S, R, R, R, R, R},
        /* + - */ {S, R, R, S, S, R, S, R, R, R},
        /* ??  */ {S, S, S, S, S, S, S, R, R, R},
        /* i   */ {R, R, R, X, X, R, X, R, R, R},
        /* FC  */ {R, R, R, X, X, R, X, R, R, R},
        /* RO  */ {S, S, R, S, S, E, S, R, R, R},
        /* (   */ {S, S, S, S, S, S, S, S, X, X},
        /* )   */ {R, R, R, X, X, R, X, E, R, R},
        /* !   */ {X, X, X, X, X, X, X, X, X, X},
        /* $   */ {S, S, S, S, S, S, S, X, R, S}};

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
    E_PLUS_E,  // E -> E + E
    E_MINUS_E, // E -> E - E
    E_MUL_E,   // E -> E * E
    E_DIV_E,   // E -> E / E

    // relational rules
    E_LT_E,     // E -> E < E
    E_LEQ_E,    // E -> E <= E
    E_GT_E,     // E -> E > E
    E_GEQ_E,    // E -> E >= E
    E_EQ_E,     // E -> E == E
    E_NEQ_E,    // E -> E != E
    E_IS_NIL_E, // E -> E ?? E

    PARL_E_PARR, // E -> (E)
    OPERAND,     // E -> id
    FUNC_CALL,   // E -> id(params)
    NO_RULE
} prec_rule_t;

// error_t precedence_analysis(PData_t *p_data);

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
 * @brief Get the prec table operation object
 *
 * @return prec_table_operation_t
 */
prec_table_operation_t get_prec_table_operation(symstack_t *stack, token_T token);

/* methods of operation */
void equal_shift();
void shift();
void reduce();
void reduce_error();

/**
 * @brief returns error code
 *
 * @return int error code
 */
int expr();

#endif
