/**
 * @file main.c (unit test 5)
 * @author Pomsar Jakub xpomsa00
 * @brief main for unit test 5 - operations with params without err
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
    unsigned int error;

    symtable_init(&global_sym_table, &error);
    assert(error == SYMTAB_OK);

    dstring_init(&item);
    dstring_add_const_str(&item, "item1");

    dstring_init(&param);
    dstring_add_const_str(&param, "x");

    dstring_init(&label);
    dstring_add_const_str(&label, "with");

    symtable_insert(&global_sym_table, &item, &error);
    assert(error == SYMTAB_OK);

    set_type(&global_sym_table, &item, function, &error);
    assert(error == SYMTAB_OK);

    assert(get_type(&global_sym_table, &item, &error) == function);
    assert(error == SYMTAB_OK);

    // insert new param
    add_param(&global_sym_table, &item, &param, &error);
    assert(error == SYMTAB_OK);
    // set label for param
    set_param_label(&global_sym_table, &item, &param, &label, &error);
    assert(error == SYMTAB_OK);
    // get label of param
    assert(dstring_cmp(get_param_label(&global_sym_table, &item, &param, &error), &label) == 0);
    assert(error == SYMTAB_OK);
    // without error

    set_param_type(&global_sym_table, &item, &param, integer, &error);
    assert(error == SYMTAB_OK);

    assert(get_param_type(&global_sym_table, &item, &param, &error) == integer);
    assert(error == SYMTAB_OK);

    dstring_free(&item);
    dstring_free(&param);
    dstring_free(&label);

    symtable_dispose(&global_sym_table);

    return 0;
}