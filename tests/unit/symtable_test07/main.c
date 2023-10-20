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
    unsigned int error;

    symtable_init(&global_sym_table, &error);
    assert(error == SYMTAB_OK);
    dstring_init(&item);
    dstring_add_const_str(&item, "item1");

    dstring_init(&param);
    dstring_add_const_str(&param, "x");

    dstring_init(&label);
    dstring_add_const_str(&label, "with");

    // insert new item
    symtable_insert(&global_sym_table, &item, &error);
    assert(error == SYMTAB_OK);

    set_type(&global_sym_table, &item, integer, &error);
    assert(error == SYMTAB_OK);

    assert(get_type(&global_sym_table, &item, &error) == integer);
    assert(error == SYMTAB_OK);

    // insert new param, however item is integer not function
    add_param(&global_sym_table, &item, &param, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);
    // try to set label for param, but param was not added and item is function
    set_param_label(&global_sym_table, &item, &param, &label, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);
    // get label of param
    assert(get_param_label(&global_sym_table, &item, &param, &error) == NULL);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);
    

    set_param_type(&global_sym_table, &item, &param, integer, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_param_type(&global_sym_table, &item, &param, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);


    dstring_free(&item);
    dstring_free(&param);
    dstring_free(&label);

    symtable_dispose(&global_sym_table);

    return 0;
}