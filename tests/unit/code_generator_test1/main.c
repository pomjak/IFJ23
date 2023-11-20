/**
 * @name IFJ23
 * @file main.c
 * @brief main to run code generator
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 12.11.2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include "code_generator.h"
#include "dyn_string.h"

/**
 * a = readInt();
 * b = readInt();
 * c = 0;
 * // while (a >= b)
 * for ( ;a >= b; ) {
 *  a = a - b;
 *  c++;
 * }
 * 
 * write(c, a);
 */

int main(int argc, char ** argv) {
    
    unsigned forId = 0;

    token_T zero;
    zero.type = TOKEN_INT;
    zero.value.int_val = 0;

    token_T one;
    one.type = TOKEN_INT;
    one.value.int_val = 1;

    dstring_t a_name;
    dstring_t b_name;
    dstring_t c_name;
    dstring_t space_str;

    dstring_init(&a_name);
    dstring_init(&b_name);
    dstring_init(&c_name);
    dstring_init(&space_str);

    dstring_append(&a_name, 'a');
    dstring_append(&b_name, 'b');
    dstring_append(&c_name, 'c');
    dstring_append(&space_str, ' ');
    

    token_T a;
    a.type = TOKEN_IDENTIFIER;
    a.value.string_val = a_name;

    token_T b;
    b.type = TOKEN_IDENTIFIER;
    b.value.string_val = b_name;

    token_T c;
    c.type = TOKEN_IDENTIFIER;
    c.value.string_val = c_name;

    token_T space;
    space.type = TOKEN_STRING;
    space.value.string_val = space_str;

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

    // 0
    code_generator_push(zero);

    // c = 0
    code_generator_var_declare_token(c);

    // start of for loop IF
    code_generator_for_label(++forId);

    // A to stack
    code_generator_push(a);

    // B to stack
    code_generator_push(b);

    // A >= B
    code_generator_operations(TOKEN_GEQ, true);

    // end of for loop IF
    code_generator_for_loop_if(forId);

    // for body
    code_generator_for_body(forId);

    // A to stack
    code_generator_push(a);

    // B to stack
    code_generator_push(b);

    // A - B
    code_generator_operations(TOKEN_SUB, true);

    // A = A - B
    code_generator_var_assign_token(a);

    // C to stack
    code_generator_push(c);

    // 1 to stack
    code_generator_push(one);

    // C + 1
    code_generator_operations(TOKEN_ADD, true);

    // C = C + 1
    code_generator_var_assign_token(c);

    //end of for
    code_generator_for_loop_end(forId);

    code_generator_function_call_param_add("write", "", c);
    code_generator_function_call_param_add("write", "", space);
    code_generator_function_call_param_add("write", "", a);
    code_generator_function_call("write");

    return 0;
}
