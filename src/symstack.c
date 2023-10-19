/**
 * @file symstack.c
 * @author Adrián Ponechal (xponec01@stud.fit.vutbr.cz)
 * @brief Stack for symbols used in precedence analysis
 * @date 2023-10-15
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symstack.h"

#define MIN_WIDTH 10

void delete_token(token_T *token)
{
    if (token->type == TOKEN_STRING || token->type == TOKEN_IDENTIFIER)
    {
        dstring_free(&token->value.string_val);
    }
}

symstack_data_t convert_token_to_data(token_T token)
{
    symstack_data_t data;
    data.token = token;
    data.isHandleBegin = false;
    data.isTerminal = true;
    strcpy(data.symbol, convert_token_type_to_string(token));
    return data;
}

void init_symstack(symstack_t *stack)
{
    stack->top = NULL;
    stack->size = 0;
}

bool symstack_push(symstack_t *stack, symstack_data_t data)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL)
    {
        return false;
    }

    new_node->data = data;
    new_node->previous = stack->top;
    stack->top = new_node;
    stack->size += 1;
    return true;
}

symstack_data_t symstack_pop(symstack_t *stack)
{
    symstack_data_t old_node_data;
    if (!symstack_is_empty(stack))
    {
        node_t *old_node = stack->top;
        old_node_data = old_node->data;

        stack->top = old_node->previous;
        stack->size -= 1;
        free(old_node);
        return old_node_data;
    }
    return old_node_data;
}

node_t *symstack_peek(const symstack_t *stack)
{
    return stack->top;
}

bool symstack_is_empty(const symstack_t *stack)
{
    return stack->size == 0 && stack->top == NULL;
}

bool symstack_dispose(symstack_t *stack)
{
    while (!symstack_is_empty(stack))
    {
        symstack_data_t data = symstack_pop(stack);
        delete_token(&data.token);
    }
    return symstack_is_empty(stack);
}

const char *convert_token_type_to_string(token_T token)
{
    switch (token.type)
    {
        /* arithmetic tokens */
    case TOKEN_ADD:
        return "+";
    case TOKEN_SUB:
        return "-";
    case TOKEN_MUL:
        return "*";
    case TOKEN_DIV:
        return "/";

    /* relational tokens */
    case TOKEN_ASS:
        return "=";
    case TOKEN_EQ:
        return "==";
    case TOKEN_NEQ:
        return "!=";
    case TOKEN_GT:
        return ">";
    case TOKEN_LT:
        return "<";
    case TOKEN_GEQ:
        return ">=";
    case TOKEN_LEQ:
        return "<=";

    /* unary operators */
    case TOKEN_NOT_NIL:
        return "!";

    /* other operators */
    case TOKEN_NIL_CHECK:
        return "??";

    /* brackets and parentheses */
    case TOKEN_L_PAR:
        return "(";
    case TOKEN_R_PAR:
        return ")";

    /* others */
    case TOKEN_IDENTIFIER:
        return "ID";

    /* data types */
    case TOKEN_INT:
        return "INT";
    case TOKEN_DBL:
        return "DBL";
    case TOKEN_STRING:
        return "STR";

    default:
        return "$";
    }
    return "";
}

/* debugging */
void print_line(unsigned int width)
{
    printf("+");
    for (int i = 1; i < width - 1; i++)
    {
        printf("-");
    }
    printf("+\n");
}

void print_node(node_t *node, unsigned int width)
{
    print_line(width);
    printf("|");

    int item_length = strlen(node->data.symbol);
    int indent = 2;
    for (int i = 1; i < indent; i++)
    {
        printf(" ");
    }
    if (!node->data.isTerminal)
    {
        item_length = 1;
        printf("E");
    }
    else if (item_length > (width - indent - 2))
    {
        printf("%.5s", node->data.symbol);
        item_length = 5;
    }
    else
    {
        printf("%s", node->data.symbol);
    }

    for (int i = indent + item_length - 1; i < width - 2; i++)
    {
        printf(" ");
    }

    printf("|\n");
}

void print_stack(const symstack_t *stack, unsigned int width)
{
    if (width < 10)
    {
        width = MIN_WIDTH;
    }
    node_t nodes[stack->size];

    node_t *current_node = stack->top;
    for (int i = 0; i < stack->size; i++)
    {
        nodes[i] = *current_node;
        current_node = current_node->previous;
    }

    for (int i = 0; i < stack->size; i++)
    {
        print_node(&nodes[i], width);
    }

    // print end
    print_line(width);
}
