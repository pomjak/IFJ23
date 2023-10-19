/**
 * @file expression.c
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @brief Expression parsing
 * @date 2023-10-18
 */

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

int error_code_handler(int error_code)
{
    static int err = EXIT_SUCCESS;
    if (err == EXIT_SUCCESS)
    {
        err = error_code;
    }
    return err;
}

void push_initial_sym(symstack_t *stack)
{
    symstack_data_t data;
    data.isHandleBegin = false;
    data.isTerminal = true;

    token_T token;
    token.type = TOKEN_UNDEFINED;
    data.token = token;

    strcpy(data.symbol, convert_token_type_to_string(token));
    symstack_push(stack, data);
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

node_t *get_closest_terminal(symstack_t *stack)
{
    node_t *current_node = symstack_peek(stack);

    while (current_node != NULL)
    {
        if (current_node->data.isTerminal)
        {
            return current_node;
        }
        current_node = current_node->previous;
    }
    return current_node;
}

prec_table_operation_t get_prec_table_operation(symstack_t *stack, token_T token)
{
    node_t *closest_terminal = get_closest_terminal(stack);
    if (closest_terminal == NULL)
    {
        return X;
    }
    printf("[%d][%d]: \n", convert_term_to_index(closest_terminal->data), convert_token_to_index(token));
    prec_table_operation_t prec_op = prec_tab[convert_term_to_index(closest_terminal->data)][convert_token_to_index(token)];
    return prec_op;
}

symbol_arr_t *symbol_arr_init()
{
    symbol_arr_t *new_arr = (symbol_arr_t *)malloc(sizeof(symbol_arr_t));
    if (new_arr == NULL)
    {
        print_error(ERR_INTERNAL, "expression.c: symbol_arr_init: memory was not allocated.\n");
    }
    new_arr->size = 0;
    return new_arr;
}

bool symbol_arr_append(symbol_arr_t *arr, node_t node)
{
    if (arr->size = 0)
    {
        // allocate new array
    }
    else
    {
        // realloc array and add item
    }
    return true;
}

void symbol_arr_free(symbol_arr_t *sym_arr)
{
    // free(sym_arr);
    // sym_arr = NULL;
}

/* methods of operation */
void equal_shift(symstack_t *stack, token_T *token)
{
    // printf("equal_shift\n");
    symstack_data_t sym_data = convert_token_to_data(*token);
    symstack_push(stack, sym_data);
}

void shift(symstack_t *stack, token_T *token)
{
    node_t *peek = symstack_peek(stack);
    peek->data.isHandleBegin = true;
    symstack_data_t sym_data = convert_token_to_data(*token);
    symstack_push(stack, sym_data);
}

prec_rule_t get_rule(symstack_t *stack, int *error_code)
{
    // get symbol and choose rule based on it
    node_t *arr = symbol_arr_init();
    if (arr == NULL)
    {
        error_code_handler(ERR_INTERNAL);
        return RULE_NO_RULE;
    }

    // push all items until finds handle
    node_t *current_node = symstack_peek(stack);
    while (!current_node->data.isHandleBegin)
    {
        if (!symbol_arr_append(arr, *current_node))
        {
            error_code_handler(ERR_INTERNAL);
            print_error(ERR_INTERNAL, "Fcking peace of shit\n");
            return RULE_NO_RULE;
        }
        current_node = current_node->previous;
    }

    // giant switch here
    printf("GET RULE\n");
    symbol_arr_free(arr);
    return RULE_NO_RULE;
}

void reduce(symstack_t *stack, int *err_code)
{
    printf("reduce\n");
    node_t *closest_term = get_closest_terminal(stack);
    prec_rule_t rule = get_rule(stack, err_code);
    // reduce_by_rule(stack, rule);
}

void reduce_error(symstack_t *stack)
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
    /* error handling */
    int error_code = EXIT_SUCCESS;

    // init stack
    symstack_t stack;
    init_symstack(&stack);

    // push inital symbol
    push_initial_sym(&stack);

    // get next symbol a
    token_T token;
    get_token(&token);
    symstack_data_t sym_data = convert_token_to_data(token);

    print_stack(&stack, 1);
    // prepare sym_data to be pushed on stack
    int i = 0;
    do
    {
        // check when end -> if token is $ thats the end of expr
        if (convert_token_to_index(token) == INDEX_DOLLAR)
        {
            // do some actions in here, end etc
            break;
        }
        switch (get_prec_table_operation(&stack, token))
        {
        case E:
            equal_shift(&stack, &token);
            get_token(&token);
            break;
        case S:
            shift(&stack, &token);
            get_token(&token);
            break;
        case R:
            reduce(&stack, &error_code);
            break;
        case X:
            // print_error(ERR_SYNTAX, "Expresion error.\n");
            break;
        default:
            break;
        }

        // sym_data = convert_token_to_data(token);
        // symstack_push(&stack, sym_data);

        i += 1;
        if (i == 10)
        {
            // print_stack(&stack, 1);
            symstack_dispose(&stack);
        }

    } while (!symstack_is_empty(&stack));

    // print_stack(&stack, 1);
    return error_code;
}
