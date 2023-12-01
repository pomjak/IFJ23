/**
 * @file symtable.c
 * @author Pomsar Jakub <xpomsa00@stud.fit.vutbr.cz>
 * @brief Implementation of symtable (version hashtable with open addressing)
 * @version 0.1
 * @date 2023-10-13
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "symtable.h"

void report_error(unsigned int *error, const unsigned int err_type)
{
    if (*error == SYMTAB_OK)
        *error = err_type;
}

symtab_t *create_local_symtab(unsigned int *error)
{
    symtab_t * new_local = malloc(sizeof(symtab_t));

    if (!new_local)
    {
        report_error(error, ERR_INTERNAL);
        return NULL;
    }

    symtable_init(new_local, error);

    return new_local;
}

void symtable_init(symtab_t *symtab, unsigned int *error)
{
    *error = SYMTAB_OK;
    symtab->size = 11;
    symtab->count = 0;
    symtab->deactivated = 0;

    symtab->items = malloc(sizeof(symtab_item_t) * symtab->size);
    if (!symtab->items)
        report_error(error, ERR_INTERNAL);

    for (size_t i = 0; i < symtab->size; i++)
        symtab->items[i] = NULL;
}

/**
 * @brief implementation of djb2 hash
 *
 * @param id
 * @return unsigned long
 */
unsigned long hash(char *id, size_t size)
{
    unsigned long hash = 0;
    const unsigned char *p;

    for (p = (const unsigned char *)id; *p != '\0'; p++)
        hash = 65599 * hash + *p;
    return (hash % size);
}

unsigned long hash2(char *id, size_t size)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *id++))
        hash = ((hash << 5) + hash) + c;

    return (hash % (size - 1)) + 1;
}

unsigned long get_hash(dstring_t *id, symtab_item_t **items, size_t size)
{
    char *wanted = dstring_to_str(id);

    unsigned long index = hash(wanted, size);
    unsigned long step = hash2(wanted, size);

    for (int i = 0; items[index] != NULL; i++) // gets hash of 1st null slot
        index = (index + i * step) % size;     // if occupied, double hash

    return index;
}

symtab_item_t *symtable_search(symtab_t *symtab, dstring_t *id, unsigned int *error)
{
    *error = SYMTAB_OK;

    if (symtab == NULL)
    {
        report_error(error, SYMTAB_NOT_INITIALIZED);
        return NULL;
    }

    char *wanted = dstring_to_str(id);

    unsigned long index = hash(wanted, symtab->size);
    unsigned long step = hash2(wanted, symtab->size);

    for (int i = 0; symtab->items[index] != NULL; i++)
    {
        if (!dstring_cmp(&(symtab->items[index])->name, id))
        {
            if ((symtab->items[index])->active) // if name matches and item is active,g success
            {
                return (symtab->items[index]);
            }
            else // if item is inactive, it was deleted, thus not found
            {
                report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
                return NULL;
            }
        }
        else
        {
            index = (index + i * step) % symtab->size;
        }
    }
    report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
    return NULL;
}

symtab_item_t *item_init(dstring_t *id, unsigned int *error)
{
    static unsigned int unique_id = 0;
    symtab_item_t *new = malloc(sizeof(symtab_item_t));

    if (!new)
    {
        report_error(error, ERR_INTERNAL);
        return NULL;
    }

    if (!dstring_init(&new->name))
    {
        report_error(error, ERR_INTERNAL);
        return NULL;
    }

    if (!dstring_copy(id, &new->name))
    {
        report_error(error, ERR_INTERNAL);
        return NULL;
    }

    new->active = true;
    new->uid = unique_id;
    unique_id++;
    new->type = undefined;
    new->is_mutable = false;
    new->is_func_defined = false;
    new->is_var_initialized = false;
    new->is_nillable = false;
    new->variadic_param = false;
    new->parameters = NULL;
    new->return_type = undefined;

    return new;
}

void resize(symtab_t *symtab, unsigned int *error)
{
    const size_t primes[] = {11, 23, 53, 107, 211, 421, 853, 1699, 3209, 6553, 12409, 25229};

    size_t new_size = 0;
    for (int i = 0; primes[i]; i++)
    {
        if (symtab->size < primes[i])
        {
            new_size = primes[i];
            break;
        }
    }

    symtab_item_t **resized_items = malloc(sizeof(symtab_item_t) * new_size);

    if (!resized_items)
    {
        report_error(error, ERR_INTERNAL);
        return;
    }

    for (size_t i = 0; i < new_size; i++)
        resized_items[i] = NULL;

    for (size_t i = 0; i < symtab->size; i++)
    {
        if (symtab->items[i] && symtab->items[i]->active)
        {
            uint64_t new_hash = get_hash(&symtab->items[i]->name, resized_items, new_size);
            resized_items[new_hash] = symtab->items[i];
        }
    }

    free(symtab->items);

    symtab->items = resized_items;
    symtab->size = new_size;
    symtab->count -= symtab->deactivated;
    symtab->deactivated = 0;
}

void check_load(symtab_t *symtab, unsigned int *error)
{
    if (0.65 < (float)((float)symtab->count / (float)symtab->size))
        resize(symtab, error);
}

void symtable_insert(symtab_t *symtab, dstring_t *id, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error); // try to search in symtab
    if (!item)                                                // if not in symtab alloc new slot for data
    {   
        *error = SYMTAB_OK; //only case when it is okay that search failed

        symtab->items[get_hash(id, symtab->items, symtab->size)] = item_init(id, error); // handover pointer to new allocated item
        symtab->count++;
        check_load(symtab, error);
    }
    else
    {
        report_error(error, SYMTAB_ERR_ITEM_ALREADY_STORED);
        return;
    }
}

void symtable_delete(symtab_t *symtab, dstring_t *target, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, target, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return;
    }

    item->active = false;
    symtab->deactivated++;
}
void param_dispose(param_t *first)
{
    while (first)
    {
        param_t *temp = first->next;
        dstring_free(&first->name);
        dstring_free(&first->label);
        free(first);

        first = temp;
    }
}

void symtable_dispose(symtab_t *symtab)
{
    for (size_t i = 0; i < symtab->size; ++i)
    {
        if (symtab->items[i] != NULL) // free only allocated pointer, not null pointers
        {
            dstring_free(&symtab->items[i]->name);

            param_dispose(symtab->items[i]->parameters);

            free(symtab->items[i]);
            symtab->items[i] = NULL;
        }
    }
    free(symtab->items);
}

void symtable_clear(symtab_t *local_symtab, unsigned int *error)
{
    symtable_dispose(local_symtab);
    symtable_init(local_symtab, error);
}

void set_type(symtab_t *symtab, dstring_t *id, Type type, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return;
    }
    item->type = type;
}

Type get_type(symtab_t *symtab, dstring_t *id, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error);
    if (item == NULL)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return undefined;
    }
    else
        return item->type;
}

void set_mutability(symtab_t *symtab, dstring_t *id, bool is_mutable, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return;
    }

    if (item->type == function)
    {
        report_error(error, SYMTAB_ERR_ITEM_IS_FUNCTION);
        return;
    }

    item->is_mutable = is_mutable;
}

bool get_mutability(symtab_t *symtab, dstring_t *id, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error);

    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return false;
    }

    if (item->type == function)
    {
        report_error(error, SYMTAB_ERR_ITEM_IS_FUNCTION);
        return false;
    }

    return item->is_mutable;
}

void set_nillable(symtab_t *symtab, dstring_t *id, bool is_nillable, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return;
    }

    item->is_nillable = is_nillable;
}

bool get_nillable(symtab_t *symtab, dstring_t *id, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error);

    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return false;
    }

    return item->is_nillable;
}

void set_func_definition(symtab_t *symtab, dstring_t *id, bool is_func_defined, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return;
    }

    item->is_func_defined = is_func_defined;
}

bool get_func_definition(symtab_t *symtab, dstring_t *id, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return false;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return false;
    }

    return item->is_func_defined;
}


void set_return_type(symtab_t *symtab, dstring_t *id, Type return_type, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return;
    }

    item->return_type = return_type;
}

Type get_return_type(symtab_t *symtab, dstring_t *id, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return undefined;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return undefined;
    }

    return item->return_type;
}

param_t *param_init(dstring_t *name_of_param, unsigned int *error)
{
    param_t *node = malloc(sizeof(param_t));
    if (!node)
    {
        report_error(error, ERR_INTERNAL);
        return NULL;
    }

    if (!dstring_init(&node->name))
    {
        report_error(error, ERR_INTERNAL);
        return NULL;
    }

    if (!dstring_init(&node->label))
    {
        report_error(error, ERR_INTERNAL);
        return NULL;
    }

    if (!dstring_copy(name_of_param, &node->name))
    {
        report_error(error, ERR_INTERNAL);
        return NULL;
    }

    node->type = undefined;
    node->next = NULL;

    return node;
}

param_t *search_param(param_t *first, dstring_t *id, unsigned int *error)
{
    if (!first)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return NULL;
    }

    param_t *node = first;

    do
    {
        if (!dstring_cmp(&node->name, id))
            return node;
        node = node->next;
    } while (node);

    report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
    return NULL;
}

void add_param(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, func_id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return;
    }

    if (!item->parameters)
    {
        item->parameters = param_init(name_of_param, error);
    }
    else
    {
        param_t *runner = item->parameters;

        while (runner->next)
            runner = runner->next;

        runner->next = param_init(name_of_param, error);
        (runner->next)->next = NULL;
    }
}

void set_param_type(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, Type type, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, func_id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return;
    }

    if (!item->parameters)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return;
    }

    param_t *node = search_param(item->parameters, name_of_param, error);

    if (!node)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return;
    }
    node->type = type;
}

Type get_param_type(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, func_id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return undefined;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return undefined;
    }

    if (!item->parameters)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return undefined;
    }
    param_t *node = search_param(item->parameters, name_of_param, error);
    if (!node)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return undefined;
    }

    return node->type;
}

void set_param_label(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, dstring_t *label, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, func_id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return;
    }

    if (!item->parameters)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return;
    }

    param_t *node = search_param(item->parameters, name_of_param, error);
    if (!node)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return;
    }
    dstring_copy(label, &node->label);
}

dstring_t *get_param_label(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, func_id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return NULL;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return NULL;
    }

    if (!item->parameters)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return NULL;
    }

    param_t *node = search_param(item->parameters, name_of_param, error);

    if (!node)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return NULL;
    }

    return &node->label;
}

void set_param_nil(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, bool nil, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, func_id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return;
    }

    if (!item->parameters)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return;
    }

    param_t *node = search_param(item->parameters, name_of_param, error);

    if (!node)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return;
    }
    node->is_nillable = nil;
}

bool get_param_nil(symtab_t *symtab, dstring_t *func_id, dstring_t *name_of_param, unsigned int *error)
{
    symtab_item_t *item = symtable_search(symtab, func_id, error);
    if (!item)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FOUND);
        return false;
    }

    if (item->type != function)
    {
        report_error(error, SYMTAB_ERR_ITEM_NOT_FUNCTION);
        return false;
    }

    if (!item->parameters)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return false;
    }
    param_t *node = search_param(item->parameters, name_of_param, error);
    if (!node)
    {
        report_error(error, SYMTAB_ERR_PARAM_NOT_FOUND);
        return false;
    }

    return node->is_nillable;
}
