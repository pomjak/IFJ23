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
    nil
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
    bool is_func_defined; // true if func was already defined, else false
    bool is_var_declared; // true if item was already declared, else false
    bool is_const;        // true if expression is constant "hello_world", "3", "2.7e10"
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
 * @brief init of one item in symtable
 *
 * @param id    id of item
 * @param err   backcheck flag
 */
symtab_item_t *item_init(dstring_t *id, bool *err);

/**
 * @brief inserts the symtab_item_t data into the specified symtable, if it already exists and matches the id, then updates it
 *
 * @param symtab    the specified table for the data to be inserted into
 * @param id        id to be hashed and stored
 * @return uint8_t  0 if success, 1 if id is already in symtable, else err internal
 */
uint8_t symtable_insert(symtab_t *symtab, dstring_t *id);

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
 * @brief Set the local symtable for specidfied id function
 *
 * @param global_symtab     ptr to global symtable
 * @param func_id           id of function
 * @param local_symtab      ptr to local table
 * @return uint8_t          0 if success, 1 if not found, 2 if item is not function
 */
uint8_t set_local_symtable(symtab_t *global_symtab, dstring_t *func_id, symtab_t *local_symtab);

/**
 * @brief Get the local symtable for specifed function from global symtable
 *
 * @param global_symtab     ptr to global symtable
 * @param func_id           function id for which local symtable is returned
 * @param err               backcheck err flag
 * @return symtab_t*        if success, null if id not found, null and err flag is raised if id is not function
 */
symtab_t *get_local_symtable(symtab_t *global_symtab, dstring_t *func_id, bool *err);

/**
 * @brief Set the value of item directly in symtable
 * @param symtab        ptr to symtable
 * @param id            id of modified item
 * @param value         value to be set
 * @return uint8_t      return 0 if success, 1 if not found
 */
uint8_t set_value(symtab_t *symtab, dstring_t *id, dstring_t *value);

/**
 * @brief Get the value of item
 *
 * @param symtab        ptr to symtable
 * @param id            id of modified item
 * @return dstring_t*   return found value as dstring pointer, else return NULL
 */
dstring_t *get_value(symtab_t *symtab, dstring_t *id);

/**
 * @brief Set the type of item directly in symtable
 *
 * @param symtab        ptr to symtable
 * @param id            id of modified item
 * @param type          type to be set
 * @return uint8_t      return 0 if success, 1 if not found
 */
uint8_t set_type(symtab_t *symtab, dstring_t *id, Type type);

/**
 * @brief Get the type of item from symtab
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param err           backcheck err flag
 * @return Type         when succes, if not found, err is set to true and return undefined
 */
Type get_type(symtab_t *symtab, dstring_t *id, bool *err);

/**
 * @brief Set the mutability of item
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param is_mutable    desired value to be set for mutability
 * @return uint8_t      0 if succcess, 1 if not found, 2 if item is function
 */
uint8_t set_mutability(symtab_t *symtab, dstring_t *id, bool is_mutable);

/**
 * @brief Get the mutability of item
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param err           backcheck flag
 * @return bool         if success, else false and err set to 1
 */
bool get_mutability(symtab_t *symtab, dstring_t *id, bool *err);

/**
 * @brief Set the func definition of item
 *
 * @param symtab            ptr to symtable
 * @param id                id of item
 * @param is_func_defined   desired value to be set for definition of function
 * @return uint8_t          0 if success, 1 if not found, 2 if item is not function
 */
uint8_t set_func_definition(symtab_t *symtab, dstring_t *id, bool is_func_defined);

/**
 * @brief Get the func definition object
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param err           backcheck flag
 * @return bool         is_func_defined if success, false and err set to true if not found
 */
bool get_func_definition(symtab_t *symtab, dstring_t *id, bool *err);

/**
 * @brief Set the var declaration object
 *
 * @param symtab            ptr to symtable
 * @param id                id of item
 * @param is_var_declared   desired value to be set for decalration of item
 * @return uint8_t          0 if success, 1 if not found, 2 if item is function
 */
uint8_t set_var_declaration(symtab_t *symtab, dstring_t *id, bool is_var_declared);

/**
 * @brief Get the var declaration object
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param err           backcheck flag
 * @return bool         var_declaration if success, false and err set to true if not found
 */
bool get_var_declaration(symtab_t *symtab, dstring_t *id, bool *err);

/**
 * @brief Set the constant of variable item
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param is_constant   desired value to be set for is_constant
 * @return uint8_t      0 if success, 1 if not found, 2 if item is function
 */
uint8_t set_constant(symtab_t *symtab, dstring_t *id, bool is_constant);

/**
 * @brief Get the constant object
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param err           backcheck flag
 * @return bool         is_const of item if success, false and err set to true if not found
 */
bool get_constant(symtab_t *symtab, dstring_t *id, bool *err);

/**
 * @brief Set the return type of item if type is function
 *
 * @param symtab        ptr to symtable
 * @param id            id of modified item
 * @param return_type   desired value to be set for return_type
 * @return uint8_t      0 if success, 1 if not found, 2 if item is not function
 */
uint8_t set_return_type(symtab_t *symtab, dstring_t *id, Type return_type);

/**
 * @brief Get the return type of function
 *
 * @param symtab        ptr to symtable
 * @param id            id of function
 * @param err           backcheck flag
 * @return Type         return_type if success,undefined and err set to true if not found
 */
Type get_return_type(symtab_t *symtab, dstring_t *id, bool *err);
