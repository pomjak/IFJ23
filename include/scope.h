/**
 * @file scope.h
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz>
 * @brief   Declarations of functions of scope
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SCOPE_H
#define SCOPE_H

#include <stdio.h>
#include "symtable.h"

typedef struct scope_element
{
    symtab_t *local_sym;
    struct scope_element *next;

} *scope_t;

/**
 * @brief init scope
 *
 * @param first ptr to stack of scopes [use in argument as "&scope"]
 */
void init_scope(scope_t *first);

/**
 * @brief adds local symtable for new scope
 *
 * @param first ptr to scope [use in argument as "&scope"]
 * @param error error flag from symtab
 */
void add_scope(scope_t *first, unsigned int *error);

/**
 * @brief removes first[last added] stacked local symtab
 *
 * @param first ptr to scope [use in argument as "&scope"]
 * @param error error code
 */
void pop_scope(scope_t *first, unsigned int *error);

/**
 * @brief dispose of the stack of scopes
 *
 * @param first ptr to scope [use in argument as "&scope"]
 * @param error error code
 */
void dispose_scope(scope_t *first, unsigned int *error);

/**
 * @brief searches symbol in the whole stack of symtabs
 *
 * @param stack ptr to scope [use in argument as only "scope"]
 * @param id    id to search for
 * @param error error code
 * @return ptr to found data
 */
symtab_item_t *search_scopes(scope_t stack, dstring_t *id, unsigned int *error);

/**
 * @brief return ptr to first initialized variable in the closest scope
 *
 * @param stack stack of symtables
 * @param id id to search for
 * @param error error
 * @return symtab_item_t*
 */
symtab_item_t *search_scopes_initialized_var(scope_t stack, dstring_t *id, unsigned int *error);

/**
 * @brief peek to stack and find out if it is safe to use stack->local_sym (e.g. with  symtable_insert)
 * otherwise if stack is NULL segmentation fault occurs
 *
 * @param stack ptr to stack [use in argument as "scope"]
 * @return true stack is not null -> safe to use
 * @return false stack is null -> skip
 */
bool peek_scope(scope_t scope);

#endif
