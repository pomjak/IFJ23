/**
 * @name IFJ23
 * @file main.c
 * @brief main to run code generator
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 22.11.2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include "code_generator.h"
#include "dyn_string.h"

/**
 * a = ord("")
 * b = ord("#a")
 * write(a, " ", b)
 */

int main(int argc, char ** argv) {
    
    unsigned forId = 0;

    dstring_t a_name;
    dstring_t b_name;
    dstring_t empty_str;
    dstring_t hash_str;
    dstring_t space_str;

    dstring_init(&a_name);
    dstring_init(&b_name);
    dstring_init(&empty_str);
    dstring_init(&hash_str);
    dstring_init(&space_str);

    dstring_append(&a_name, 'a');
    dstring_append(&b_name, 'b');
    dstring_append(&hash_str, '#');
    dstring_append(&hash_str, 'a');
    dstring_append(&space_str, ' ');
    

    token_T a;
    a.type = TOKEN_IDENTIFIER;
    a.value.string_val = a_name;

    token_T b;
    b.type = TOKEN_IDENTIFIER;
    b.value.string_val = b_name;

    token_T empty_s;
    empty_s.type = TOKEN_STRING;
    empty_s.value.string_val = empty_str;

    token_T hash_s;
    hash_s.type = TOKEN_STRING;
    hash_s.value.string_val = hash_str;

    token_T space;
    space.type = TOKEN_STRING;
    space.value.string_val = space_str;

    // file begin
    code_generator_prolog();

    // ord()
    code_generator_function_call_param_add("ord", empty_s);
    code_generator_function_call("ord");

    // a = ord();
    code_generator_var_declare_token(a);

    // ord();
    code_generator_function_call_param_add("ord", hash_s);
    code_generator_function_call("ord");

    // b = ord();
    code_generator_var_declare_token(b);

    code_generator_function_call_param_add("write", a);
    code_generator_function_call_param_add("write", space);
    code_generator_function_call_param_add("write", b);
    code_generator_function_call("write");

    return 0;
}
