/**
 * @file expression.c
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @author Jakub Pomsar (xpomsa00@stud.fit.vutbr.cz)
 * @brief Expression parsing
 * @date 2023-11-18
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "expression.h"
#include "lexical_analyzer.h"
#include "symstack.h"
#include "error.h"
#include "debug.h"
#include "parser.h"
#include "symtable.h"
#include "scope.h"
#include "code_generator.h"

#define REPORT_ERROR(err_code ,...)     \
    error_code_handler(err_code);       \
    print_error(err_code,__VA_ARGS__);

#ifdef SHOW_STACK
#define PRINT_STACK(stack) print_stack(stack, 1);
#else
#define PRINT_STACK(stack)
#endif

#define EMPTY_TOKEN(eol)                \
    {                                   \
        eol, TOKEN_UNDEFINED, { false } \
    }

#define DEFINE_EXPR_SYMBOL                      \
    symstack_data_t expr_symbol;                \
    expr_symbol.is_terminal = false;            \
    expr_symbol.is_handleBegin = false;         \
    expr_symbol.expr_res.expr_type = undefined; \
    expr_symbol.expr_res.nilable = false;       \
    token_T empty = EMPTY_TOKEN(false);         \
    expr_symbol.token = empty;

bool is_multiline_expr = false;
bool is_all_literals = true;

static void set_is_multiline_expr(bool value) { is_multiline_expr = value; }

static bool get_is_multiline_expr() { return is_multiline_expr; }

static void set_is_all_literals(bool value) { is_all_literals = value; }

static bool get_is_all_literals() { return is_all_literals; }

/**
 * @brief precedence table 
 * 
 * Note:
 * i - identifier
 * RO - Relational Operators 
 */
const prec_table_operation_t prec_tab[PREC_TABLE_SIZE][PREC_TABLE_SIZE] =
    {
        /*      | / * | + - | ?? | i | RO | ( | ) | ! | $ | */
        /* / * */ {R, R, R, S, R, S, R, S, R},
        /* + - */ {S, R, R, S, R, S, R, S, R},
        /* ??  */ {S, S, S, S, S, S, R, S, R},
        /* i   */ {R, R, R, X, R, X, R, R, R},
        /* RO  */ {S, S, R, S, E, S, R, S, R},
        /* (   */ {S, S, S, S, S, S, E, S, X},
        /* )   */ {R, R, R, X, R, X, R, X, R},
        /* !   */ {R, R, R, R, R, R, R, R, R}, 
        /* $   */ {S, S, S, S, S, S, X, S, R}};

void symbol_arr_init(symbol_arr_t *new_arr)
{
    new_arr->arr = NULL;
    new_arr->size = 0;
}

bool symbol_arr_append(symbol_arr_t *sym_arr, symstack_data_t data)
{
    if(sym_arr == NULL)
    {
        REPORT_ERROR(ERR_INTERNAL, "Appending to null symbol array.");
        return false;
    }

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
        REPORT_ERROR(ERR_INTERNAL,"Cannot copy expression to symbol array.\n");
    }

    node_t *current_node = symstack_peek(stack);
    while (!current_node->data.is_handleBegin)
    {
        if (!symbol_arr_append(sym_arr, current_node->data))
        {
            REPORT_ERROR(ERR_INTERNAL,"Could not append symbol to symbol array.\n");
        }
        DEBUG_PRINT("sym_arr_append\n");
        current_node = current_node->previous;
    }
}

void symbol_arr_move_expr_to_arr(symstack_t *stack, symbol_arr_t *sym_arr)
{
    symstack_data_t current_symbol = symstack_pop(stack);
    while (!current_symbol.is_handleBegin && !symstack_is_empty(stack) && sym_arr->size < MAX_EXPR_SIZE)
    {
        if (!symbol_arr_append(sym_arr, current_symbol))
        {
            REPORT_ERROR(ERR_INTERNAL, "Could not push symbol to symbol array.\n");
            symstack_dispose(stack);
            return;
        }
        current_symbol = symstack_pop(stack);
    }
    // remove handle and push it back
    current_symbol.is_handleBegin = false;
    symstack_push(stack, current_symbol);
}

void symbol_arr_reverse(symbol_arr_t *sym_arr)
{
    int start = 0;
    int end = sym_arr->size - 1;

    while (start < end)
    {
        // swap elements at start and end
        symstack_data_t temp = sym_arr->arr[start];
        sym_arr->arr[start] = sym_arr->arr[end];
        sym_arr->arr[end] = temp;

        // move the indices toward the center
        start += 1;
        end -= 1;
    }
}

void symbol_arr_free(symbol_arr_t *sym_arr)
{
    if(sym_arr == NULL)
    {
        return;
    }

    if(sym_arr->arr != NULL)
    {
        free(sym_arr->arr);
    }
    sym_arr->size = 0;
}

void print_symbol_arr(symbol_arr_t *sym_arr)
{
    for (size_t i = 0; i < sym_arr->size; i++)
    {
        if (sym_arr->arr[i].token.type == TOKEN_IDENTIFIER || sym_arr->arr[i].token.type == TOKEN_STRING)
        {
            printf("sym_arr[%ld]: %s\n", i, sym_arr->arr[i].token.value.string_val.str);
        }
        else
        {
            printf("sym_arr[%ld]: %s\n", i, sym_arr->arr[i].symbol);
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
    data.is_handleBegin = false;
    data.is_terminal = true;

    token_T token;
    token.type = TOKEN_UNDEFINED;
    data.token = token;

    strcpy(data.symbol, convert_token_type_to_string(token));
    symstack_push(stack, data);
}

bool is_operand(symstack_data_t symbol)
{
    if(is_literal(symbol) || symbol.token.type == TOKEN_IDENTIFIER)
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

bool find_closest_eol(symstack_t *stack)
{
    node_t *current_node = stack->top;
    bool eol_found = false;

    while (current_node != NULL)
    {
        if (current_node->data.token.preceding_eol)
        {
            DEBUG_PRINT("PRECEDING EOL");
            eol_found = true;
            break;
        }
        current_node = current_node->previous;
    }

    return eol_found;
}

void reduce_to_eol(symstack_t *stack, Parser *p)
{
    node_t *current_node = symstack_peek(stack);
    while (current_node != NULL)
    {
        if (current_node->data.token.preceding_eol)
        {
            break;
        }
        tb_prev(&p->buffer);
        symstack_pop(stack);
        current_node = symstack_peek(stack);
    }
}

bool id_is_defined(token_T token, Parser *p)
{
    unsigned int error = EXIT_SUCCESS;
    if (token.type == TOKEN_IDENTIFIER)
    {
        // find in local scopes
        if (p->lhs_id != NULL && token.type == TOKEN_IDENTIFIER)
        {
            if (!p->lhs_id->is_var_initialized && !dstring_cmp(&token.value.string_val, &p->lhs_id->name))
            {
                p->current_id = search_scopes(p->stack->next, &token.value.string_val, &error);
            }
            else
            {
                p->current_id = search_scopes(p->stack, &token.value.string_val, &error);
            }
        }
        else
        {
            p->current_id = search_scopes(p->stack, &token.value.string_val, &error);
        }
        if (p->current_id)
        {
            // found and unitialized
            if (!p->current_id->is_var_initialized)
            {
                REPORT_ERROR(ERR_UNDEFINED_VARIABLE,"Expressions undefined variable.\n")
            }
            return true;
        }
        else
        {
            // find in global scopes
            p->current_id = symtable_search(&p->global_symtab, &token.value.string_val, &error);

            // found and unitialized
            if (p->current_id)
            {
                if (!p->current_id->is_var_initialized)
                {
                    REPORT_ERROR(ERR_UNDEFINED_VARIABLE,"Expressions undefined varibale.\n");
                }
                return true;
            }
            REPORT_ERROR(ERR_UNDEFINED_VARIABLE,"Expressions undefined varibale.\n");
            return false;
        }
    }
    return false;
}

bool is_literal(symstack_data_t symbol)
{
    token_type_T exp_type = symbol.token.type;
    return (exp_type == TOKEN_INT || exp_type == TOKEN_DBL || exp_type == TOKEN_STRING || exp_type == TOKEN_NIL);
}

bool is_identifier(symstack_data_t symbol)
{
    return !symbol.is_literal;
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
    case TOKEN_NIL:
        return INDEX_IDENTIFIER;
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
        if (current_node->data.is_terminal)
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
    DEBUG_PRINT("[%d][%d]: %d\n", convert_term_to_index(closest_terminal->data), convert_token_to_index(token), prec_op);
    return prec_op;
}

/* methods of operation */
void equal_shift(symstack_t *stack, token_T *token)
{
    DEBUG_PRINT("equal shift\n");
    symstack_data_t sym_data = convert_token_to_data(*token);
    symstack_push(stack, sym_data);
    PRINT_STACK(stack);
}

void shift(symstack_t *stack, token_T *token)
{
    DEBUG_PRINT("shift\n");
    node_t *peek = get_closest_terminal(stack);
    peek->data.is_handleBegin = true;
    symstack_data_t sym_data = convert_token_to_data(*token);
    symstack_push(stack, sym_data);
    PRINT_STACK(stack);
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
    if (sym_arr->arr == NULL)
    {
        return RULE_NO_RULE;
    }
    DEBUG_PRINT("\nCHOOSING RULE | size: %zu \n", sym_arr->size);

    DEBUG_PRINT("IS OPERAND ? : %d", is_operand(sym_arr->arr[0]));

    prec_rule_t rule = RULE_NO_RULE;
    switch (sym_arr->size)
    {
    case 1:
        if (is_operand(sym_arr->arr[0]))
        {
            rule = RULE_OPERAND;
            break;
        }
        rule = RULE_NO_RULE;
        break;
    case 2:
        // E!
        if (!sym_arr->arr[0].is_terminal && sym_arr->arr[1].token.type == TOKEN_NOT_NIL)
        {
            rule = RULE_E_NOT_NIL;
            break;
        }
        rule = RULE_NO_RULE;
        break;
    case 3:
        // choose rule E_OP_E
        if (!sym_arr->arr[0].is_terminal && !sym_arr->arr[2].is_terminal)
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

void push_reduced_symbol_on_stack(symstack_t *stack, symbol_arr_t *sym_arr, prec_rule_t rule, Parser *p)
{
    DEFINE_EXPR_SYMBOL;

    switch (rule)
    {
    // id reduction
    case RULE_OPERAND:
        // set to non-terminal
        expr_symbol = process_operand(&sym_arr->arr[0], p);
        DEBUG_PRINT("\t EXPR_SYM expr_t   : %d\n", expr_symbol.expr_res.expr_type);
        DEBUG_PRINT("\t EXPR_SYM isterm   : %d\n", expr_symbol.is_terminal);
        DEBUG_PRINT("\t EXPR_SYM ishandle : %d\n", expr_symbol.is_handleBegin);
        DEBUG_PRINT("\t EXPR_SYM isliteral : %d\n", expr_symbol.is_literal);
        symstack_push(stack, expr_symbol);
        break;

    // unary operations
    // E!
    case RULE_E_NOT_NIL:
        // change type nilable to false
        expr_symbol.token = sym_arr->arr[0].token;

        // if token E is not operand
        if(!sym_arr->arr[0].is_identifier)
        {
            REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Cannot by applied to more than 1 operand.\n");
        }

        expr_symbol.expr_res.expr_type = sym_arr->arr[0].expr_res.expr_type;
        if(sym_arr->arr[0].expr_res.nilable)
        {
            expr_symbol.expr_res.nilable = false;
        }
        else
        {
            expr_symbol.expr_res.expr_type = undefined;
            expr_symbol.expr_res.nilable = false;
            REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Cannot force unwrap non-nilable variable.\n");
        }
        DEBUG_PRINT("\t EXPR_SYM expr_t   : %d\n", expr_symbol.expr_res.expr_type);
        DEBUG_PRINT("\t EXPR_SYM isterm   : %d\n", expr_symbol.is_terminal);
        DEBUG_PRINT("\t EXPR_SYM ishandle : %d\n", expr_symbol.is_handleBegin);
        DEBUG_PRINT("\t EXPR_SYM isliteral : %d\n", expr_symbol.is_literal);
        DEBUG_PRINT("\t EXPR_SYM isnilable : %d\n", expr_symbol.expr_res.nilable);
        symstack_push(stack, expr_symbol);
        break;

    // binary operations
    case RULE_E_PLUS_E:
    case RULE_E_MINUS_E:
    case RULE_E_MUL_E:
    case RULE_E_DIV_E:
        expr_symbol = process_arithmetic_operation(sym_arr);
        DEBUG_PRINT("\t EXPR_SYM expr_t   : %d\n", expr_symbol.expr_res.expr_type);
        DEBUG_PRINT("\t EXPR_SYM isterm   : %d\n", expr_symbol.is_terminal);
        DEBUG_PRINT("\t EXPR_SYM ishandle : %d\n", expr_symbol.is_handleBegin);
        DEBUG_PRINT("\t EXPR_SYM isliteral : %d\n", expr_symbol.is_literal);
        symstack_push(stack, expr_symbol);
        return;

    // relational operations
    case RULE_E_LT_E:
    case RULE_E_LEQ_E:
    case RULE_E_GT_E:
    case RULE_E_GEQ_E:
    case RULE_E_EQ_E:
    case RULE_E_NEQ_E:
    case RULE_E_IS_NIL_E:
        expr_symbol = process_relational_operation(sym_arr,p);
        DEBUG_PRINT("\t EXPR_SYM expr_t   : %d\n", expr_symbol.expr_res.expr_type);
        DEBUG_PRINT("\t EXPR_SYM isterm   : %d\n", expr_symbol.is_terminal);
        DEBUG_PRINT("\t EXPR_SYM ishandle : %d\n", expr_symbol.is_handleBegin);
        DEBUG_PRINT("\t EXPR_SYM isliteral : %d\n", expr_symbol.is_literal);
        symstack_push(stack, expr_symbol);
        return;
    // parenthesis rule
    case RULE_PARL_E_PARR:
        expr_symbol = process_parenthesis(sym_arr);
        DEBUG_PRINT("\t EXPR_SYM expr_t   : %d\n", expr_symbol.expr_res.expr_type);
        DEBUG_PRINT("\t EXPR_SYM isterm   : %d\n", expr_symbol.is_terminal);
        DEBUG_PRINT("\t EXPR_SYM ishandle : %d\n", expr_symbol.is_handleBegin);
        DEBUG_PRINT("\t EXPR_SYM isliteral : %d\n", expr_symbol.is_literal);
        symstack_push(stack, expr_symbol);
        return;
    default:
        break;
    }
}

void reduce(symstack_t *stack, Parser *p)
{
    symbol_arr_t sym_arr;
    symbol_arr_init(&sym_arr);
    symbol_arr_move_expr_to_arr(stack, &sym_arr);
    symbol_arr_reverse(&sym_arr);

    prec_rule_t rule = get_rule(&sym_arr);
    DEBUG_PRINT("RULE %d\n", rule);
    if (rule == RULE_NO_RULE)
    {

        if (!find_closest_eol(stack))
        {
            reduce_error(stack, &sym_arr);
            error_code_handler(ERR_SYNTAX);
            symbol_arr_free(&sym_arr);
        }
        else
        {
            DEBUG_PRINT("NO ERR");
            reduce_to_eol(stack, p);
            set_is_multiline_expr(true);
            tb_prev(&p->buffer);
            PRINT_STACK(stack);

            // set the end of the expression
            token_T empty = EMPTY_TOKEN(p->curr_tok.preceding_eol);
            p->curr_tok = empty;
        }
        symbol_arr_free(&sym_arr);
        return;
    }

    push_reduced_symbol_on_stack(stack, &sym_arr, rule, p);
    printf("push reduced symbol on stack\n");
    PRINT_STACK(stack);
    symbol_arr_free(&sym_arr);
}

void reduce_error(symstack_t *stack, symbol_arr_t *sym_arr)
{
    DEBUG_PRINT("Reduce error");

    /*
    Error states: 
    E (
    E E
    E )
    E operator E
    */
    symstack_data_t data;
    data.token.type = TOKEN_UNDEFINED;

    if (sym_arr != NULL && sym_arr->arr != NULL)
    {
        if (is_operand(sym_arr->arr[0]))
        {

            data.token.type = sym_arr->arr[0].token.type;
            // E )
            if (sym_arr->arr[1].token.type == TOKEN_R_PAR)
            {
                REPORT_ERROR(ERR_SYNTAX,"Missing left parenthesis.\n");
            }
            // E E
            else if (is_operand(sym_arr->arr[1]))
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
                REPORT_ERROR(ERR_SYNTAX,"Missing operator.\n");
            }
            // E (
            else if (sym_arr->arr[1].token.type == TOKEN_L_PAR)
            {
                REPORT_ERROR(ERR_SYNTAX,"Missing operator.\n");
            }
            else if (is_binary_operator(sym_arr->arr[1]))
            {
                REPORT_ERROR(ERR_SYNTAX,"Missing second operand.\n");
            }
        }
        // first (
        else if (sym_arr->arr[0].token.type == TOKEN_L_PAR)
        {
            // ( E
            if (is_operand(sym_arr->arr[1]))
            {
                data.token.type = sym_arr->arr[1].token.type;
                REPORT_ERROR(ERR_SYNTAX,"Missing right parenthesis.\n");
            }
            // ( )
            else if (sym_arr->arr[1].token.type == TOKEN_R_PAR)
            {
                REPORT_ERROR(ERR_SYNTAX,"Missing operand2.\n");
            }
            // ( TERM
            else
            {
                REPORT_ERROR(ERR_SYNTAX, "Unexpected terminal.\n")
            }
        }
        // else first TERM
        else
        {
            REPORT_ERROR(ERR_SYNTAX,"Missing operand3.\n");
        }
    }

    data.is_handleBegin = false;
    data.is_terminal = false;
    data.is_identifier = false;
    data.is_literal = false;
    data.expr_res.expr_type = undefined;
    data.expr_res.nilable = false;
    token_T empty = EMPTY_TOKEN(false);
    data.token = empty;
    strcpy(data.symbol, "ERR");
    symstack_push(stack, data);
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

Type convert_to_expr_type(token_type_T type)
{
    switch (type)
    {
    case TOKEN_INT:
        return integer;
    case TOKEN_DBL:
        return double_;
    case TOKEN_STRING:
        return string;
    case TOKEN_NIL:
        return nil;
    default:
        return undefined;
    }
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
int expr(Parser *p)
{
    DEBUG_PRINT("EXPR\n");

    symstack_t stack;
    init_symstack(&stack);

    push_initial_sym(&stack);

    GET_TOKEN();

    symstack_data_t sym_data = convert_token_to_data(p->curr_tok);

    do
    {
        switch (get_prec_table_operation(&stack, p->curr_tok))
        {
        case E:
            equal_shift(&stack, &p->curr_tok);
            GET_TOKEN();

            break;
        case S:
            shift(&stack, &p->curr_tok);
            GET_TOKEN();
            break;
        case R:
            reduce(&stack, p);
            PRINT_STACK(&stack);
            break;
        case X:
            sym_data = convert_token_to_data(p->curr_tok);
            symstack_push(&stack, sym_data);

            if (find_closest_eol(&stack))
            {
                reduce_to_eol(&stack, p);
                symstack_pop(&stack);
                set_is_multiline_expr(true);
                tb_prev(&p->buffer);

                PRINT_STACK(&stack);

                // set the end of the expression
                token_T empty = EMPTY_TOKEN(false);
                p->curr_tok = empty;
            }
            else
            {
                expr_error(&stack);
                GET_TOKEN();
            }
            break;
        default:
            REPORT_ERROR(ERR_INTERNAL,"Unknown precedence table operation.\n");
            return ERR_INTERNAL;
        }
    } while (!((convert_term_to_index(get_closest_terminal(&stack)->data) == INDEX_DOLLAR) && (convert_token_to_index(p->curr_tok) == INDEX_DOLLAR)));

    symstack_data_t final_expr = symstack_pop(&stack);
    if (!symstack_is_empty(&stack))
    {
        symstack_dispose(&stack);
    }
    else
    {
        REPORT_ERROR(ERR_SYNTAX,"Missing expression.\n");
    }

    DEBUG_PRINT("before token type: %d", p->curr_tok.type);
    DEBUG_PRINT("before expr type: %d", final_expr.expr_res.expr_type);

    if (get_is_multiline_expr())
    {
        set_is_multiline_expr(false);
        GET_TOKEN();
    }

    verify_lhs_type(&final_expr, p);

    DEBUG_PRINT("recieved token type: %d", p->curr_tok.type);
    DEBUG_PRINT("final expr type: %d", final_expr.expr_res.expr_type);

    p->expr_res = final_expr.expr_res;
    DEBUG_PRINT("expr_type : %d | expr is nilable: %d\n", p->expr_res.expr_type, p->expr_res.nilable);

    return error_code_handler(EXIT_SUCCESS);
}

symstack_data_t process_operand(symstack_data_t *operand, Parser *p)
{
    DEBUG_PRINT("Process operand");
    DEFINE_EXPR_SYMBOL;
    expr_symbol.token = operand->token;
    expr_symbol.is_literal = is_literal(*operand);
    expr_symbol.is_identifier = (operand->token.type == TOKEN_IDENTIFIER);

    // get type of the expression
    if (operand->token.type == TOKEN_IDENTIFIER)
    {
        set_is_all_literals(false);
        // find the operand
        if (id_is_defined(operand->token, p))
        {
            expr_symbol.expr_res.expr_type = p->current_id->type;
            expr_symbol.expr_res.nilable = p->current_id->is_nillable;
        }
    }
    else if (is_operand(*operand))
    {
        expr_symbol.expr_res.expr_type = convert_to_expr_type(operand->token.type);
    }
    code_generator_push(operand->token);
    return expr_symbol;
}

symstack_data_t process_arithmetic_operation(symbol_arr_t *sym_arr)
{
    DEBUG_PRINT("Process arithmetic op");

    symstack_data_t first_operand = sym_arr->arr[0];
    token_T op = sym_arr->arr[1].token;
    symstack_data_t second_operand = sym_arr->arr[2];

    DEFINE_EXPR_SYMBOL;
    expr_symbol.is_literal = first_operand.is_literal && second_operand.is_literal;
    expr_symbol.is_identifier = false;

    // check types of operands
    if (!compare_types_strict(&first_operand, &second_operand) && (first_operand.is_literal || second_operand.is_literal))
    {
        convert_if_retypeable(&first_operand, &second_operand);
    }

    // if they are identifiers and both are same strict type
    if (is_identifier(first_operand) && is_identifier(second_operand))
    {
        if (!compare_types_strict(&first_operand, &second_operand))
        {
            REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Incompatibile types of operands.\n");
            return expr_symbol;
        }
    }

    // if operands are nillable
    if (operand_is_nilable(&first_operand) || operand_is_nilable(&second_operand))
    {
        REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Incompatibile types of operands.\n");
        return expr_symbol;
    }
    

    if (compare_operand_with_type(&first_operand, string) || compare_operand_with_type(&second_operand, string))
    {
        expr_symbol = process_concatenation(sym_arr);
        return expr_symbol;
    }

    if (op.type == TOKEN_DIV)
    {
        expr_symbol = process_division(&first_operand, &second_operand);
        return expr_symbol;
    }

    // if adding same types
    if (compare_types_strict(&first_operand, &second_operand))
    {
        if (compare_operand_with_type(&first_operand, integer) || compare_operand_with_type(&first_operand, double_))
        {
            expr_symbol.expr_res.expr_type = first_operand.expr_res.expr_type;
        }
        else
        {
            REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Addition of incompatibile types.\n");
            return expr_symbol;
        }
    }

    // set expr symbol type based on operands type
    if (compare_operand_with_type(&first_operand, double_) || compare_operand_with_type(&second_operand, double_))
    {
        expr_symbol.expr_res.expr_type = double_;
    }
    else if (first_operand.expr_res.expr_type == integer && compare_types_strict(&first_operand, &second_operand))
    {
        expr_symbol.expr_res.expr_type = integer;
    }
    code_generator_operations(op.type, compare_operand_with_type(&expr_symbol,integer));

    return expr_symbol;

    REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Addition of incompatibile types.\n");

    expr_symbol.expr_res.expr_type = undefined;
    return expr_symbol;
}

symstack_data_t process_division(symstack_data_t * first_operand, symstack_data_t * second_operand)
{
    DEBUG_PRINT("Process division");
    DEFINE_EXPR_SYMBOL;
    expr_symbol.is_identifier = false;
    expr_symbol.expr_res.expr_type = (first_operand->expr_res.expr_type == integer ? integer : double_);

    expr_symbol.is_literal = first_operand->is_literal && second_operand->is_literal;

    if (!compare_types_strict(first_operand, second_operand))
    {
        REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Division of incompatibile types.\n");
        return expr_symbol;
    }

    if (compare_operand_with_type(second_operand, integer))
    {
        if (second_operand->token.value.int_val == 0)
        {
            REPORT_ERROR(ERR_SEMANTIC,"Division by zero.\n");
            return expr_symbol;
        }
    }
    else if (compare_operand_with_type(second_operand, double_))
    {
        if (second_operand->token.value.double_val == 0.0)
        {
            REPORT_ERROR(ERR_SEMANTIC,"Division by zero.\n");
            return expr_symbol;
        }
    }
    code_generator_operations(TOKEN_DIV, compare_operand_with_type(&expr_symbol,integer));
    return expr_symbol;
}

symstack_data_t process_concatenation(symbol_arr_t *sym_arr)
{
    DEBUG_PRINT("Process concat");
    DEFINE_EXPR_SYMBOL;
    expr_symbol.is_identifier = false;

    expr_symbol.expr_res.expr_type = string;

    symstack_data_t first_operand = sym_arr->arr[0];
    token_T operator= sym_arr->arr[1].token;
    symstack_data_t second_operand = sym_arr->arr[2];

    expr_symbol.is_literal = first_operand.is_literal && first_operand.is_literal;

    if (operator.type != TOKEN_ADD)
    {
        REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Unknown string operation.\n");
        return expr_symbol;
    }

    if (compare_types_strict(&first_operand, &second_operand))
    {
        DEBUG_PRINT("Generate concatenation\n");
        code_generator_concats();
        return expr_symbol;
    }
    REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Concatenation with uncompatibile types.\n");
    return expr_symbol;
}

symstack_data_t process_relational_operation(symbol_arr_t *sym_arr, Parser *p)
{
    DEBUG_PRINT("Process relational op");
    DEFINE_EXPR_SYMBOL;
    expr_symbol.expr_res.expr_type = bool_;
    expr_symbol.is_identifier = false;

    // define operands
    symstack_data_t first_operand = sym_arr->arr[0];
    token_T op = sym_arr->arr[1].token;
    symstack_data_t second_operand = sym_arr->arr[2];
    expr_symbol.is_literal = first_operand.is_literal && second_operand.is_literal;

    // process rel expression with nil
    bool first_is_nil = compare_operand_with_type(&first_operand, nil);
    bool second_is_nil = compare_operand_with_type(&second_operand, nil);

    // if there is nil operand
    if(op.type != TOKEN_NIL_CHECK && (first_is_nil || second_is_nil))
    {
        if(first_is_nil)
        {
            if(!operand_is_nilable(&second_operand))
            {
                REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Incompatibile types to compare.\n");
                return expr_symbol;
            }
        }
        else
        {
            if(!operand_is_nilable(&first_operand))
            {
                REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Incompatibile types to compare.\n");
                return expr_symbol;
            }
        }
    }
    else // if there is not nil operand
    {
        if (!compare_types_strict(&first_operand, &second_operand))
        {
            if (op.type != TOKEN_NIL_CHECK && (first_operand.is_literal || second_operand.is_literal))
            {
                convert_if_retypeable(&first_operand, &second_operand);
            }
            else if(op.type != TOKEN_NIL_CHECK)
            {
                REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Incompatibile types to compare.\n");
                return expr_symbol;
            }
        }
    }

    // retype nill check
    if (op.type == TOKEN_NIL_CHECK)
    {
        // check if not same types
        if (first_operand.expr_res.expr_type != nil && (first_operand.expr_res.expr_type != second_operand.expr_res.expr_type))
        {
            REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Incompatibile types in nil check.\n");
            return expr_symbol;
        }

        // check if not (type? ?? type)
        if (first_operand.expr_res.expr_type != nil && (!first_operand.expr_res.nilable || second_operand.expr_res.nilable))
        {
            REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Incompatibile types in nil check.\n");
            return expr_symbol;
        }

        // set final type
        if(compare_operand_with_type(&first_operand,nil))
        {
            expr_symbol.expr_res.expr_type = second_operand.expr_res.expr_type;
        }
        else
        {
            expr_symbol.expr_res.expr_type = first_operand.expr_res.expr_type;
        }

        // ?? generation
        code_generator_nil_check(p->cond_uid);
        p->cond_uid += 1;

        expr_symbol.expr_res.nilable = false;
        return expr_symbol;
    }
    code_generator_operations(op.type,false);
    return expr_symbol;
}

symstack_data_t process_parenthesis(symbol_arr_t *sym_arr)
{
    DEFINE_EXPR_SYMBOL;
    expr_symbol.is_identifier = false;
    expr_symbol.expr_res.expr_type = sym_arr->arr[1].expr_res.expr_type;
    expr_symbol.is_literal = sym_arr->arr[1].is_literal;
    return expr_symbol;
}

/* TYPE RELATED FUNCTIONS */
bool compare_types_strict(symstack_data_t *operand1, symstack_data_t *operand2)
{
    bool core_type = (operand1->expr_res.expr_type == operand2->expr_res.expr_type);
    bool nilable_type = (operand1->expr_res.nilable == operand2->expr_res.nilable);
    DEBUG_PRINT("Core types     : %d == %d\n", operand1->expr_res.expr_type, operand2->expr_res.expr_type);
    DEBUG_PRINT("Nilable types  : %d == %d\n", operand1->expr_res.nilable, operand2->expr_res.nilable);
    return (core_type && nilable_type);
}

bool compare_operand_with_type(symstack_data_t *operand, Type type)
{
    bool core_type = (operand->expr_res.expr_type == type);
    return core_type;
}

bool operand_is_nilable(symstack_data_t *operand)
{
    return operand->expr_res.nilable;
}

void verify_lhs_type(symstack_data_t *final_expr, Parser *p)
{
    // if lhs is defined and all processed operands were literals
    if (p->lhs_id != NULL && get_is_all_literals())
    {
        // check core types
        if (p->lhs_id->type != final_expr->expr_res.expr_type)
        {
            // find if type conversion exists
            if (p->lhs_id->type == double_ && final_expr->expr_res.expr_type == integer)
            {
                // generate code tu push int2char expr on stack
                code_generator_int2doubles(0);
                final_expr->expr_res.expr_type = double_;
            }
        }
    }
    set_is_all_literals(true);
}

void convert_if_retypeable(symstack_data_t *operand1, symstack_data_t *operand2)
{
    bool first_is_retypeable = (compare_operand_with_type(operand1, integer) || compare_operand_with_type(operand1, double_)) && (operand1->expr_res.nilable == false);
    bool second_is_retypeable = (compare_operand_with_type(operand2, integer) || compare_operand_with_type(operand2, double_)) && (operand2->expr_res.nilable == false);

    // if they are both retypeable
    if (first_is_retypeable && second_is_retypeable)
    {
        if (operand1->is_literal && compare_operand_with_type(operand1, integer))
        {
            DEBUG_PRINT("retyping FIRST from int to double");

            // convert first operand on stack to double - second from top on stack
            code_generator_int2doubles(1);
            operand1->expr_res.expr_type = double_;
        }
        else if (operand2->is_literal && compare_operand_with_type(operand2, integer))
        {
            DEBUG_PRINT("retyping SECOND from int to double");
            // convert second operand on stack to double - on the stack top
            code_generator_int2doubles(0);
            operand2->expr_res.expr_type = double_;
        }
        else
        {
            REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Implicit conversion is not supported in this case.\n");
        }
    }
    else
    {
        REPORT_ERROR(ERR_INCOMPATIBILE_TYPE,"Implicit conversion is not supported in this case.\n");
    }
}
