/**
 * @file main.c (unit test 7)
 * @author Pomsar Jakub xpomsa00
 * @brief main for unit test 7 - adding params to item which is not function
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
    uint8_t error = 255;
    bool err;

    symtable_init(&global_sym_table,11);

    dstring_init(&item);
    dstring_add_const_str(&item, "item1");

    dstring_init(&param);
    dstring_add_const_str(&param, "x");

    dstring_init(&label);
    dstring_add_const_str(&label, "with");

    
    // insert new item
    symtable_insert(&global_sym_table, &item);

    assert(set_type(&global_sym_table, &item, integer) == 0);

    assert(get_type(&global_sym_table, &item, &err) == integer);

    assert(err == false);
    
    // insert new param, however item is integer not function
    assert(add_param(&global_sym_table, &item, &param, &err) == 2);
    // try to set label for param, but param was not added and item is function
    assert(set_param_label(&global_sym_table, &item, &param, &label) == 2);
    // get label of param
    assert(get_param_label(&global_sym_table, &item, &param, &error) == NULL);
    // err flag is raised to value of 3 [param not found]
    assert(error == 2);

    assert(set_param_type(&global_sym_table, &item, &param, integer) == 2);

    assert(get_param_type(&global_sym_table, &item, &param, &error) == undefined);

    assert(error == 2);
    
    
    dstring_free(&item);
    dstring_free(&param);
    dstring_free(&label);

    symtable_dispose(&global_sym_table);

    return 0;
}