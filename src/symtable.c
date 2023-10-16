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

uint8_t symtable_insert(symtab_t *symtab, dstring_t *id)
{
    symtab_item_t *item = symtable_search(symtab, id); // try to search in symtab
    if (!item)                                         // if not in symtab alloc new slot for data
    {
        symtab_item_t *new = malloc(sizeof(symtab_item_t));

        if (!new)
            return ERR_INTERNAL;

        dstring_copy(id, &new->name);          // copy id
        (*symtab)[get_hash(id, symtab)] = new; // handover poiter to new allocated item
    }

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

uint8_t set_local_symtable(symtab_t *global_symtab, dstring_t *func_id, symtab_t *local_symtab)
{
    symtab_item_t *item = symtable_search(global_symtab, func_id);
    if (!item)
        return 1;
    if (item->type != function)
        return 2;
    item->local_symtable = local_symtab;
    return 0;
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
    {
        if (dstring_init(&item->value))
            return ERR_INTERNAL;
        if (!dstring_copy(value, &item->value))
            return ERR_INTERNAL;
    }
    return 0;
}

dstring_t *get_value(symtab_t *symtab, dstring_t *id)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return NULL;
    return (&item->value);
}

uint8_t set_type(symtab_t *symtab, dstring_t *id, Type type)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    item->type = type;
    return 0;
}

Type get_type(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
    {
        *err = true;
        return undefined;
    }
    else
    {
        *err = false;
        return item->type;
    }
}

uint8_t set_mutability(symtab_t *symtab, dstring_t *id, bool is_mutable)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (item->type == function)
        return 2;
    item->is_mutable = is_mutable;
    return 0;
}

bool get_mutability(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item || item->type == function)
    {
        *err = true;
        return false;
    }
    else
    {
        *err = false;
        return item->is_mutable;
    }
}

uint8_t set_func_definition(symtab_t *symtab, dstring_t *id, bool is_func_defined)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (item->type != function)
        return 2;
    item->is_func_defined = is_func_defined;
    return 0;
}

bool get_func_definition(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item || item->type != function)
    {
        *err = true;
        return false;
    }
    else
    {
        *err = false;
        return item->is_func_defined;
    }
}

uint8_t set_var_declaration(symtab_t *symtab, dstring_t *id, bool is_var_declared)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (item->type == function)
        return 2;
    item->is_var_declared = is_var_declared;
    return 0;
}

bool get_var_declaration(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item || item->type == function)
    {
        *err = true;
        return false;
    }
    else
    {
        *err = false;
        return item->is_var_declared;
    }
}

uint8_t set_constant(symtab_t *symtab, dstring_t *id, bool is_constant)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (item->type == function)
        return 2;
    item->is_const = is_constant;
    return 0;
}

bool get_constant(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item || item->type == function)
    {
        *err = true;
        return false;
    }
    else
    {
        *err = false;
        return item->is_const;
    }
}

uint8_t set_return_type(symtab_t *symtab, dstring_t *id, Type return_type)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item)
        return 1;
    if (item->type != function)
        return 2;

    item->return_type = return_type;

    return 0;
}

Type get_return_type(symtab_t *symtab, dstring_t *id, bool *err)
{
    symtab_item_t *item = symtable_search(symtab, id);
    if (!item || item->type != function)
    {
        *err = true;
        return undefined;
    }
    else
    {
        *err = false;
        return item->type;
    }
}