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
typedef enum
{
    undefined,
    function,
    integer,
    double_,
    string,
    nil,
    constant
} Type;

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
    struct param *next;
} param_t;

/**
 * @brief struct of one record in symtable
 *
 */
typedef struct symtab_item
{
    bool active;          // active - 0 -> item was deleted (but kept in htab for path-finding[implicit synonyms])
    dstring_t name;       // id
    Type type;            // func,int,dbl,str,const
    bool is_mutable;      // true for var, false for let
    bool defined;         // true if func was already defined, else false
    bool declared;        // true if item was already declared, else false
    dstring_t value;      // value
    param_t *parametrs;   // pointer to param_t struct
    Type return_type;     // anything but func
    void *local_symtable; // points to local symtable if item is function
} symtab_item_t;

typedef symtab_item_t *symtab_t[SYMTAB_SIZE]; // ?FIXME maybe dynamic resizing is needed?

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
 * @cite                from ial presentation 6th 21/22
 * @param id            identifier to be hashed
 * @return u_int32_t    new hashed key
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
 * @param symtab            the specified table to search
 * @param id                the ID to search for
 * @return symtab_item_t*   returns pointer to data structure if found, else NULL
 */
symtab_item_t *symtable_search(symtab_t *symtab, dstring_t *id);

/**
 * @brief inserts the symtab_item_t data into the specified symtable, if it already exists and matches the id, then updates it
 *
 * @param symtab    the specified table for the data to be inserted into
 * @param id        id to be hashed
 * @param data      ptr to data, which will be inserted or updated
 * @return uint8_t
 */
uint8_t symtable_insert(symtab_t *symtab, dstring_t *id, symtab_item_t *data);

/**
 * @brief delete (set active to false) in specified symtable based on id
 * @details
 * The symtable delete operation does not perform a true deletion in the traditional sense.
 * Rather than removing or freeing the item from the symtable, it marks the item's 'activity' attribute as false.
 * This means that while the item technically still exists in the symtable,
 * it is effectively deactivated or considered inactive.
 * As a result, searches or operations involving this item will not yield expected results,
 * essentially making it appear as though the item has been deleted.
 * @param symtab    targeted symtab
 * @param target    target to be deleted(set inactive)
 * @return uint8_t  0 for successful deactivating, 1 otherwise
 */
uint8_t symtable_delete(symtab_t *symtab, dstring_t *target);

/**
 * @brief dispose all allocated items in symtable
 *
 * @param symtab to dispose
 */
void symtable_dispose(symtab_t *symtab);

/**
 * @brief Get the local symtable for specifed function from global symtable
 *
 * @param global_symtab     ptr to global symtable
 * @param func_id           function id for which local symtable is returned
 * @return symtab_t*        ptr to local symtable if success, else if func_id is not stored or not an function -> NULL
 */
symtab_t *get_local_symtable(symtab_t *global_symtab, dstring_t *func_id);

/**
 * @brief Set the value and type of item directly in symtable
 * @attention
 * if desired to set only one of item attributes [value/type],
 * the other should be set to its default value [value: NULL / type: undefined]
 * @param symtab        ptr to symtable
 * @param id            id of modified item
 * @param value         value to be set
 * @param type          type to be set
 * @return uint8_t      return 0 if success, 1 if not found,
 */
uint8_t set_value_and_type(symtab_t *symtab, dstring_t *id, dstring_t *value, Type type);

/**
 * @brief Set the flags of item directly in symtable
 *
 * @param symtab        ptr to symtable
 * @param id            id of modified item
 * @param is_mutable    is_mutable flag 1-set / 0-unset
 * @param defined       defined flag 1-set / 0-unset
 * @param declared      declared flag 1-set / 0-unset
 * @return unit8_t      uint8_t return 0 if success, 1 if not found,
 */
uint8_t set_flags(symtab_t *symtab, dstring_t *id, bool is_mutable, bool defined, bool declared);

/**
 * @brief Set the return type of item if type is function
 *
 * @param symtab    ptr to symtable
 * @param id        id of modified item
 * @return uint8_t  0 if success, 1 if item is not function, 2 if item not found
 */
uint8_t set_return_type(symtab_item_t *symtab, dstring_t *id, Type return_type);

/**
 * @brief Get the type of item from symtab
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @attention           return of type ***undefined*** -> not found or id is not defined yet
 * @return Type         when succes, else undefined (or item is simply yet not defined)
 */
Type get_type(symtab_t *symtab, dstring_t *id);
