/**
 * @file symtable.h
 * @author Pomsar Jakub xpomsa00
 * @brief implementation of symtable(version hashtable with open addressing)
 * @version 0.1
 * @date 2023-10-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "dyn_string.h"
#include "error.h"

#define SYMTAB_SIZE 1021 //max size of htab items is prime number for better distribution 

/**
 * @brief different types that can be stored are function, variable and its type(int,dbl,str,nil) or constant?
 *
 */
enum Type
{
    function,
    integer,
    double_,
    string,
    nil,
    constant
};

//? FIXME maybe useless ?
/**
 * @brief struct for storing paramaters passed to functions
 *
 */
typedef struct param     
{
    dstring_t name;
    dstring_t label;
    Type type;
}param_t;

/**
 * @brief struct of one record in symtable
 *
 */
typedef struct symtab_item
{
    dstring name;
    Type type;
    bool is_mutable;
    dstring_t value;
    param_t *parametrs;
    Type return_type;
    //TODO pointer to local table for function
}symtab_item_t;

typedef symtab_item_t *symtab[SYMTAB_SIZE];

u_int8_t symtable_init(symtab*);
