/**
 * @file main.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz
 * @brief   test 03 for scope [init, add scope,search_scopes, dispose]
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
    dstring_t item1, value1, item2, value2;
    unsigned int error;

    /*-----------------------------------------*/

    dstring_init(&item1);
    dstring_add_const_str(&item1, "item1");

    dstring_init(&value1);
    dstring_add_const_str(&value1, "value1");

    dstring_init(&item2);
    dstring_add_const_str(&item2, "item2");

    dstring_init(&value2);
    dstring_add_const_str(&value2, "value2");

    init_scope(&stack);

    /*-----------------------------------------*/

    add_scope(&stack, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(stack->local_sym, &item1, &error);
    assert(error == SYMTAB_OK);

    /*-----------------------------------------*/

    add_scope(&stack, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(stack->local_sym, &item2, &error);
    assert(error == SYMTAB_OK);

    /*-----------------------------------------*/

    assert(search_scopes(stack, &item2, &error) != NULL);
    assert(error == SYMTAB_OK);

    assert(search_scopes(stack, &item1, &error) != NULL);
    assert(error == SYMTAB_OK);

    /*-----------------------------------------*/


    dispose_scope(&stack, &error);
    assert(error == SYMTAB_OK);

    dstring_free(&item1);
    dstring_free(&value1);

    dstring_free(&item2);
    dstring_free(&value2);

    return 0;
}