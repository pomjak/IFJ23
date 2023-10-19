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
    param_t *parameters;   // pointer to param_t struct
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

/**
 * @brief Init of sym_table
 *
 * @param symtab pointer to desired symtable
 */
void symtable_init(symtab_t *symtab);

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
 * @return symtab_item_t*   returns pointer to data structure if found, else NULL
 */
symtab_item_t *symtable_search(symtab_t *symtab, dstring_t *id);

/**
 * @brief           returns true/false if id is stored in symtable
 * 
 * @param symtab    ptr to symtable
 * @param id        id to be looked for
 * @return true     if success
 * @return false     if not found
 */
bool is_in_symtable(symtab_t *symtab, dstring_t *id);

/**
 * @brief init of one item in symtable
 *
 * @param id    id of item
 * @param err   err flag
 */
symtab_item_t *item_init(dstring_t *id, bool *err);

/**
 * @brief resizes symtable
 *
 * @param symtab ptr to symtable
 */
void resize(symtab_t *symtab);

/**
 * @brief calculates load of symtable, if needed symtable is resized
 *
 * @param size      size of symtable [max capacity]
 * @param count     actual size of symtable [act N]
 */
void load(symtab_t *symtab);

/**
 * @brief inserts the symtab_item_t data into the specified symtable, if it already exists and matches the id, then updates it
 *
 * @param symtab    the specified table for the data to be inserted into
 * @param id        id to be hashed and stored
 * @return unsigned int  0 if success, 1 if id is already in symtable, else err internal
 */
unsigned int symtable_insert(symtab_t *symtab, dstring_t *id);

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
unsigned int symtable_delete(symtab_t *symtab, dstring_t *target);

/**
 * @brief dispose all allocated params in linked list
 *
 * @param first ptr to first param stored
 */
void param_dispose(param_t *first);

/**
 * @brief clearing all data of local_table
 * 
 * @param local_symtab  ptr to local_table
 *
*/
void symtable_clear(symtab_t *local_symtab);

/**
 * @brief dispose all allocated items in symtable
 *
 * @param symtab to dispose
 */
void symtable_dispose(symtab_t *symtab);

/**
 * @brief Set the local symtable for specified id function
 *
 * @param global_symtab     ptr to global symtable
 * @param func_id           id of function
 * @param local_symtab      ptr to local table
 * @return unsigned int          0 if success, 1 if not found, 2 if item is not function
 */
unsigned int set_local_symtable(symtab_t *global_symtab, dstring_t *func_id, symtab_t *local_symtab);

/**
 * @brief Get the local symtable for specifed function from global symtable
 *
 * @param global_symtab     ptr to global symtable
 * @param func_id           function id for which local symtable is returned
 * @param err               err flag
 * @return symtab_t*        if success, null if id not found, null and err flag is raised if id is not function
 */
symtab_t *get_local_symtable(symtab_t *global_symtab, dstring_t *func_id, bool *err);


/**
 * @brief Set the type of item directly in symtable
 *
 * @param symtab        ptr to symtable
 * @param id            id of modified item
 * @param type          type to be set
 * @return unsigned int      return 0 if success, 1 if not found
 */
unsigned int set_type(symtab_t *symtab, dstring_t *id, Type type);

/**
 * @brief Get the type of item from symtab
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param err           err flag
 * @return Type         when success, if not found, err is set to true and return undefined
 */
Type get_type(symtab_t *symtab, dstring_t *id, bool *err);

/**
 * @brief Set the mutability of item
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param is_mutable    desired value to be set for mutability
 * @return unsigned int      0 if success, 1 if not found, 2 if item is function
 */
unsigned int set_mutability(symtab_t *symtab, dstring_t *id, bool is_mutable);

/**
 * @brief Get the mutability of item
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param err           err flag
 * @return bool         if success, else false and err set to 1
 */
bool get_mutability(symtab_t *symtab, dstring_t *id, bool *err);

/**
 * @brief Set the func definition of item
 *
 * @param symtab            ptr to symtable
 * @param id                id of item
 * @param is_func_defined   desired value to be set for definition of function
 * @return unsigned int          0 if success, 1 if not found, 2 if item is not function
 */
unsigned int set_func_definition(symtab_t *symtab, dstring_t *id, bool is_func_defined);

/**
 * @brief Get the func definition object
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param err           err flag
 * @return bool         is_func_defined if success, false and err set to true if not found
 */
bool get_func_definition(symtab_t *symtab, dstring_t *id, bool *err);

/**
 * @brief Set the var declaration object
 *
 * @param symtab            ptr to symtable
 * @param id                id of item
 * @param is_var_declared   desired value to be set for declaration of item
 * @return unsigned int          0 if success, 1 if not found, 2 if item is function
 */
unsigned int set_var_declaration(symtab_t *symtab, dstring_t *id, bool is_var_declared);

/**
 * @brief Get the var declaration object
 *
 * @param symtab        ptr to symtable
 * @param id            id of item
 * @param err           err flag
 * @return bool         var_declaration if success, false and err set to true if not found
 */
bool get_var_declaration(symtab_t *symtab, dstring_t *id, bool *err);


/**
 * @brief Set the return type of item if type is function
 *
 * @param symtab        ptr to symtable
 * @param id            id of modified item
 * @param return_type   desired value to be set for return_type
 * @return unsigned int      0 if success, 1 if not found, 2 if item is not function
 */
unsigned int set_return_type(symtab_t *symtab, dstring_t *id, Type return_type);

/**
 * @brief Get the return type of function
 *
 * @param symtab        ptr to symtable
 * @param id            id of function
 * @param err           err flag
 * @return Type         return_type if success,undefined and err set to true if not found
 */
Type get_return_type(symtab_t *symtab, dstring_t *id, bool *err);

/**
 * @brief initialization of param node
 *
 * @param name_of_param     name of param
 * @param err               err flag
 * @return param_t*         initialized param if success, null and err flag is raised
 */
param_t *param_init(dstring_t *name_of_param, bool *err);

/**
 * @brief searches the whole linked list of params for id
 *
 * @param first     ptr from symtable to fist param node
 * @param id        id to be searched for
 * @return param_t* when success, else NULL
 */
param_t *search_param(param_t *first, dstring_t *id);

/**
 * @brief add param to the end of linked list of params
 *
 * @param symtab            ptr to symtable
 * @param func_id           id of function with params
 * @param name_of_param     name of param of function id
 * @param err               err flag
 * @return unsigned int          0 if success, 1 if function not found, 2 if item is not function, err flag is raised if internal error occurs
 */
unsigned int add_param(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, bool *err);

/**
 * @brief Set the param type
 *
 * @param symtab            ptr to symtable
 * @param func_id           id of function with params
 * @param name_of_param     name of param
 * @param type              type of param to be set
 * @return unsigned int          0 if success, 1 if function not found, 2 if item is not an function, 3 if param not found
 */
unsigned int set_param_type(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, Type type);

/**
 * @brief Get the param type
 *
 * @param symtab            ptr to symtable
 * @param func_id           id of function with params
 * @param name_of_param     name of param
 * @param type              type of param to be set
 * @param err               raised to 1 if item not found, 2 if item is not function, 3 if param is not found
 * @return Type             if success, else undefined and err flag is raised [1,2,3] as mentioned earlier
 */
Type get_param_type(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, unsigned int *err);

/**
 * @brief Set the param label
 *
 * @param symtab            ptr to symtable
 * @param func_id           id of function with params
 * @param name_of_param     name of param
 * @param label             label of param to be set
 * @return unsigned int          0 if success, 1 if function not found, 2 if item is not an function, 3 if param not found
 */
unsigned int set_param_label(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, dstring_t *label);

/**
 * @brief Get the param label
 *
 * @param symtab            ptr to symtable
 * @param func_id           id of function with params
 * @param name_of_param     name of param
 * @param err               raised to 1 if item not found, 2 if item is not function, 3 if param is not found
 * @return dstring*         if success, else NULL and err flag is raised [1,2,3] as mentioned earlier
 */
dstring_t *get_param_label(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, unsigned int *err);
