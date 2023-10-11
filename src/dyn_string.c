/**
 * @name IFJ23
 * @file dyn_string.c
 * @brief Dynamic string operations
 * @author Simon Cagala <xcagal00>
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 06.10.2023
 **/
#include "dyn_string.h"
#include "error.h"


void dstring_clear(dstring_t *dstring) {

    dstring->length = 0;
    dstring->str[dstring->length] = '\0';

}


bool dstring_init(dstring_t *dstring) {

    if(!(dstring->str = (char *)malloc(sizeof(char)*DSTR_ALLOC_SIZE))) {
        print_error(ERR_INTERNAL, "dyn_string: dstring_init: Dynamic string allocation failed.\n");
        return false;
    }
    dstring->alloc_size = DSTR_ALLOC_SIZE;
    dstring_clear(dstring);

    return true;
}


void dstring_free(dstring_t *dstring) {
    
    dstring_clear(dstring);
    free(dstring->str);

}


bool dstring_append(dstring_t *dstring, char c) {
    
    if(dstring->length + 1 >= dstring->alloc_size) {
        // Not enough memory, have to realloc for one more char
        dstring->str = (char *) realloc(dstring->str, (dstring->length+DSTR_ALLOC_SIZE)*sizeof(char));

        if(!(dstring->str)) {
            print_error(ERR_INTERNAL, "dyn_string: dstring_append: realloc failed.\n");
            return false;
        }
        dstring->alloc_size = dstring->length + DSTR_ALLOC_SIZE;
    }

    // Append char at the end of the string
    dstring->str[dstring->length] = c;
    dstring->length += 1;
    // End the new string
    dstring->str[dstring->length] = '\0';

    return true;
}


bool dstring_add_const_str(dstring_t *dstring, const char *str) {

    size_t cstr_len = strlen(str);

    if(dstring->length + cstr_len + 1 >= dstring->alloc_size) {
        size_t new_size = dstring->length + cstr_len + 1;
        
        dstring->str = (char *) realloc(dstring->str, new_size);

        if(!(dstring->str)) {
            print_error(ERR_INTERNAL, "dyn_string: dstring_add_const_str: realloc failed\n");
            return false;
        }
        dstring->alloc_size = new_size;
    }

    dstring->length += cstr_len;
    // Append string to the end of dynstring
    strcat(dstring->str, str);
    dstring->str[dstring->length] = '\0';


    return true;
}


bool dstring_copy(dstring_t *src, dstring_t *dst) {

    size_t new_lenght = src->length;

    if(new_lenght >= dst->alloc_size) {
        dst->str = (char *) realloc(dst->str, new_lenght + 1);
        if(!(dst->str)) {
            print_error(ERR_INTERNAL, "dyn_string: dstring_copy: realloc failed.\n"); //compiler err miesto internal, internal tam je preto aby som to vedel spusit, pravdepodobne editnuty error modul
            return false;
        }
        dst->alloc_size = new_lenght + 1;
    }
    // Copy contents of source to destination
    strcpy(dst->str, src->str);
    dst->length = new_lenght;
    return true;
}


int dstring_cmp_const_str(dstring_t *dstring, const char *cstr) {

    return strcmp(dstring->str, cstr);

}

int dstring_cmp_str(dstring_t *dstring, char *str) {

    return strcmp(dstring->str, str);

}
int dstring_cmp(dstring_t *s1, dstring_t *s2) {
    return strcmp(s1->str, s2->str);
}

char *dstring_to_str(dstring_t *dstring) {
    return dstring->str;
}

void dstring_retract(dstring_t *dstring, int count) {
    dstring->length -= count;
    dstring->str[dstring->length] = '\0';
}