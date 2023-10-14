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