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

        *new = *data;
        (*symtab)[get_hash(id, symtab)] = new;
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
        if ((*symtab)[i] != NULL)
        {
            free((*symtab)[i]);
            (*symtab)[i] = NULL;
        }
    }
}
