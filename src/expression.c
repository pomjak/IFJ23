#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "expression.h"
#include "lexical_analyzer.h"
#include "symstack.h"

/**
 * @brief Methods to write
 * 0) equal_shift()
 * 1) shift()
 * 2) reduce()
 * 3) reduce_error()
 */

/**
 * QUESTIONABLE:
 * 1) convert token to index function
 * |- if function call is necessary
 * |- if token assignment is necessary
 */

void push_initial_sym(symstack_t *stack)
{
    symstack_data_t data;
    data.isHandleBegin = false;
    data.isTerminal = true;

    token_T token;
    token.type = TOKEN_UNDEFINED;
    data.token = token;

    strcpy(data.symbol, convert_token_type_to_string(token));
}

prec_tab_index_t convert_token_to_index(token_T token)
{
    switch (token.type)
    {
    case TOKEN_MUL:
    case TOKEN_DIV:
        return INDEX_MUL_DIV;
    case TOKEN_ADD:
    case TOKEN_SUB:
        return INDEX_ADD_SUB;
    case TOKEN_NIL_CHECK:
        return INDEX_NILL_CHECK;
    case TOKEN_IDENTIFIER:
        // if next token == (
        // return INDEX_FUNC_CALL
        // maybe not necessary according to they are the same
        return INDEX_IDENTIFIER;
    // maybe not necessary according, depends on assingment support, dont think so
    case TOKEN_ASS:
    case TOKEN_EQ:
    case TOKEN_NEQ:
    case TOKEN_GT:
    case TOKEN_LT:
    case TOKEN_GEQ:
    case TOKEN_LEQ:
        return INDEX_RELATIONAL_OP;

    case TOKEN_L_PAR:
        return INDEX_LPAR;
    case TOKEN_R_PAR:
        return INDEX_RPAR;
    case TOKEN_NOT_NIL:
        return INDEX_NOT_NIL;
    default:
        return INDEX_DOLLAR;
    }
}

prec_tab_index_t convert_term_to_index(symstack_data_t data)
{
    return convert_token_to_index(data.token);
}

prec_table_operation_t get_prec_table_operation(symstack_t *stack, token_T token)
{
    symstack_data_t closest_terminal;

    node_t *current_node = symstack_peek(&stack);
    while (current_node != NULL)
    {
        if (current_node->data.isTerminal)
        {
            // convert term to index
            return prec_tab[convert_term_to_index(current_node->data)][convert_token_to_index(token)];
        }
        current_node = current_node->previous;
    }
}

/* methods of operation */
void equal_shift()
{
    printf("equal_shift\n");
}

void shift()
{
    printf("shift\n");
}

void reduce()
{
    printf("reduce\n");
}

void reduce_error()
{
    printf("reduce_error\n");
}

/**
 * a - current got token / symbol
 * b - closest terminal on stack top
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

    // push inital symbol
    push_initial_sym(&stack);

    // get next symbol a
    token_T token;
    get_token(&token);
    symstack_data_t sym_data = convert_token_to_data(token);

    // prepare sym_data to be pushed on stack
    do
    {
        switch (get_prec_table_operation(&stack, token))
        {
        case E:
            equal_shift();
            break;
        case S:
            shift();
            break;
        case R:
            reduce();
            break;
        case X:
            print_error(ERR_SYNTAX, "Expresion error.\n");
            break;
        default:
            break;
        }

    } while (false);

    return EXIT_SUCCESS;
}
