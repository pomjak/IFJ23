/**
 * @file main.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz
 * @brief   test 01 for symstack [init, add scope, dispose]
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
    symstack_t local_table = NULL;
    dstring_t item1, value1;
    unsigned int error;

    dstring_init(&item1);
    dstring_add_const_str(&item1, "item1");
    dstring_init(&value1);
    dstring_add_const_str(&item1, "value1");

    init_symstack(&local_table);

    add_scope(&local_table, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(local_table->local_sym, &item1, &error);
    assert(error == SYMTAB_OK);

    dispose_stack(&local_table, &error);
    assert(error == SYMTAB_OK);

    dstring_free(&item1);
    dstring_free(&value1);

    return 0;
}