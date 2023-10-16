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

void init_symstack(symstack_t *stack)
{
    stack->top = NULL;
    stack->size = 0;
}

// push
bool symstack_push(symstack_t *stack, data_t data)
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

// pop
data_t symstack_pop(symstack_t *stack)
{
    data_t old_node_data;
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

// peek
data_t symstack_peek(const symstack_t *stack)
{
    return stack->top->data;
}

bool symstack_is_empty(const symstack_t *stack)
{
    return stack->size == 0 && stack->top == NULL;
}

// dispose
bool symstack_dispose(symstack_t *stack)
{
    while (!symstack_is_empty(stack))
    {
        symstack_pop(stack);
    }
    return symstack_is_empty(stack);
}

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

    int item_length = strlen(node->data.c);
    int indent = 2;
    for (int i = 1; i < indent; i++)
    {
        printf(" ");
    }

    if (item_length > (width - indent - 2))
    {
        printf("%.5s", node->data.c);
        item_length = 5;
    }
    else
    {
        printf("%s", node->data.c);
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

/*
 print_line(width);
    printf("|");

    int item_length = strlen(node->data.c);
    int indent = (int)((width - 2 - item_length) / 2);
    for (int i = 1; i < indent; i++)
    {
        printf(" ");
    }
    printf("%s", node->data.c);

    for (int i = indent + item_length - 1; i < width - 1; i++)
    {
        printf(" ");
    }

    printf("|\n");
    */
