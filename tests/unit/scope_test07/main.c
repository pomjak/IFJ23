/**
 * @file main.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz
 * @brief   test 05 for scope
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "scope.h"
#include <assert.h>

int main()
{
    scope_t stack;
    dstring_t item;
    unsigned int error;

    /*-----------------------------------------*/

    dstring_init(&item);
    dstring_add_const_str(&item, "item");

    init_scope(&stack);

    /*-----------------------------------------*/

    add_scope(&stack, &error);
    symtable_insert(stack->local_sym, &item, &error);
    symtable_search(stack->local_sym, &item, &error)->is_var_initialized = true;

    /*-----------------------------------------*/

    add_scope(&stack, &error);
    symtable_insert(stack->local_sym, &item, &error);
    symtable_search(stack->local_sym, &item, &error)->is_var_initialized = false;

    /*-----------------------------------------*/

    assert(search_scopes_initialized_var(stack, &item, &error)->is_var_initialized == true);

    /*-----------------------------------------*/

    dispose_scope(&stack, &error);
    assert(error == SYMTAB_OK);

    dstring_free(&item);

    return 0;
}