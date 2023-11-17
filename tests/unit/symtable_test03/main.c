/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 3 delete of 1 item
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
    dstring_t item1;
    unsigned int error;

    symtable_init(&global_sym_table, &error);
    assert(error == SYMTAB_OK);

    dstring_init(&item1);
    dstring_add_const_str(&item1, "item1");

    // in symtable is stored item with id "item1"
    symtable_insert(&global_sym_table, &item1, &error);
    assert(error == SYMTAB_OK);

    // and successfully searched
    assert(symtable_search(&global_sym_table, &item1, &error) != NULL);
    assert(error == SYMTAB_OK);

    // delete on item with id "item1" returns 0
    symtable_delete(&global_sym_table, &item1, &error);
    assert(error == SYMTAB_OK);

    dstring_free(&item1);

    symtable_dispose(&global_sym_table);

    return 0;
}