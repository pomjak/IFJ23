/**
 * @file main.c (unit test 5)
 * @author Pomsar Jakub xpomsa00
 * @brief main for unit test 5 - operations with params
 * @version 0.1
 * @date 2023-10-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "symtable.h"
#include <assert.h>

int main()
{
    symtab_t global_sym_table;
    dstring_t item, param, label;

    symtable_init(&global_sym_table);

    dstring_init(&item);
    dstring_add_const_str(&item, "item1");

    dstring_init(&param);
    dstring_add_const_str(&param, "x");

    dstring_init(&label);
    dstring_add_const_str(&label, "with");

    symtable_insert(&global_sym_table, &item);


    bool err;

    assert(set_type(&global_sym_table, &item, function) == 0);

    assert(get_type(&global_sym_table, &item, &err) == function);

    assert(err == false);

    uint8_t error = 255;

    assert(add_param(&global_sym_table, &item,&param,&err) == 0);

    assert(set_param_label(&global_sym_table, &item,&param,&label) == 0);

    assert(dstring_cmp(get_param_label(&global_sym_table, &item,&param,&error),&label) == 0);

    assert(error == 0);


    dstring_free(&item);
    dstring_free(&param);
    dstring_free(&label);

    symtable_dispose(&global_sym_table);

    return 0;
}