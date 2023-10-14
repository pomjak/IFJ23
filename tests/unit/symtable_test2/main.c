/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 2 insert & search of one item
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
    dstring_t temp;

    dstring_init(&temp);
    dstring_add_const_str(&temp, "x");

    symtab_item_t data;

    data.active = true;
    data.is_mutable = false;
    dstring_init(&data.name);
    dstring_copy(&temp, &data.name);
    data.type = double_;

    symtable_init(&global_sym_table);

    symtable_insert(&global_sym_table,&temp,&data);

    symtab_item_t *getter;
    getter=symtable_search(&global_sym_table, &temp);

    assert(getter != NULL);
    assert(getter->active==true);
    assert(getter->is_mutable==false);
    assert(dstring_cmp(&getter->name,&temp) == 0);
    assert(data.type == double_);
    
    dstring_free(&temp);
    dstring_free(&data.name);

    symtable_dispose(&global_sym_table);

    return 0;
}