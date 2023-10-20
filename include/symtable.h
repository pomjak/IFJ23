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
#include "debug.h"

#define SYMTAB_OK 0
#define SYMTAB_ERR_ITEM_NOT_FOUND 1
#define SYMTAB_ERR_ITEM_NOT_FUNCTION 2
#define SYMTAB_ERR_ITEM_IS_FUNCTION 3
#define SYMTAB_ERR_PARAM_NOT_FOUND 4
#define SYMTAB_NOT_INITIALIZED 5


/**
 * @brief different types that can be stored are function, variable and its type(int,dbl,str,nil)
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

/**
 * @brief struct for storing parameters passed to functions
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
    Type type;            // func,int,dbl,str,nil,undefined
    bool is_mutable;      // true for var, false for let
    bool is_func_defined; // true if func was already defined, else false
    bool is_var_declared; // true if item was already declared, else false
    bool is_nillable;     // true if item can be nil
    param_t *parameters;  // pointer to param_t struct
    Type return_type;     // anything but func
    void *local_symtable; // points to local symtable if item is function
} symtab_item_t;

typedef struct symtab
{
    symtab_item_t **items;
    size_t count;
    size_t size;
    size_t deactivated;
} symtab_t;

void report_error(unsigned int *error, const unsigned int_);

/**
 * @brief           Init of sym_table
 *
 * @param symtab    pointer to desired symtable
 * @param error     err flag
 */
void symtable_init(symtab_t *symtab, unsigned int *error);

/**
 * @brief hash function implemented as sdbm algo
 *
 * @cite                http://www.cse.yorku.ca/~oz/hash.html
 * @param id            identifier to be hashed
 * @return u_int32_t    hashed key
 */
unsigned long hash(char *id, size_t size);

/**
 * @brief hash2 for double hashing when collision occurs implemented as djb2
 *
 * @cite                from ial presentation 6th 21/22
 * @param id            identifier to be hashed
 * @return u_int32_t    new hashed key
 */
unsigned long hash2(char *id, size_t size);

/**
 * @brief get the hash of free slot using double hashing
 *
 * @param id                id to be hashed
 * @param items             ptr to items of hashtab
 * @param size              size of items
 * @return unsigned long    hash after double hashing
 */
unsigned long get_hash(dstring_t *id, symtab_item_t **items, size_t size);

/**
 * @brief search in specified symtable based on id
 *
 * @param symtab            the specified table to search
 * @param id                the ID to search for
 * @param error             err flag
 * @return symtab_item_t*   returns pointer to data structure if found, else NULL
 */
symtab_item_t *symtable_search(symtab_t *symtab, dstring_t *id, unsigned int *error);

/**
 * @brief init of one item in symtable
 *
 * @param id        id of item
 * @param error     err flag
 */
symtab_item_t *item_init(dstring_t *id, unsigned int *error);

/**
 * @brief           resizes symtable
 *
 * @param symtab    ptr to symtable
 * @param error     err flag
 */
void resize(symtab_t *symtab,unsigned int *error);

/**
 * @brief calculates load of symtable, if needed symtable is resized
 *
 * @param size      size of symtable [max capacity]
 * @param count     actual size of symtable [act N]
 * @param error     err flag
 */
void check_load(symtab_t *symtab,unsigned int *error);

/**
 * @brief inserts the symtab_item_t data into the specified symtable, if it already exists and matches the id, then updates it
 *
 * @param symtab    the specified table for the data to be inserted into
 * @param id        id to be hashed and stored
 * @param error     err flag
 */
void symtable_insert(symtab_t *symtab, dstring_t *id, unsigned int *error);

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
 * @return unsigned int  0 for successful deactivating, 1 otherwise
 */
void symtable_delete(symtab_t *symtab, dstring_t *target,unsigned int *error);

/**
 * @brief dispose all allocated params in linked list
 *
 * @param first ptr to first param stored
 */
void param_dispose(param_t *first);


/**
 * @brief               clearing all data of local_table
 *
 * @param local_symtab  ptr to local_table
 * @param error         err flag
 */
void symtable_clear(symtab_t *local_symtab, unsigned int *error);

/**
 * @brief dispose all allocated items in symtable
 *
 * @param symtab to dispose
 */
void symtable_dispose(symtab_t *symtab);

/**
 * @brief Set the type of item directly in symtable
 *
 * @param symtab        ptr to symtable
 * @param id            id of modified item
 * @param type          type to be set
 * @param error         error flag
 */
void set_type(symtab_t *symtab, dstring_t *id, Type type,unsigned int *error);

/**
 * @brief Get the type of item from symtab
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param error         err flag
 * @return Type         when success, if not found, err flag is raised
 */
Type get_type(symtab_t *symtab, dstring_t *id, unsigned int *error);

/**
 * @brief Set the mutability of item
 *
 * @param symtab            ptr to symtable
 * @param id                id of item
 * @param is_mutable        desired value to be set for mutability
 * @param error             err flag
 */
void set_mutability(symtab_t *symtab, dstring_t *id, bool is_mutable,unsigned int *error);

/**
 * @brief Get the mutability of item
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param error         err flag
 */
bool get_mutability(symtab_t *symtab, dstring_t *id, unsigned int *error);

/**
 * @brief Set the func definition of item
 *
 * @param symtab            ptr to symtable
 * @param id                id of item
 * @param is_func_defined   desired value to be set for definition of function
 * @param error             err flag
 */
void set_func_definition(symtab_t *symtab, dstring_t *id, bool is_func_defined,unsigned int *error);

/**
 * @brief Get the func definition object
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param err           err flag
 * @return bool         is_func_defined if success, false and err flag is raised
 */
bool get_func_definition(symtab_t *symtab, dstring_t *id, unsigned int *error);

/**
 * @brief Set the var declaration object
 *
 * @param symtab            ptr to symtable
 * @param id                id of item
 * @param is_var_declared   desired value to be set for declaration of item
 * @param error             err flag
 */
void set_var_declaration(symtab_t *symtab, dstring_t *id, bool is_var_declared,unsigned int *error);

/**
 * @brief Get the var declaration object
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param error         err flag
 * @return bool         var_declaration if success, false and err flag is raised
 */
bool get_var_declaration(symtab_t *symtab, dstring_t *id, unsigned int *error);

/**
 * @brief Set the return type of item if type is function
 *
 * @param symtab            ptr to symtable
 * @param id                id of modified item
 * @param return_type       desired value to be set for return_type
 * @param error             err flag
 */
void set_return_type(symtab_t *symtab, dstring_t *id, Type return_type,unsigned int *error);

/**
 * @brief Get the return type of function
 *
 * @param symtab        ptr to symtable
 * @param id            id of function
 * @param err           err flag
 * @return Type         return_type if success,undefined and err flag is raised
 */
Type get_return_type(symtab_t *symtab, dstring_t *id, unsigned int *error);

/**
 * @brief initialization of param node
 *
 * @param name_of_param     name of param
 * @param err               err flag
 * @return param_t*         initialized param if success, null and err flag is raised
 */
param_t *param_init(dstring_t *name_of_param, unsigned int *error);

/**
 * @brief searches the whole linked list of params for id
 *
 * @param first         ptr from symtable to fist param node
 * @param id            id to be searched for
 * @return param_t*     when success, else NULL
 */
param_t *search_param(param_t *first, dstring_t *id,unsigned int *error);

/**
 * @brief add param to the end of linked list of params
 *
 * @param symtab            ptr to symtable
 * @param func_id           id of function with params
 * @param name_of_param     name of param of function id
 * @param error             error flag
 */
void add_param(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, unsigned int *error);

/**
 * @brief Set the param type
 *
 * @param symtab            ptr to symtable
 * @param func_id           id of function with params
 * @param name_of_param     name of param
 * @param type              type of param to be set
 * @param error             err flag
 */
void set_param_type(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, Type type,unsigned int *error);

/**
 * @brief Get the param type
 *
 * @param symtab            ptr to symtable
 * @param func_id           id of function with params
 * @param name_of_param     name of param
 * @param type              type of param to be set
 * @param err               err flag
 */
Type get_param_type(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, unsigned int *err);

/**
 * @brief Set the param label
 *
 * @param symtab            ptr to symtable
 * @param func_id           id of function with params
 * @param name_of_param     name of param
 * @param label             label of param to be set
 * @param error             error
 */
void set_param_label(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, dstring_t *label,unsigned int *error);

/**
 * @brief Get the param label
 *
 * @param symtab            ptr to symtable
 * @param func_id           id of function with params
 * @param name_of_param     name of param
 * @param err               err flag
 * @return dstring*         if success, else NULL and err flag is raised 
 */
dstring_t *get_param_label(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, unsigned int *error);
