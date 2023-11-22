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
 * a = readInt()
 * b = chr(a)
 * write(b)
 */

int main(int argc, char ** argv) {
    
    unsigned forId = 0;

    dstring_t a_name;
    dstring_t b_name;

    dstring_init(&a_name);
    dstring_init(&b_name);

    dstring_append(&a_name, 'a');
    dstring_append(&b_name, 'b');
    
    token_T a;
    a.type = TOKEN_IDENTIFIER;
    a.value.string_val = a_name;

    token_T b;
    b.type = TOKEN_IDENTIFIER;
    b.value.string_val = b_name;

    // file begin
    code_generator_prolog();

    // readInt();
    code_generator_function_call("readInt");

    // a = readInt();
    code_generator_var_declare_token(a);


    // chr(a)
    code_generator_function_call_param_add("chr", a);
    code_generator_function_call("chr");

    // b = chr(a)
    code_generator_var_declare_token(b);

    code_generator_function_call_param_add("write", b);
    code_generator_function_call("write");

    return 0;
}
