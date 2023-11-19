/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 2 every setter & getter for 1 item
 * @version 0.1
 * @date 2023-10-14
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "symtable.h"
#include <assert.h>

int main()
{
    symtab_t global_sym_table;
    dstring_t item1, value1;
    unsigned int error;

    dstring_init(&item1);
    dstring_add_const_str(&item1, "item1");
    dstring_init(&value1);
    dstring_add_const_str(&item1, "value1");

    symtable_init(&global_sym_table, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(&global_sym_table, &item1, &error);
    assert(error == SYMTAB_OK);

    set_type(&global_sym_table, &item1, function, &error);
    assert(error == SYMTAB_OK);

    assert(get_type(&global_sym_table, &item1, &error) == function);
    assert(error == SYMTAB_OK);

    set_return_type(&global_sym_table, &item1, integer, &error);
    assert(error == SYMTAB_OK);

    assert(get_return_type(&global_sym_table, &item1, &error) == integer);
    assert(error == SYMTAB_OK);

    // item is function, mutability cannot be set
    set_mutability(&global_sym_table, &item1, true, &error);
    assert(error == SYMTAB_ERR_ITEM_IS_FUNCTION);
    // mutability stays unset (unset from item_init)
    assert(get_mutability(&global_sym_table, &item1, &error) == false);
    assert(error == SYMTAB_ERR_ITEM_IS_FUNCTION);
    // item is function so error flag is raised

    set_func_definition(&global_sym_table, &item1, true, &error);
    assert(error == SYMTAB_OK);

    assert(get_func_definition(&global_sym_table, &item1, &error) == true);
    assert(error == SYMTAB_OK);

    symtab_t local_symtable;

    symtable_init(&local_symtable, &error);
    assert(error == SYMTAB_OK);

    dstring_free(&item1);
    dstring_free(&value1);

    symtable_dispose(&global_sym_table);
    symtable_dispose(&local_symtable);

    return 0;
}