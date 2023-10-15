#include "symtable.h"

void symtable_init(symtab_t *symtab)
{
    for (int i = 0; i < SYMTAB_SIZE; i++)
        (*symtab)[i] = NULL;
}

/**
 * @brief implementation of djb2 hash
 *
 * @param id
 * @return unsigned long
 */
unsigned long hash(char *id)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *id++))
        hash = ((hash << 5) + hash) + c;

    return (hash % SYMTAB_SIZE);
}

unsigned long hash2(char *id)
{
    unsigned int seed = 131;
    unsigned int hash;
    unsigned int i;

    for (i = 0, hash = 0; *id; id++, i++)
        hash = (hash * seed) + (*id);

    return (hash % SYMTAB_SIZE);
}

unsigned long get_hash(dstring_t *id, symtab_t *symtab)
{
    char *wanted = dstring_to_str(id);

    unsigned long index = hash(wanted);
    unsigned long step = hash2(wanted);

    while ((*symtab)[index] != NULL)          // gets hash of 1st null slot
        index = (index + step) % SYMTAB_SIZE; // if occupied, double hash

    return index;
}

symtab_item_t *symtable_search(symtab_t *symtab, dstring_t *id)
{
    char *wanted = dstring_to_str(id);

    unsigned long index = hash(wanted);
    unsigned long step = hash2(wanted);

    if (symtab == NULL)
        return NULL;

    while ((*symtab)[index] != NULL)
    {
        if (!dstring_cmp(&((*symtab)[index])->name, id))
            if ((*symtab)[index]->active) // if name matches and item is active, success
                return ((*symtab)[index]);
            else // if item is inactive, it was deleted, not found
                return NULL;
        else
            index = (index + step) % SYMTAB_SIZE;
    }
    return NULL;
}

uint8_t symtable_insert(symtab_t *symtab, dstring_t *id, symtab_item_t *data)
{
    symtab_item_t *item = symtable_search(symtab, id); // try to search in symtab
    if (!item)                                         // if not in symtab alloc new slot for data
    {
        symtab_item_t *new = malloc(sizeof(symtab_item_t));

        if (!new)
            return ERR_INTERNAL;

        *new = *data;                          // copy data
        (*symtab)[get_hash(id, symtab)] = new; // handover poiter to new allocated item
    }
    else // if already in symtab, update
        *item = *data;

    return 0;
}

uint8_t symtable_delete(symtab_t *symtab, dstring_t *target)
{
    symtab_item_t *item = symtable_search(symtab, target);
    if (!item)
        return 1;

    item->active = false;
    return 0;
}

void symtable_dispose(symtab_t *symtab)
{
    for (int i = 0; i < SYMTAB_SIZE; ++i)
    {
        if ((*symtab)[i] != NULL) // free only allocated pointer, not null pointers
        {
            free((*symtab)[i]);
            (*symtab)[i] = NULL;
        }
    }
}

symtab_t *get_local_symtable(symtab_t *global_symtab, dstring_t *func_id)
{
    symtab_item_t *item = symtable_search(global_symtab, func_id);
    if (item && item->type == function) // if found item is not null and is function, return ptr to local symtable
        return item->local_symtable;
    else // else null
        return NULL;
}

uint8_t set_value(symtab_t *symtab, dstring_t *id, dstring_t *value)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (value != NULL)
        if (!dstring_copy(value, &item->value))
            return ERR_INTERNAL;
    return 0;
}

uint8_t set_type(symtab_t *symtab, dstring_t *id, Type type)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    item->type = type;
    return 0;
}

uint8_t set_flags(symtab_t *symtab, dstring_t *id, bool is_mutable, bool is_func_defined, bool is_var_declared, bool is_constant)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (is_mutable != NULL)
        item->is_mutable = is_mutable;
    if (is_func_defined != NULL)
        item->is_func_defined = is_func_defined;
    if (is_var_declared != NULL)
        item->is_var_declared = is_var_declared;
    if (is_constant != NULL)
        item->is_const = is_constant;

    return 0;
}

uint8_t set_return_type(symtab_t *symtab, dstring_t *id, Type return_type)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 2;
    if (item->type != function)
        return 1;

    item->return_type = return_type;

    return 0;
}

Type get_type(symtab_t *symtab, dstring_t *id)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return undefined;
    return item->type;
}