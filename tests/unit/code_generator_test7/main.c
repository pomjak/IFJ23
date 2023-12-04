/**
 * @name IFJ23
 * @file main.c
 * @brief main to run code generator
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 1.12.2023
 **/

#include <stdio.h>
#include <stdlib.h>
#include "code_generator.h"
#include "dyn_string.h"
#include "symtable.h"
#include "scope.h"

/**
 * a = readInt();
 * b = readInt();
 * 
 * int foo(int a) {
 *      a = a + b
 *      return a;
 * }
 * 
 * c = foo(a);
 * 
 * write(c, a);
 */

int main(int argc, char ** argv) {
    
    unsigned forId = 0;

    dstring_t a_name;
    dstring_t b_name;
    dstring_t c_name;

    symtab_t global_symtable;
    scope_t stack;
    unsigned int error;

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

    symtable_init(&global_symtable, &error);

    init_scope(&stack);

    symtable_insert(&global_symtable, &a_name, &error);
    symtable_insert(&global_symtable, &b_name, &error);
    symtable_insert(&global_symtable, &c_name, &error);

    symtable_search(&global_symtable, &a_name, &error)->is_var_initialized = true;
    symtable_search(&global_symtable, &b_name, &error)->is_var_initialized = true;
    symtable_search(&global_symtable, &c_name, &error)->is_var_initialized = true;

    code_generator_set_current_symtable(&global_symtable, &stack);

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

    add_scope(&stack, &error);
    symtable_insert(stack->local_sym, &a_name, &error);
    symtable_search(stack->local_sym, &a_name, &error)->is_var_initialized = true;

    // int a
    code_generator_param_map("a", 0);

    // function code
    code_generator_push(a);
    code_generator_push(b);

    // a + b
    code_generator_operations(TOKEN_ADD, true);

    // a = a + b
    code_generator_var_assign_token(a);

    code_generator_push(a);

    // code_generator_push(a); // return a;
    code_generator_return(); // this return item on stack

    code_generator_function_end("foo");

    pop_scope(&stack, &error);

    // foo(a, b)
    code_generator_function_call_param_add("foo", a);
    code_generator_function_call("foo");

    // c = foo(a, b);
    code_generator_var_declare_token(c);

    code_generator_function_call_param_add("write", c);
    code_generator_function_call_param_add("write", a);
    code_generator_function_call("write");

    return 0;
}
