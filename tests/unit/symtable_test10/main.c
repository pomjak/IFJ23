/**
 * @file main.c (for testing purposes)
 * @author Pomsar Jakub xpomsa00
 * @brief   main for unit test 10 - used every created function
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
    dstring_t item1, item2, item3, item4, param, label;
    unsigned int error;

    /**
     * @brief init of dstrings
     *
     */
    dstring_init(&item1);
    dstring_add_const_str(&item1, "item1");

    dstring_init(&item2);
    dstring_add_const_str(&item2, "item2");

    dstring_init(&item3);
    dstring_add_const_str(&item3, "item3");

    dstring_init(&item4);
    dstring_add_const_str(&item4, "item4");

    dstring_init(&param);
    dstring_add_const_str(&item4, "param");

    dstring_init(&label);
    dstring_add_const_str(&item4, "label");

    symtable_init(&global_sym_table, &error);
    assert(error == SYMTAB_OK);

    /**
     * @brief insert items into symtab
     *
     */
    symtable_insert(&global_sym_table, &item1, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(&global_sym_table, &item2, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(&global_sym_table, &item3, &error);
    assert(error == SYMTAB_OK);

    symtable_insert(&global_sym_table, &item4, &error);
    assert(error == SYMTAB_OK);

    /**
     * @brief set type
     *
     */
    set_type(&global_sym_table, &item1, function, &error);
    assert(error == SYMTAB_OK);

    set_type(&global_sym_table, &item2, integer, &error);
    assert(error == SYMTAB_OK);

    set_type(&global_sym_table, &item3, double_, &error);
    assert(error == SYMTAB_OK);

    set_type(&global_sym_table, &item4, string, &error);
    assert(error == SYMTAB_OK);

    /**
     * @brief get type without errors
     *
     */
    assert(get_type(&global_sym_table, &item1, &error) == function);
    assert(error == SYMTAB_OK);

    assert(get_type(&global_sym_table, &item2, &error) == integer);
    assert(error == SYMTAB_OK);

    assert(get_type(&global_sym_table, &item3, &error) == double_);
    assert(error == SYMTAB_OK);

    assert(get_type(&global_sym_table, &item4, &error) == string);
    assert(error == SYMTAB_OK);

    /**
     * @brief set mutability (fails with function)
     *
     */
    set_mutability(&global_sym_table, &item1, true, &error);
    assert(error == SYMTAB_ERR_ITEM_IS_FUNCTION);

    set_mutability(&global_sym_table, &item2, true, &error);
    assert(error == SYMTAB_OK);

    set_mutability(&global_sym_table, &item3, true, &error);
    assert(error == SYMTAB_OK);

    set_mutability(&global_sym_table, &item4, true, &error);
    assert(error == SYMTAB_OK);

    /**
     * @brief get mutability (fails with function)
     *
     */
    assert(get_mutability(&global_sym_table, &item1, &error) == false);
    assert(error == SYMTAB_ERR_ITEM_IS_FUNCTION);

    assert(get_mutability(&global_sym_table, &item2, &error) == true);
    assert(error == SYMTAB_OK);

    assert(get_mutability(&global_sym_table, &item3, &error) == true);
    assert(error == SYMTAB_OK);

    assert(get_mutability(&global_sym_table, &item4, &error) == true);
    assert(error == SYMTAB_OK);

    /**
     * @brief set declaration of function (success only with function)
     *
     */
    set_func_definition(&global_sym_table, &item1, true, &error);
    assert(error == SYMTAB_OK);

    set_func_definition(&global_sym_table, &item2, true, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    set_func_definition(&global_sym_table, &item3, true, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    set_func_definition(&global_sym_table, &item4, true, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    /**
     * @brief get definition of function (success only with function)
     *
     */
    assert(get_func_definition(&global_sym_table, &item1, &error) == true);
    assert(error == SYMTAB_OK);

    assert(get_func_definition(&global_sym_table, &item2, &error) == false);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_func_definition(&global_sym_table, &item3, &error) == false);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_func_definition(&global_sym_table, &item4, &error) == false);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    /**
     * @brief set declaration of variable (fails with function)
     *
     */
    set_var_declaration(&global_sym_table, &item1, true, &error);
    assert(error == SYMTAB_ERR_ITEM_IS_FUNCTION);

    set_var_declaration(&global_sym_table, &item2, true, &error);
    assert(error == SYMTAB_OK);

    set_var_declaration(&global_sym_table, &item3, true, &error);
    assert(error == SYMTAB_OK);

    set_var_declaration(&global_sym_table, &item4, true, &error);
    assert(error == SYMTAB_OK);

    /**
     * @brief get declaration of variable (fails with function)
     *
     */
    assert(get_var_declaration(&global_sym_table, &item1, &error) == false);
    assert(error == SYMTAB_ERR_ITEM_IS_FUNCTION);

    assert(get_var_declaration(&global_sym_table, &item2, &error) == true);
    assert(error == SYMTAB_OK);

    assert(get_var_declaration(&global_sym_table, &item3, &error) == true);
    assert(error == SYMTAB_OK);

    assert(get_var_declaration(&global_sym_table, &item4, &error) == true);
    assert(error == SYMTAB_OK);

    /**
     * @brief set nullability of variable (okay)
     *
     */
    set_nillable(&global_sym_table, &item1, true, &error);
    assert(error == SYMTAB_OK);

    set_nillable(&global_sym_table, &item2, true, &error);
    assert(error == SYMTAB_OK);

    set_nillable(&global_sym_table, &item3, true, &error);
    assert(error == SYMTAB_OK);

    set_nillable(&global_sym_table, &item4, true, &error);
    assert(error == SYMTAB_OK);

    /**
     * @brief get nil of variable (should be okay)
     *
     */
    assert(get_nillable(&global_sym_table, &item1, &error) == true);
    assert(error == SYMTAB_OK);

    assert(get_nillable(&global_sym_table, &item2, &error) == true);
    assert(error == SYMTAB_OK);

    assert(get_nillable(&global_sym_table, &item3, &error) == true);
    assert(error == SYMTAB_OK);

    assert(get_nillable(&global_sym_table, &item4, &error) == true);
    assert(error == SYMTAB_OK);
    /**
     * @brief set return type of function (success only with function)
     *
     */
    set_return_type(&global_sym_table, &item1, integer, &error);
    assert(error == SYMTAB_OK);

    set_return_type(&global_sym_table, &item2, integer, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    set_return_type(&global_sym_table, &item3, integer, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    set_return_type(&global_sym_table, &item4, integer, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    /**
     * @brief get return type of function (success only with function)
     *
     */
    assert(get_return_type(&global_sym_table, &item1, &error) == integer);
    assert(error == SYMTAB_OK);

    assert(get_return_type(&global_sym_table, &item2, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_return_type(&global_sym_table, &item3, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_return_type(&global_sym_table, &item4, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    /**
     * @brief add function params (success only with function)
     *
     */
    add_param(&global_sym_table, &item1, &param, &error);
    assert(error == SYMTAB_OK);

    add_param(&global_sym_table, &item2, &param, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    add_param(&global_sym_table, &item3, &param, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    add_param(&global_sym_table, &item4, &param, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    /**
     * @brief search function param (success only with function)
     *
     */
    assert(search_param(symtable_search(&global_sym_table, &item1, &error)->parameters, &param, &error) != NULL);
    assert(error == SYMTAB_OK);

    assert(search_param(symtable_search(&global_sym_table, &item2, &error)->parameters, &param, &error) == NULL);
    assert(error == SYMTAB_ERR_PARAM_NOT_FOUND);

    assert(search_param(symtable_search(&global_sym_table, &item3, &error)->parameters, &param, &error) == NULL);
    assert(error == SYMTAB_ERR_PARAM_NOT_FOUND);

    assert(search_param(symtable_search(&global_sym_table, &item4, &error)->parameters, &param, &error) == NULL);
    assert(error == SYMTAB_ERR_PARAM_NOT_FOUND);

    /**
     * @brief set type of param (success only with function)
     *
     */
    set_param_type(&global_sym_table, &item1, &param, integer, &error);
    assert(error == SYMTAB_OK);

    set_param_type(&global_sym_table, &item2, &param, integer, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    set_param_type(&global_sym_table, &item3, &param, integer, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    set_param_type(&global_sym_table, &item4, &param, integer, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    /**
     * @brief get type of param (success only with function)
     *
     */
    assert(get_param_type(&global_sym_table, &item1, &param, &error) == integer);
    assert(error == SYMTAB_OK);

    assert(get_param_type(&global_sym_table, &item2, &param, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_param_type(&global_sym_table, &item3, &param, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_param_type(&global_sym_table, &item4, &param, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    /**
     * @brief set label param (success only with function)
     *
     */
    set_param_label(&global_sym_table, &item1, &param, &label, &error);
    assert(error == SYMTAB_OK);

    set_param_label(&global_sym_table, &item2, &param, &label, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    set_param_label(&global_sym_table, &item3, &param, &label, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    set_param_label(&global_sym_table, &item4, &param, &label, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    /**
     * @brief get label param (success only with function)
     *
     */
    assert(dstring_cmp(get_param_label(&global_sym_table, &item1, &param, &error), &label) == 0);
    assert(error == SYMTAB_OK);

    assert(get_param_label(&global_sym_table, &item2, &param, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_param_label(&global_sym_table, &item3, &param, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_param_label(&global_sym_table, &item4, &param, &error) == undefined);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);


/**
     * @brief set nillable for param  (success only with function)
     *
     */
    set_param_nil(&global_sym_table, &item1, &param, true, &error);
    assert(error == SYMTAB_OK);

    set_param_nil(&global_sym_table, &item2, &param, true, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    set_param_nil(&global_sym_table, &item3, &param, true, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    set_param_nil(&global_sym_table, &item4, &param, true, &error);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    /**
     * @brief get type of param (success only with function)
     *
     */
    assert(get_param_nil(&global_sym_table, &item1, &param, &error) == true);
    assert(error == SYMTAB_OK);

    assert(get_param_nil(&global_sym_table, &item2, &param, &error) == false);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_param_nil(&global_sym_table, &item3, &param, &error) == false);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    assert(get_param_nil(&global_sym_table, &item4, &param, &error) == false);
    assert(error == SYMTAB_ERR_ITEM_NOT_FUNCTION);

    /**
     * @brief dispose whole symtable and dstrings
     *
     */
    symtable_dispose(&global_sym_table);

    dstring_free(&item1);
    dstring_free(&item2);
    dstring_free(&item3);
    dstring_free(&item4);
    dstring_free(&param);
    dstring_free(&label);

    return 0;
}