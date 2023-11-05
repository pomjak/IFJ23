/**
 * @file main.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz
 * @brief   test 02 for symstack [init, add scope, dispose]
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "symstack.h"
#include <assert.h>

int main()
{
    symstack_t stack = NULL;
    dstring_t item1, value1;
    unsigned int error;

    dstring_init(&item1);
    dstring_add_const_str(&item1, "item1");

    dstring_init(&value1);
    dstring_add_const_str(&item1, "value1");

    init_symstack(&stack);

    add_scope(&stack, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(stack->local_sym, &item1, &error);
    assert(error == SYMTAB_OK);

    assert(symtable_search(stack->local_sym,&item1, &error) != NULL);
    assert(error == SYMTAB_OK);

    pop_scope(&stack, &error);
    assert(error == SYMTAB_OK);

    dispose_stack(&stack, &error);
    assert(error == SYMTAB_OK);

    dstring_free(&item1);
    dstring_free(&value1);

    return 0;
}