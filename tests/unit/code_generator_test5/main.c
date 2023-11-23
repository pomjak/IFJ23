/**
 * @name IFJ23
 * @file main.c
 * @brief main to run code generator
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 23.11.2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include "code_generator.h"
#include "dyn_string.h"

/** 
 * c = substring("", 0, 1)
 * if c == nil write("Dobry den")
 * c = substring("Dobry den", 0, 5)
 * write(c)
 */

int main(int argc, char ** argv) {
    
    unsigned forId = 0;

    dstring_t c_name;
    dstring_t b_string;
    dstring_t a_string;

    dstring_init(&c_name);
    dstring_init(&a_string);
    dstring_init(&b_string);

    dstring_append(&c_name, 'c');
    dstring_add_const_str(&b_string, "Dobry den");

    token_T c;
    c.type = TOKEN_IDENTIFIER;
    c.value.string_val = c_name;

    token_T a_str;
    a_str.type = TOKEN_STRING;
    a_str.value.string_val = a_string;

    token_T b_str;
    b_str.type = TOKEN_STRING;
    b_str.value.string_val = b_string;

    token_T i;
    i.type = TOKEN_INT;
    i.value.int_val = 0;

    token_T j_a;
    j_a.type = TOKEN_INT;
    j_a.value.int_val = 1;

    token_T j_b;
    j_b.type = TOKEN_INT;
    j_b.value.int_val = 5;

    token_T nil;
    nil.type = TOKEN_NIL;

    // file begin
    code_generator_prolog();

    // substring()
    code_generator_function_call_param_add("substring", a_str);
    code_generator_function_call_param_add("substring", i);
    code_generator_function_call_param_add("substring", j_a);
    code_generator_function_call("substring");

    // c = substring();
    code_generator_var_declare_token(c);

    code_generator_push(c);
    code_generator_push(nil);
    code_generator_operations(TOKEN_EQ, false);
    code_generator_if_header(0);

    code_generator_function_call_param_add("write", b_str);
    code_generator_function_call("write");
    
    code_generator_if_else(0);
    code_generator_if_end(0);

    // substring();
    code_generator_function_call_param_add("substring", b_str);
    code_generator_function_call_param_add("substring", i);
    code_generator_function_call_param_add("substring", j_b);
    code_generator_function_call("substring");

    // c = substring();
    code_generator_var_assign_token(c);

    code_generator_function_call_param_add("write", c);
    code_generator_function_call("write");

    return 0;
}
