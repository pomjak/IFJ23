/**
 * @name IFJ23
 * @file dyn_string.h
 * @brief Dynamic String header
 * @author Simon Cagala <xcagal00>
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 06.10.2023
 **/

#ifndef DYN_STRING_H
#define DYN_STRING_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


/**
 * @brief Initial string alloc size
 **/
#define DSTR_ALLOC_SIZE 8

///====================================================================================================================================///


/**
 * @brief dynamic string (array) implementation
 **/
typedef struct dstring
{
    char *str;          
    size_t length;
    size_t alloc_size;
    
}dstring_t;

/**
 * @brief Initializes a dynamic string
 * 
 * @param *dstring dynstring to init
 * @return true on successful init
 **/
bool dstring_init(dstring_t *dstring);

/**
 * @brief frees allocated memory of dynstring
 * 
 * @param *dstring dynstring pointer to free
 **/
void dstring_free(dstring_t *dstring);

/**
 * @brief Sets dynstring to post-init state
 * 
 * @param *dstring dynstring pointer to clear
 **/
void dstring_clear(dstring_t *dstring);

/**
 * @brief Appends char to the end of dynstring
 * 
 * @param *dstring dynstring pointer 
 * @param c char to add to the end of dynstring
 * @return true on successfull append
 **/
bool dstring_append(dstring_t *dstring, char c);

/**
 * @brief Adds constant string to the end of dynstring
 * 
 * @param dstring dynstring
 * @param str constant string
 * @return true on success
 **/
bool dstring_add_const_str(dstring_t *dstring, const char *str);

/**
 * @brief Copies content of src to dst
 * 
 * @param src source dynstring
 * @param dst destination dynstring
 * @return true on success
 **/
bool dstring_copy(dstring_t *src, dstring_t *dst);

/**
 * @brief Compares dynstring to constant string
 * 
 * @param dstring dynstring
 * @param cstr const string
 * @return 1, 0 or -1, if s1 is gt, equal or lt s2 (just as strcmp())
 **/
int dstring_cmp_const_str(dstring_t *dstring, const char *cstr);

/**
 * @brief Compares dynstring with string
 * 
 * @param dstring dynstring contents to compare
 * @param str string to compare with
 * @return 1, 0 or -1, if s1 is gt, equal or lt s2 (just as strcmp())
 **/
int dstring_cmp_str(dstring_t *dstring, char *str);


/**
 * @brief compares 2 dynstrings
 * 
 * @param s1 
 * @param s2 
 * @return 1, 0 or -1, if s1 is gt, equal or lt s2 (just as strcmp())
 **/
int dstring_cmp(dstring_t *s1, dstring_t *s2);

/**
 * @brief Converts dynamic string contents to classic c string (char *)
 * 
 * @param dstring dynstring pointer
 * @return char* dstring data
 **/
char *dstring_to_str(dstring_t *dstring);

/**
 * @brief Deletes last n chars.
 * 
 * @param dstring dynstring pointer
 * @param count count of deleted chars
 * @return void
 **/
void dstring_retract(dstring_t *dstring, int count);


#endif
