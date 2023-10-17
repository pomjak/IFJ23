/**
 * @file main.c (unit test 6)
 * @author Pomsar Jakub xpomsa00
 * @brief main for unit test 5 - operations with params with error handling
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

    symtable_init(&global_sym_table);

    dstring_init(&item);
    dstring_add_const_str(&item, "item1");

    dstring_init(&param);
    dstring_add_const_str(&param, "x");

    dstring_init(&label);
    dstring_add_const_str(&label, "with");

    
    // insert new item
    symtable_insert(&global_sym_table, &item);

    assert(set_type(&global_sym_table, &item, function) == 0);

    assert(get_type(&global_sym_table, &item, &err) == function);

    assert(err == false);
    
    //now item is function but param not set
    assert(set_param_label(&global_sym_table, &item, &param, &label) == 3);

    assert(get_param_label(&global_sym_table, &item, &param, &error) == NULL);

    assert(error == 3);

    assert(set_param_type(&global_sym_table, &item, &param, integer) == 3);

    assert(get_param_type(&global_sym_table, &item, &param, &error) == undefined);

    assert(error == 3);
    
    dstring_free(&item);
    dstring_free(&param);
    dstring_free(&label);

    symtable_dispose(&global_sym_table);

    return 0;
}