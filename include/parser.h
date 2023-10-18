/***
 * @brief
 * @author Simon Cagala <xcagal00@stud.fit.vutbr.cz>
 */

#include "error.h"
#include "symtable.h"
#include "debug.h"
#include <stdbool.h>
#include "lexical_analyzer.h"

typedef struct {
    bool in_function;

    token_T *current;
    symtab_t global_symtab;
    symtab_t local_symtab;
} Parser;