/**
 * @name IFJ23
 * @file main.c
 * @brief main to run code generator
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 4.12.2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include "code_generator.h"
#include "dyn_string.h"

/**
 * a;
 * b;
 * c;
 * 
 * pushs(1);
 * pushs(2);
 * pushs(3);
 * 
 * int2doubles(2);
 * 
 * pops(c);
 * pops(b);
 * pops(a);
 * 
 * write(a,b,c);
 */

int main(int argc, char ** argv) {
    
    unsigned forId = 0;

    dstring_t a_name;
    dstring_t b_name;
    dstring_t c_name;

    dstring_init(&a_name);
    dstring_init(&b_name);
    dstring_init(&c_name);

    dstring_append(&a_name, 'a');
    dstring_append(&b_name, 'b');
    dstring_append(&c_name, 'c');
    

    token_T a;
    a.type = TOKEN_IDENTIFIER;
    a.value.string_val = a_name;

    token_T b;
    b.type = TOKEN_IDENTIFIER;
    b.value.string_val = b_name;

    token_T c;
    c.type = TOKEN_IDENTIFIER;
    c.value.string_val = c_name;

    token_T one;
    one.type = TOKEN_INT;
    one.value.int_val = 1;

    token_T two;
    two.type = TOKEN_INT;
    two.value.int_val = 2;

    token_T three;
    three.type = TOKEN_INT;
    three.value.int_val = 3;

    // file begin
    code_generator_prolog();

    code_generator_push(one);
    code_generator_push(two);
    code_generator_push(three);

    code_generator_int2doubles(2);

    code_generator_var_declare_token(c);
    code_generator_var_declare_token(b);
    code_generator_var_declare_token(a);


    code_generator_function_call_param_add("write", a);
    code_generator_function_call_param_add("write", b);
    code_generator_function_call_param_add("write", c);
    code_generator_function_call("write");

    return 0;
}
