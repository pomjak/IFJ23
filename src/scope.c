/**
 * @file scope.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz>
 * @brief Implementation of stack for local symbol tables
 * @version 0.1
 * @date 2023-11-20
 *
 * @copyright Copyright (c) 2023
 */

#include "scope.h"

// initialize the scope stack
void init_scope(scope_t *first)
{
    *first = NULL;
    return;
}

// add a new scope to the stack
void add_scope(scope_t *first, unsigned int *error)
{
    *error = SYMTAB_OK;

    // allocate memory for the new scope element
    scope_t new = malloc(sizeof(struct scope_element));

    if (!new)
    {
        // report an internal error if memory allocation fails
        report_error(error, ERR_INTERNAL);
        return;
    }

    // link the new scope to the stack and create a local symbol table for it
    new->next = *first;
    new->local_sym = create_local_symtab(error);
    *first = new;
}

// pop the top scope from the stack
void pop_scope(scope_t *first, unsigned int *error)
{
    *error = SYMTAB_OK;

    if (*first != NULL)
    {
        // save the next scope, dispose of the current scope's resources, and update the stack
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

// dispose of all scopes in the stack
void dispose_scope(scope_t *first, unsigned int *error)
{
    *error = SYMTAB_OK;

    scope_t temp;
    if (*first == NULL)
        return;

    // iterate through all scopes, dispose of resources, and free memory
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

// search for an item in the scopes
symtab_item_t *search_scopes(scope_t stack, dstring_t *id, unsigned int *error)
{
    if (stack == NULL)
    {
        // report an error if the symbol table stack is not initialized
        report_error(error, SYMTAB_NOT_INITIALIZED);
        return NULL;
    }

    // iterate through scopes to find the item
    while (stack)
    {
        if (symtable_search(stack->local_sym, id, error) == NULL)
        {
            stack = stack->next;
        }
        else
            break;
    }

    // return the found item or report an error if not found
    if (peek_scope(stack))
        return symtable_search(stack->local_sym, id, error);
    else
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return NULL;
    }
}

// search for an initialized variable in the scopes
symtab_item_t *search_scopes_initialized_var(scope_t stack, dstring_t *id, unsigned int *error)
{
    if (stack == NULL)
    {
        // report an error if the symbol table stack is not initialized
        report_error(error, SYMTAB_NOT_INITIALIZED);
        return NULL;
    }

    // iterate through scopes to find the initialized variable
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

    // return the found item or report an error if not found
    if (peek_scope(stack))
        return symtable_search(stack->local_sym, id, error);
    else
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return NULL;
    }
}

// check if the scope stack is not empty
bool peek_scope(scope_t stack)
{
    return (stack != NULL);
}
