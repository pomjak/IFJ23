/**
 * @file main.c (unit test 4 )
 * @author Pomsar Jakub xpomsa00
 * @brief main for unit test 4
 * @version 0.1
 * @date 2023-10-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "symtable.h"
#include <assert.h>

int main()
{
    symtab_t global_sym_table;
    dstring_t item, value1;
    FILE *input = fopen("input.txt", "r");

    symtable_init(&global_sym_table);

    dstring_init(&item);

    dstring_init(&value1);

    char buffer[50];

    for(int i=0;fgets(buffer, sizeof(buffer), input) != NULL;i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        dstring_clear(&item);

        dstring_add_const_str(&item, buffer);

        assert(symtable_insert(&global_sym_table, &item) == 0);

        assert(set_type(&global_sym_table, &item, i%6 )==0);
    }


    rewind(input);
    bool err;


    for(int i=0;fgets(buffer, sizeof(buffer), input) != NULL;i++)
    {
        if (strchr(buffer, '\n') != NULL)
            *(strchr(buffer, '\n')) = '\0';

        err=true;

        dstring_clear(&item);

        dstring_add_const_str(&item, buffer);

        assert(symtable_search(&global_sym_table, &item) != NULL);

        assert(get_type(&global_sym_table, &item, &err) == i%6);

        assert(err == false);
    }

    dstring_free(&item);

    dstring_free(&value1);

    symtable_dispose(&global_sym_table);

    return 0;
}