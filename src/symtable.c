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