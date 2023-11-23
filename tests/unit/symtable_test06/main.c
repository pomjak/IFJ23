/**
 * @file main.c (unit test 6)
 * @author Pomsar Jakub xpomsa00
 * @brief main for unit test 6 - adding params without inserted item in symtab
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
    unsigned int error = 255;
    bool err;

    symtable_init(&global_sym_table, &error);
    assert(error == SYMTAB_OK);

    dstring_init(&item);
    dstring_add_const_str(&item, "item1");

    dstring_init(&param);
    dstring_add_const_str(&param, "x");

    dstring_init(&label);
    dstring_add_const_str(&label, "with");

    // insert new param, but item not inserted
    add_param(&global_sym_table, &item, &param, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FOUND);

    set_param_label(&global_sym_table, &item, &param, &label, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FOUND);

    assert(get_param_label(&global_sym_table, &item, &param, &error) == NULL);
    assert(error == SYMTAB_ERR_ITEM_NOT_FOUND);

    set_param_type(&global_sym_table, &item, &param, integer, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FOUND);

    assert(get_param_type(&global_sym_table, &item, &param, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FOUND);

    dstring_free(&item);
    dstring_free(&param);
    dstring_free(&label);

    symtable_dispose(&global_sym_table);

    return 0;
}