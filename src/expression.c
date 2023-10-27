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
        /* (   */ {S, S, S, S, S, S, S, E, X, X},
        /* )   */ {R, R, R, X, X, R, X, R, R, R},
        /* !   */ {X, X, X, X, X, X, X, X, X, X},
        /* $   */ {S, S, S, S, S, S, S, X, R, R}};

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

void symbol_arr_copy_exp_to_arr(symstack_t *stack, symbol_arr_t *sym_arr)
{
    if (sym_arr == NULL)
    {
        error_code_handler(ERR_INTERNAL);
    }

    node_t *current_node = symstack_peek(stack);
    while (!current_node->data.isHandleBegin)
    {
        if (!symbol_arr_append(sym_arr, current_node->data))
        {
            error_code_handler(ERR_INTERNAL);
            print_error(ERR_INTERNAL, "Could not append symbol to symbol array.\n");
        }
        DEBUG_PRINT("sym_arr_append\n");
        current_node = current_node->previous;
    }
}

void symbol_arr_move_expr_to_arr(symstack_t *stack, symbol_arr_t *sym_arr)
{
    symstack_data_t current_symbol = symstack_pop(stack);
    while (!current_symbol.isHandleBegin && !symstack_is_empty(stack) && sym_arr->size < MAX_EXPR_SIZE)
    {
        if (!symbol_arr_append(sym_arr, current_symbol))
        {
            print_error(ERR_INTERNAL, "Could not push symbol to symbol array.\n");
            error_code_handler(ERR_INTERNAL);
            symstack_dispose(stack);
            return;
        }
        current_symbol = symstack_pop(stack);
    }
    // remove handle and push it back
    current_symbol.isHandleBegin = false;
    symstack_push(stack, current_symbol);
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

void print_symbol_arr(symbol_arr_t *sym_arr)
{
    for (int i = 0; i < sym_arr->size; i++)
    {
        if (sym_arr->arr[i].token.type == TOKEN_IDENTIFIER || sym_arr->arr[i].token.type == TOKEN_STRING)
        {
            printf("sym_arr[%d]: %s\n", i, sym_arr->arr[i].token.value.string_val.str);
        }
        else
        {
            printf("sym_arr[%d]: %s\n", i, sym_arr->arr[i].symbol);
        }
    }
}

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

bool is_operand(symstack_data_t symbol)
{
    token_type_T symbol_type = symbol.token.type;
    if (symbol_type == TOKEN_IDENTIFIER || symbol_type == TOKEN_INT || symbol_type == TOKEN_DBL || symbol_type == TOKEN_STRING)
    {
        return true;
    }
    return false;
}

bool is_binary_operator(symstack_data_t symbol)
{
    switch (symbol.token.type)
    {
    // arithmetic
    case TOKEN_ADD:
    case TOKEN_SUB:
    case TOKEN_MUL:
    case TOKEN_DIV:

    // relational
    case TOKEN_ASS:
    case TOKEN_EQ:
    case TOKEN_NEQ:
    case TOKEN_GT:
    case TOKEN_LT:
    case TOKEN_GEQ:
    case TOKEN_LEQ:
    case TOKEN_NIL_CHECK:
        return true;
    default:
        break;
    }
    return false;
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
    prec_table_operation_t prec_op = prec_tab[convert_term_to_index(closest_terminal->data)][convert_token_to_index(token)];
    printf("[%d][%d]: %d\n", convert_term_to_index(closest_terminal->data), convert_token_to_index(token), prec_op);
    return prec_op;
}

/* methods of operation */
void equal_shift(symstack_t *stack, token_T *token)
{
    DEBUG_PRINT("equal shift\n");
    symstack_data_t sym_data = convert_token_to_data(*token);
    symstack_push(stack, sym_data);
    print_stack(stack, 1);
}

void shift(symstack_t *stack, token_T *token)
{
    DEBUG_PRINT("shift\n");
    node_t *peek = get_closest_terminal(stack);
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
    case TOKEN_NEQ:
        return RULE_E_NEQ_E;
    case TOKEN_NIL_CHECK:
        return RULE_E_IS_NIL_E;
    default:
        return RULE_NO_RULE;
    }
}

prec_rule_t get_rule(symbol_arr_t *sym_arr)
{
    prec_rule_t rule = RULE_NO_RULE;
    switch (sym_arr->size)
    {
    case 1:
        // question of function handling here
        if (is_operand(sym_arr->arr[0]))
        {
            // find id in symtable

            // if id.type == func {
            //  rule = RULE_FUNC_CALL
            //  check function call syntax
            // }

            // else RULE_OPERAND
            rule = RULE_OPERAND;
            break;
        }
        rule = RULE_NO_RULE;
        break;
    case 2:
        // E!
        if (sym_arr->arr[0].isTerminal && sym_arr->arr[1].token.type == TOKEN_NOT_NIL)
        {
            rule = RULE_E_NOT_NIL;
            break;
        }
        rule = RULE_NO_RULE;
        break;
    case 3:
        // choose rule E_OP_E
        if (!sym_arr->arr[0].isTerminal && !sym_arr->arr[2].isTerminal)
        {
            rule = choose_operator_rule(sym_arr->arr[1]);
            break;
        }
        else if (sym_arr->arr[0].token.type == TOKEN_L_PAR && sym_arr->arr[2].token.type == TOKEN_R_PAR)
        {
            rule = RULE_PARL_E_PARR;
            break;
        }

        rule = RULE_NO_RULE;
        break;
    default:
        rule = RULE_NO_RULE;
        break;
    }
    return rule;
}

void push_non_term_on_stack(symstack_t *stack, symstack_data_t *term)
{
    term->isTerminal = false;
    symstack_push(stack, *term);
}

void push_reduced_symbol_on_stack(symstack_t *stack, symbol_arr_t *sym_arr, prec_rule_t rule)
{
    // see expression types
    symstack_data_t expr_symbol;

    switch (rule)
    {
    // id reduction
    case RULE_OPERAND:
        // set to non-terminal
        push_non_term_on_stack(stack, &sym_arr->arr[0]);
        break;

    // unary operations
    // E!
    case RULE_E_NOT_NIL:
        // change type nilable to false
        push_non_term_on_stack(stack, &sym_arr->arr[0]);
        break;

    // binary operations
    case RULE_E_PLUS_E:
        expr_symbol = process_addition(sym_arr);
        push_non_term_on_stack(stack, &expr_symbol);
        return;
    case RULE_E_MINUS_E:
    case RULE_E_DIV_E:
    case RULE_E_MUL_E:

    // relational operations
    case RULE_E_LT_E:
    case RULE_E_LEQ_E:
    case RULE_E_GT_E:
    case RULE_E_GEQ_E:
    case RULE_E_EQ_E:
    case RULE_E_NEQ_E:
    case RULE_E_IS_NIL_E:
        printf("Not implemented.\n");
        break;
    default:
        break;
    }
}

void reduce(symstack_t *stack)
{
    // maybe load only 3 top symbols
    symbol_arr_t sym_arr;
    symbol_arr_init(&sym_arr);
    symbol_arr_move_expr_to_arr(stack, &sym_arr);
    symbol_arr_reverse(&sym_arr);

    // printf("expr to be reduced: \n");
    // print_symbol_arr(&sym_arr);

    prec_rule_t rule = get_rule(&sym_arr);

    if (rule == RULE_NO_RULE)
    {
        error_code_handler(ERR_SYNTAX);
        reduce_error(stack, &sym_arr);
        symbol_arr_free(&sym_arr);
        return;
    }

    push_reduced_symbol_on_stack(stack, &sym_arr, rule);
    symbol_arr_free(&sym_arr);
}

void reduce_error(symstack_t *stack, symbol_arr_t *sym_arr)
{
    error_code_handler(ERR_SYNTAX);
    print_stack(stack, 1);
    print_symbol_arr(sym_arr);

    printf("\t\t\tTOKEN TYPE: %d\n", sym_arr->arr[0].token.type);
    if (sym_arr->arr[0].isTerminal)
    {
        printf("\t\t\tTERMINAL %d\n", sym_arr->arr[0].token.type);
    }

    /*
    E (
    E E
    E )
    E operator E
    */
    symstack_data_t data;
    data.token.type = TOKEN_UNDEFINED;

    // if (is_operand(sym_arr->arr[0]) || sym_arr->arr[0].token.type == TOKEN_UNDEFINED)
    if (is_operand(sym_arr->arr[0]))
    {
        data.token.type = sym_arr->arr[0].token.type;
        // E )
        if (sym_arr->arr[1].token.type == TOKEN_R_PAR)
        {
            print_error(ERR_SYNTAX, "Missing left parenthesis.\n");
        }
        // E E
        else if (is_operand(sym_arr->arr[1]) || sym_arr->arr[0].token.type == TOKEN_UNDEFINED)
        {
            // if 2 operand types are not equal
            if (sym_arr->arr[0].token.type != sym_arr->arr[1].token.type)
            {
                // if both operands are not strings
                if (sym_arr->arr[0].token.type != TOKEN_STRING && sym_arr->arr[1].token.type != TOKEN_DT_STRING)
                {
                    // if one of them is double
                    if (sym_arr->arr[0].token.type == TOKEN_DBL || sym_arr->arr[1].token.type == TOKEN_DBL)
                    {
                        data.token.type = TOKEN_DBL;
                    }
                    else
                    {
                        data.token.type = TOKEN_INT;
                    }
                }
            }
            data.token.type = sym_arr->arr[1].token.type;
            print_error(ERR_SYNTAX, "Missing operator.\n");
        }
        // E (
        else if (sym_arr->arr[0].token.type == TOKEN_L_PAR)
        {
            print_error(ERR_SYNTAX, "Missing operator.\n");
        }
        else if (is_binary_operator(sym_arr->arr[1]))
        {
            print_error(ERR_SYNTAX, "Missing second operand.\n");
        }
    }
    // first (
    else if (sym_arr->arr[0].token.type == TOKEN_L_PAR)
    {
        // ( E
        // if (is_operand(sym_arr->arr[1]) || sym_arr->arr[0].token.type == TOKEN_UNDEFINED)
        if (is_operand(sym_arr->arr[1]))
        {
            data.token.type = sym_arr->arr[1].token.type;
            print_error(ERR_SYNTAX, "Missing right parenthesis.\n");
        }
        // ( )
        else if (sym_arr->arr[1].token.type == TOKEN_R_PAR)
        {
            print_error(ERR_SYNTAX, "Missing operand2.\n");
        }
        // ( TERM
        else
        {
            print_error(ERR_SYNTAX, "Unexpected terminal.\n");
        }
    }
    // else first TERM
    else
    {
        printf("operator types: %d %d\n", sym_arr->arr[0].token.type, sym_arr->arr[1].token.type);
        print_error(ERR_SYNTAX, "Missing operand3.\n");
    }

    data.isHandleBegin = false;
    data.isTerminal = false;
    strcpy(data.symbol, "E");
    push_non_term_on_stack(stack, &data);
}

void expr_error(symstack_t *stack)
{
    symbol_arr_t sym_arr;
    symbol_arr_init(&sym_arr);

    // here push it to handle
    symbol_arr_move_expr_to_arr(stack, &sym_arr);
    symbol_arr_reverse(&sym_arr);

    // reduce with error
    reduce_error(stack, &sym_arr);

    // free error
    symbol_arr_free(&sym_arr);
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
    bool is_end_of_expression = false;

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
    do
    {
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
            print_stack(&stack, 1);
            break;
        case X:
            // set up array
            sym_data = convert_token_to_data(token);
            symstack_push(&stack, sym_data);

            expr_error(&stack);

            get_token(&token);

            break;
        default:
            print_error(ERR_INTERNAL, "Unknown precedense table operation.\n");
            return ERR_INTERNAL;
        }
    } while (!((convert_term_to_index(get_closest_terminal(&stack)->data)) == INDEX_DOLLAR && (convert_token_to_index(token) == INDEX_DOLLAR)));
    if (!symstack_is_empty(&stack))
    {
        symstack_dispose(&stack);
    };
    delete_token(&token);

    return error_code_handler(EXIT_SUCCESS);
}

symstack_data_t process_addition(symbol_arr_t *sym_arr)
{
    token_T first_operand = sym_arr->arr[0].token;
    token_T second_operand = sym_arr->arr[2].token;

    symstack_data_t expr_symbol;

    // if adding same types
    if (first_operand.type == second_operand.type)
    {
        if (first_operand.type == TOKEN_INT || first_operand.type == TOKEN_DBL || first_operand.type == TOKEN_STRING)
        {
            expr_symbol.token.type = first_operand.type;
        }
        else
        {
            print_error(ERR_UNCOMPATIBILE_TYPE, "Addition of incompatibile types.\n");
            error_code_handler(ERR_UNCOMPATIBILE_TYPE);
            expr_symbol.token.type = TOKEN_UNDEFINED;
        }
        return expr_symbol;
    }

    printf("Not same operands.\n");
    // int + int = int
    // int + double = double
    // str + str2 = strstr2
    // else err

    // if tokens are not strings
    if (first_operand.type != TOKEN_STRING && second_operand.type != TOKEN_STRING)
    {

        if (first_operand.type == TOKEN_DBL || second_operand.type == TOKEN_DBL)
        {
            DEBUG_PRINT("Generate conversion \"int2double %s\"\n", );
            expr_symbol.token.type = TOKEN_DBL;
            return expr_symbol;
        }
        else if (first_operand.type == TOKEN_INT && second_operand.type == TOKEN_INT)
        {
            expr_symbol.token.type = TOKEN_INT;
            return expr_symbol;
        }

        print_error(ERR_UNCOMPATIBILE_TYPE, "Addition of incompatibile types.\n");
        error_code_handler(ERR_UNCOMPATIBILE_TYPE);
        DEBUG_PRINT("Generate ADDITION %s + %s\n", first_operand.value, first_operand.value);
    }
    else if (first_operand.type == TOKEN_STRING && second_operand.type == TOKEN_STRING)
    {
        DEBUG_PRINT("Generate conversion \"int2double %s\"\n", );
    }
    error_code_handler(ERR_UNCOMPATIBILE_TYPE);
    print_error(ERR_UNCOMPATIBILE_TYPE, "Addition of incompatibile types.\n");
    expr_symbol.token.type = TOKEN_UNDEFINED;
    return expr_symbol;
}
