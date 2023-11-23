/**
 * @file scope.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz>
 * @brief   Implementation of stack for local symbol tables
 * @version 0.1
 * @date 2023-11-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "scope.h"

void init_scope(scope_t *first)
{
    *first = NULL;
    return;
}

void add_scope(scope_t *first, unsigned int *error)
{
    *error = SYMTAB_OK;

    scope_t new = malloc(sizeof(struct scope_element));

    if (!new)
    {
        report_error(error, ERR_INTERNAL);
        return;
    }

    new->next = *first;
    new->local_sym = create_local_symtab(error);
    *first = new;
}

void pop_scope(scope_t *first, unsigned int *error)
{
    *error = SYMTAB_OK;

    if (*first != NULL)
    {
        scope_t temp;
        temp = (*first)->next;

        symtable_dispose((*first)->local_sym);
        free((*first)->local_sym);

        free(*first);
        (*first) = temp;
    }
    else
        report_error(error, SYMTAB_NOT_INITIALIZED);
}

void dispose_scope(scope_t *first, unsigned int *error)
{
    *error = SYMTAB_OK;

    scope_t temp;
    if (*first == NULL)
        return;

    while (*first)
    {
        temp = (*first)->next;

        symtable_dispose((*first)->local_sym);
        free((*first)->local_sym);

        free(*first);
        (*first) = temp;
    }
    first = NULL;
}

symtab_item_t *search_scopes(scope_t stack, dstring_t *id, unsigned int *error)
{
    if (stack == NULL)
    {
        report_error(error, SYMTAB_NOT_INITIALIZED);
        return NULL;
    }

    while (stack)
    {
        if (symtable_search(stack->local_sym, id, error) == NULL)
        {
            stack = stack->next;
        }
        else
            break;
    }

    if (peek_scope(stack))
        return symtable_search(stack->local_sym, id, error);
    else
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return NULL;
    }
}

symtab_item_t *search_scopes_initialized_var(scope_t stack, dstring_t *id, unsigned int *error)
{
    if (stack == NULL)
    {
        report_error(error, SYMTAB_NOT_INITIALIZED);
        return NULL;
    }

    while (stack)
    {
        symtab_item_t *temp = symtable_search(stack->local_sym, id, error);
        if (temp == NULL)
        {
            stack = stack->next;
        }
        else if (!temp->is_var_initialized)
        {
            stack = stack->next;
        }
        else
            break;
    }

    if (peek_scope(stack))
        return symtable_search(stack->local_sym, id, error);
    else
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return NULL;
    }
}

bool peek_scope(scope_t stack)
{
    return (stack != NULL);
}