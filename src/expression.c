#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "expression.h"
#include "lexical_analyzer.h"
#include "symstack.h"

/**
 * @brief Methods to write
 * 1) shift()
 * 2) reduce()
 * 3) reduce_error()
 */

#define PREC_TABLE_SIZE 10
typedef enum PREC_TABLE_OPERATIONS
{
    S, // shift
    R, // reduce
    E, // equal
    X  // error
} prec_table_operations_t;

/**
 * @brief TODO:
 * 1) check and discuss precedence table
 * 2) prototpye function
 */

// RO - Relational Operators , FC - Function Call
prec_table_operations_t prec_tab[PREC_TABLE_SIZE][PREC_TABLE_SIZE] =
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

/**
 * a - current got token / symbol
 * a - closest terminal on stack top
 * repeat:
 *  case: prec_tab[b][a]:
 *      E: push(a) & get next symbol
 *      S: b.isHandle = true & push(a) & get next symbol
 *      R: scan stack: if( < y) is on peek and r: A -> y contain P
 *          then: switch <y to A & display rule on stdout
 *          else error
 *      X: error
 * until a = $ and b = $
 *
 * return idea: token after expr
 */

int expr()
{

    // init stack
    symstack_t stack;
    init_symstack(&stack);

    // symstack_data_t
    symstack_data_t sym_data;

    // prepare sym_data to be pushed on stack
    do
    {

    } while (false);

    return EXIT_SUCCESS;
}
