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
#include "debug.h"

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
    case TOKEN_INT:
    case TOKEN_DBL:
    case TOKEN_STRING:
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

void symbol_arr_init(symbol_arr_t *new_arr)
{
    new_arr->arr = NULL;
    new_arr->size = 0;
}

bool symbol_arr_append(symbol_arr_t *sym_arr, symstack_data_t data)
{
    if (sym_arr->size == 0)
    {
        // allocate new array
        sym_arr->arr = (symstack_data_t *)malloc(sizeof(symstack_data_t));
        sym_arr->size = 1;
        if (sym_arr->arr == NULL)
        {
            sym_arr->size = 0;
            return false;
        }
        sym_arr->arr[0] = data;
    }
    else
    {
        // realloc array and add item
        sym_arr->size += 1;
        sym_arr->arr = realloc(sym_arr->arr, (sym_arr->size) * sizeof(symstack_data_t));
        if (sym_arr->arr == NULL)
        {
            sym_arr->size -= 1;
            return false;
        }
        sym_arr->arr[sym_arr->size - 1] = data;
    }
    return true;
}

void symbol_arr_reverse(symbol_arr_t *sym_arr)
{
    int start = 0;
    int end = sym_arr->size - 1;

    while (start < end)
    {
        // Swap elements at start and end
        symstack_data_t temp = sym_arr->arr[start];
        sym_arr->arr[start] = sym_arr->arr[end];
        sym_arr->arr[end] = temp;

        // Move the indices toward the center
        start++;
        end--;
    }
}

void symbol_arr_free(symbol_arr_t *sym_arr)
{
    for (int i = 0; i < sym_arr->size; i++)
    {
        delete_token(&sym_arr->arr[i].token);
    }

    free(sym_arr->arr);
    sym_arr->size = 0;
}

/* methods of operation */
void equal_shift(symstack_t *stack, token_T *token)
{
    DEBUG_PRINT("equal shift\n");
    symstack_data_t sym_data = convert_token_to_data(*token);
    symstack_push(stack, sym_data);
}

void shift(symstack_t *stack, token_T *token)
{
    DEBUG_PRINT("shift\n");
    node_t *peek = symstack_peek(stack);
    peek->data.isHandleBegin = true;
    symstack_data_t sym_data = convert_token_to_data(*token);
    symstack_push(stack, sym_data);
    print_stack(stack, 1);
}

prec_rule_t choose_operator_rule(symstack_data_t data)
{
    switch (data.token.type)
    {
    // arithmetic rules
    case TOKEN_ADD:
        return RULE_E_PLUS_E;
    case TOKEN_SUB:
        return RULE_E_MINUS_E;
    case TOKEN_MUL:
        return RULE_E_MUL_E;
    case TOKEN_DIV:
        return RULE_E_DIV_E;

    // relational rules
    case TOKEN_LT:
        return RULE_E_LT_E;
    case TOKEN_LEQ:
        return RULE_E_LEQ_E;
    case TOKEN_GT:
        return RULE_E_GT_E;
    case TOKEN_GEQ:
        return RULE_E_GEQ_E;
    case TOKEN_EQ:
        return RULE_E_EQ_E;
    case TOKEN_NIL_CHECK:
        return RULE_E_IS_NIL_E;
    default:
        return RULE_NO_RULE;
    }
}

prec_rule_t get_rule(symstack_t *stack)
{
    // get symbol and choose rule based on it
    symbol_arr_t *arr;
    symbol_arr_init(arr);
    DEBUG_PRINT("sym_arr_init\n");
    if (arr == NULL)
    {
        error_code_handler(ERR_INTERNAL);
        return RULE_NO_RULE;
    }

    // push all items until finds handle
    node_t *current_node = symstack_peek(stack);
    while (!current_node->data.isHandleBegin)
    {
        if (!symbol_arr_append(arr, current_node->data))
        {
            error_code_handler(ERR_INTERNAL);
            print_error(ERR_INTERNAL, "Fcking peace of shit\n");
            return RULE_NO_RULE;
        }
        DEBUG_PRINT("sym_arr_append\n");
        current_node = current_node->previous;
    }

    symbol_arr_reverse(arr);

    // giant switch here
    prec_rule_t rule = RULE_NO_RULE;
    switch (arr->size)
    {
    case 1:
        // question of function handling here
        if (arr->arr[0].token.type == TOKEN_IDENTIFIER)
        {
            rule = RULE_OPERAND;
            break;
        }
        rule = RULE_NO_RULE;
        break;
    case 2:
        // E!
        if (arr->arr[0].isTerminal && arr->arr[1].token.type == TOKEN_NOT_NIL)
        {
            rule = RULE_E_NOT_NIL;
            break;
        }
        else if (arr->arr[0].token.type == TOKEN_L_PAR && arr->arr[2].token.type == TOKEN_R_PAR)
        {
            rule = RULE_PARL_E_PARR;
            break;
        }
        rule = RULE_NO_RULE;
        break;
    case 3:
        // choose rule E_OP_E
        if (!arr->arr[0].isTerminal && !arr->arr[2].isTerminal)
        {
            rule = choose_operator_rule(arr->arr[1]);
            break;
        }
        rule = RULE_NO_RULE;
        break;
    default:
        rule = RULE_NO_RULE;
        break;
    }

    symbol_arr_free(arr);
    DEBUG_PRINT("sym_arr_free\n");
    return rule;
}

void reduce(symstack_t *stack)
{
    DEBUG_PRINT("reduce\n");
    node_t *closest_term = get_closest_terminal(stack);
    prec_rule_t rule = get_rule(stack);
    if (rule == RULE_NO_RULE)
    {
        reduce_error(stack);
        return;
    }
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
            DEBUG_PRINT("End of expression\n");
            symstack_dispose(&stack);
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
            reduce(&stack);
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
    delete_token(&token);

    // print_stack(&stack, 1);
    return error_code_handler(EXIT_SUCCESS);
}
