#ifndef EXPRESSION_H
#define EXPRESSION_H

// #include "parser.h"
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
    E_PLUS_E,   // E -> E + E
    E_MINUS_E,  // E -> E - E
    E_CONCAT_E, // E -> E.E
    E_MUL_E,    // E -> E * E
    E_DIV_E,    // E -> E / E

    // relational rules
    E_LT_E,  // E -> E < E
    E_LEQ_E, // E -> E <= E
    E_GT_E,  // E -> E > E
    E_GEQ_E, // E -> E >= E
    E_EQ_E,  // E -> E === E
    E_NEQ_E, // E -> E !== E

    PARL_E_PARR, // E -> (E)
    OPERAND,     // E -> id
    NO_RULE
} prec_rule_t;

typedef enum PREC_OPERATORS
{
    IDX_ADD_SUB_CONCAT = 0,
    IDX_MUL_DIV,
    IDX_REL_OP,
    IDX_REL_EQ,
    IDX_ID,
    IDX_DOLLAR_SYMBOL,
    IDX_PAR_L,
    IDX_PAR_R
} prec_table_idx_op_t;

// error_t precedence_analysis(PData_t *p_data);

#endif
