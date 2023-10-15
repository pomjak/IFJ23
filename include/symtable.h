/**
 * @file symtable.h
 * @author Pomsar Jakub xpomsa00
 * @brief Implementation of symtable (version hashtable with open addressing)
 * @version 0.1
 * @date 2023-10-13
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "dyn_string.h"
#include "error.h"

#define SYMTAB_SIZE 1021 // max size of htab items is prime number for better distribution

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
    enum Type type;
} param_t;

/**
 * @brief struct of one record in symtable
 *
 */
typedef struct symtab_item
{
    bool active;           // active - 0 -> item was deleted (but kept in htab for path )
    dstring_t name;        // id
    enum Type type;        // func,int,dbl,str,const
    bool is_mutable;       // true for var, false for let
    dstring_t value;       // value
    param_t *parametrs;    // pointer to param_t struct
    enum Type return_type; // anything but func
    void *local_symtable;  
} symtab_item_t;

typedef symtab_item_t *symtab_t[SYMTAB_SIZE];

/**
 * @brief Init of sym_table
 *
 * @param symtab pointer to desired symtable
 */
void symtable_init(symtab_t *symtab);

/**
 * @brief hash function implemented as DJB2 algo
 *
 * @cite from ial presentation 6th 21/22
 * @param id identifier to be hashed
 * @return u_int32_t hashed key
 */
unsigned long hash(char *id);

/**
 * @brief hash2 for double hashing when collision occurs implemented as BKDR
 *
 * @cite from ial presentation 6th 21/22
 * @param id identifier to be hashed
 * @return u_int32_t new hashed key
 */
unsigned long hash2(char *id);

/**
 * @brief get the hash of free slot or occupied slot with matching id using double hashing
 * 
 * @param id 
 * @param symtab 
 * @return unsigned long 
 */
unsigned long get_hash(dstring_t *id, symtab_t *symtab);

/**
 * @brief search in specified symtable based on id
 *
 * @param symtab the specified table to search
 * @param id the ID to search for
 * @return symtab_item_t* returns pointer to data structure if found, else NULL
 */
symtab_item_t *symtable_search(symtab_t *symtab, dstring_t *id);

/**
 * @brief inserts the symtab_item_t data into the specified symtable, if it already exists and matches the id, then updates it 
 * 
 * @param symtab the specified table for the data to be inserted into
 * @param id id to be hashed
 * @param data ptr to data, which will be inserted or updated
 * @return uint8_t 
 */
uint8_t symtable_insert(symtab_t *symtab,dstring_t *id, symtab_item_t *data);

/**
 * @brief dispose all allocated items in symtable
 * 
 * @param symtab to dispose
 */
void symtable_dispose(symtab_t *symtab);