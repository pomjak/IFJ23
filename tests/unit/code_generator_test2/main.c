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
 * a = readInt();
 * b = readInt();
 * 
 * int foo(int a, int b) {
 *      return a + b;
 * }
 * 
 * c = foo(a, b);
 * 
 * write(c);
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

    // file begin
    code_generator_prolog();

    // readInt();
    code_generator_function_call("readInt");

    // a = readInt();
    code_generator_var_declare_token(a);

    // readInt();
    code_generator_function_call("readInt");

    // b = readInt();
    code_generator_var_declare_token(b);

    // function foo
    code_generator_function_label("foo");

    // int a
    code_generator_param_map("a", 0);
    
    // int b
    code_generator_param_map("b", 1);

    // function code
    code_generator_push(a);
    code_generator_push(b);

    // a + b
    code_generator_operations(TOKEN_ADD, true);

    // code_generator_push(a); // return a;
    code_generator_return(); // this return item on stack

    code_generator_function_end("foo");

    // foo(a, b)
    code_generator_function_call_param_add("foo", a);
    code_generator_function_call_param_add("foo", b);
    code_generator_function_call("foo");

    // c = foo(a, b);
    code_generator_var_declare_token(c);

    code_generator_function_call_param_add("write", c);
    code_generator_function_call("write");

    return 0;
}
