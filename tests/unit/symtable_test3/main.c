/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 3 insert & search of multiple items
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
    symtab_t local_sym_table;
    dstring_t temp;

    dstring_init(&temp);
    dstring_add_const_str(&temp, "x");

    symtab_item_t data;

    data.active = true;
    data.is_mutable = false;
    dstring_init(&data.name);
    dstring_copy(&temp, &data.name);
    dstring_init(&data.value);
    dstring_add_const_str(&data.value, "3.92e-9");
    data.type = double_;

    symtable_init(&global_sym_table);

    symtable_insert(&global_sym_table, &temp, &data);

    symtab_item_t *getter;
    getter = symtable_search(&global_sym_table, &temp);

    assert(getter != NULL);
    assert(getter->active == true);
    assert(getter->is_mutable == false);
    assert(dstring_cmp(&getter->name, &temp) == 0);
    assert(dstring_cmp_const_str(&getter->value, "3.92e-9") == 0);
    assert(data.type == double_);

    dstring_t temp1;

    dstring_init(&temp1);
    dstring_add_const_str(&temp1, "a");

    symtable_init(&local_sym_table);

    data.active = true;
    data.is_mutable = false;
    dstring_copy(&temp1, &data.name);
    data.type = function;
    data.return_type = integer;
    data.local_symtable = &local_sym_table;

    assert(symtable_insert(&global_sym_table, &temp1, &data) == 0);

    symtab_item_t *getter1;
    getter1 = symtable_search(&global_sym_table, &temp1);
    
    assert(getter1 != NULL);
    assert(getter1->active == true);
    assert(getter1->is_mutable == false);
    assert(dstring_cmp(&getter1->name, &temp1) == 0);
    assert(getter1->type == function);
    assert(getter1->return_type == integer);
    assert((symtab_t *)getter1->local_symtable == &local_sym_table);

    dstring_free(&temp);
    dstring_free(&temp1);
    dstring_free(&data.name);
    dstring_free(&data.value);

    symtable_dispose(&global_sym_table);
    symtable_dispose(&local_sym_table);

    return 0;
}