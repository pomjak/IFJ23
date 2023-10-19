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
    bool err_bl;
    unsigned int err_int;

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
    symtable_init(&global_sym_table);

    /**
     * @brief insert items into symtab
     *
     */
    assert(symtable_insert(&global_sym_table, &item1) == 0);
    assert(symtable_insert(&global_sym_table, &item2) == 0);
    assert(symtable_insert(&global_sym_table, &item3) == 0);
    assert(symtable_insert(&global_sym_table, &item4) == 0);

    /**
     * @brief check if they are actually stored in symtab
     *
     */
    assert(is_in_symtable(&global_sym_table, &item1) == true);
    assert(is_in_symtable(&global_sym_table, &item2) == true);
    assert(is_in_symtable(&global_sym_table, &item3) == true);
    assert(is_in_symtable(&global_sym_table, &item4) == true);

    /**
     * @brief set type
     *
     */
    assert(set_type(&global_sym_table, &item1, function) == 0);
    assert(set_type(&global_sym_table, &item2, integer) == 0);
    assert(set_type(&global_sym_table, &item3, double_) == 0);
    assert(set_type(&global_sym_table, &item4, string) == 0);

    /**
     * @brief get type without errors
     *
     */
    assert(get_type(&global_sym_table, &item1, &err_bl) == function);
    assert(err_bl == false);
    assert(get_type(&global_sym_table, &item2, &err_bl) == integer);
    assert(err_bl == false);
    assert(get_type(&global_sym_table, &item3, &err_bl) == double_);
    assert(err_bl == false);
    assert(get_type(&global_sym_table, &item4, &err_bl) == string);
    assert(err_bl == false);

    /**
     * @brief set mutability (fails with function)
     *
     */
    assert(set_mutability(&global_sym_table, &item1, true) == 2);
    assert(set_mutability(&global_sym_table, &item2, true) == 0);
    assert(set_mutability(&global_sym_table, &item3, true) == 0);
    assert(set_mutability(&global_sym_table, &item4, true) == 0);

    /**
     * @brief get mutability (fails with function)
     *
     */
    assert(get_mutability(&global_sym_table, &item1, &err_bl) == false);
    assert(err_bl == true);
    assert(get_mutability(&global_sym_table, &item2, &err_bl) == true);
    assert(err_bl == false);
    assert(get_mutability(&global_sym_table, &item3, &err_bl) == true);
    assert(err_bl == false);
    assert(get_mutability(&global_sym_table, &item4, &err_bl) == true);
    assert(err_bl == false);

    /**
     * @brief set declaration of function (success only with function)
     *
     */
    assert(set_func_definition(&global_sym_table, &item1, true) == 0);
    assert(set_func_definition(&global_sym_table, &item2, true) == 2);
    assert(set_func_definition(&global_sym_table, &item3, true) == 2);
    assert(set_func_definition(&global_sym_table, &item4, true) == 2);

    /**
     * @brief get definition of function (success only with function)
     *
     */
    assert(get_func_definition(&global_sym_table, &item1, &err_bl) == true);
    assert(err_bl == false);
    assert(get_func_definition(&global_sym_table, &item2, &err_bl) == false);
    assert(err_bl == true);
    assert(get_func_definition(&global_sym_table, &item3, &err_bl) == false);
    assert(err_bl == true);
    assert(get_func_definition(&global_sym_table, &item4, &err_bl) == false);
    assert(err_bl == true);

    /**
     * @brief set declaration of variable (fails with function)
     *
     */
    assert(set_var_declaration(&global_sym_table, &item1, true) == 2);
    assert(set_var_declaration(&global_sym_table, &item2, true) == 0);
    assert(set_var_declaration(&global_sym_table, &item3, true) == 0);
    assert(set_var_declaration(&global_sym_table, &item4, true) == 0);

    /**
     * @brief get declaration of variable (fails with function)
     *
     */
    assert(get_var_declaration(&global_sym_table, &item1, &err_bl) == false);
    assert(err_bl == true);
    assert(get_var_declaration(&global_sym_table, &item2, &err_bl) == true);
    assert(err_bl == false);
    assert(get_var_declaration(&global_sym_table, &item3, &err_bl) == true);
    assert(err_bl == false);
    assert(get_var_declaration(&global_sym_table, &item4, &err_bl) == true);
    assert(err_bl == false);

    /**
     * @brief set return type of function (success only with function)
     *
     */
    assert(set_return_type(&global_sym_table, &item1, integer) == 0);
    assert(set_return_type(&global_sym_table, &item2, integer) == 2);
    assert(set_return_type(&global_sym_table, &item3, integer) == 2);
    assert(set_return_type(&global_sym_table, &item4, integer) == 2);

    /**
     * @brief get return type of function (success only with function)
     *
     */
    assert(get_return_type(&global_sym_table, &item1, &err_bl) == integer);
    assert(err_bl == false);
    assert(get_return_type(&global_sym_table, &item2, &err_bl) == undefined);
    assert(err_bl == true);
    assert(get_return_type(&global_sym_table, &item3, &err_bl) == undefined);
    assert(err_bl == true);
    assert(get_return_type(&global_sym_table, &item4, &err_bl) == undefined);
    assert(err_bl == true);

    /**
     * @brief add function params (success only with function)
     *
     */
    assert(add_param(&global_sym_table, &item1, &param, &err_bl) == 0);
    assert(err_bl == false); // err flag for internal error
    assert(add_param(&global_sym_table, &item2, &param, &err_bl) == 2);
    assert(err_bl == false);
    assert(add_param(&global_sym_table, &item3, &param, &err_bl) == 2);
    assert(err_bl == false);
    assert(add_param(&global_sym_table, &item4, &param, &err_bl) == 2);
    assert(err_bl == false);

    /**
     * @brief search function param (success only with function)
     *
     */
    assert(search_param(symtable_search(&global_sym_table, &item1)->parameters, &param) != NULL);
    assert(search_param(symtable_search(&global_sym_table, &item2)->parameters, &param) == NULL);
    assert(search_param(symtable_search(&global_sym_table, &item3)->parameters, &param) == NULL);
    assert(search_param(symtable_search(&global_sym_table, &item4)->parameters, &param) == NULL);

    /**
     * @brief set type of param (success only with function)
     *
     */
    assert(set_param_type(&global_sym_table, &item1, &param, integer) == 0);
    assert(set_param_type(&global_sym_table, &item2, &param, integer) == 2);
    assert(set_param_type(&global_sym_table, &item3, &param, integer) == 2);
    assert(set_param_type(&global_sym_table, &item4, &param, integer) == 2);

    /**
     * @brief get type of param (success only with function)
     *
     */
    assert(get_param_type(&global_sym_table, &item1, &param, &err_int) == integer);
    assert(err_int == 0);
    assert(get_param_type(&global_sym_table, &item2, &param, &err_int) == undefined);
    assert(err_int == 2);
    assert(get_param_type(&global_sym_table, &item3, &param, &err_int) == undefined);
    assert(err_int == 2);
    assert(get_param_type(&global_sym_table, &item4, &param, &err_int) == undefined);
    assert(err_int == 2);

    /**
     * @brief set label param (success only with function)
     *
     */
    assert(set_param_label(&global_sym_table, &item1, &param, &label) == 0);
    assert(set_param_label(&global_sym_table, &item2, &param, &label) == 2);
    assert(set_param_label(&global_sym_table, &item3, &param, &label) == 2);
    assert(set_param_label(&global_sym_table, &item4, &param, &label) == 2);

/**
     * @brief get label param (success only with function)
     *
     */
    assert(dstring_cmp(get_param_label(&global_sym_table, &item1, &param, &err_int), &label) == 0);
    assert(err_int == 0);
    assert(get_param_label(&global_sym_table, &item2, &param, &err_int) == undefined);
    assert(err_int == 2);
    assert(get_param_label(&global_sym_table, &item3, &param, &err_int) == undefined);
    assert(err_int == 2);
    assert(get_param_label(&global_sym_table, &item4, &param, &err_int) == undefined);
    assert(err_int == 2);

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