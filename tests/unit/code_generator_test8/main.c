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
 * int a = 0;
 * for (int i = 1; i<=5; i++) {
 *      int b = 0;
 *      for (int j = 1; j <= 5; j++) {
 *          int a = j*j;
 *          b += a;
 *      }
 *      a += b;
 * }
 * write(a);
 */

int main(int argc, char ** argv) {
    
    unsigned forId = 0;

    token_T zero;
    zero.type = TOKEN_INT;
    zero.value.int_val = 0;

    token_T one;
    one.type = TOKEN_INT;
    one.value.int_val = 1;

    token_T five;
    five.type = TOKEN_INT;
    five.value.int_val = 5;

    dstring_t a_name;
    dstring_t b_name;
    dstring_t i_name;
    dstring_t j_name;

    dstring_init(&a_name);
    dstring_init(&b_name);
    dstring_init(&i_name);
    dstring_init(&j_name);

    dstring_append(&a_name, 'a');
    dstring_append(&b_name, 'b');
    dstring_append(&i_name, 'i');
    dstring_append(&j_name, 'j');
    

    token_T a;
    a.type = TOKEN_IDENTIFIER;
    a.value.string_val = a_name;

    token_T b;
    b.type = TOKEN_IDENTIFIER;
    b.value.string_val = b_name;

    token_T i;
    i.type = TOKEN_IDENTIFIER;
    i.value.string_val = i_name;

    token_T j;
    j.type = TOKEN_IDENTIFIER;
    j.value.string_val = j_name;

    symtab_t global_symtable;
    scope_t stack;
    
    unsigned int error;

    symtable_init(&global_symtable, &error);

    init_scope(&stack);

    symtable_insert(&global_symtable, &a_name, &error);
    symtable_insert(&global_symtable, &i_name, &error);

    code_generator_set_current_symtable(&global_symtable, &stack);

    // file begin
    code_generator_prolog();

    // 0
    code_generator_push(zero);

    // a = 0
    code_generator_var_declare_token(a);

    // 1
    code_generator_push(one);

    // i = 1
    code_generator_var_declare_token(i);

    // start of for loop IF
    code_generator_for_label(++forId);

    add_scope(&stack, &error);
    
    symtable_insert(stack->local_sym, &j_name, &error);
    symtable_insert(stack->local_sym, &b_name, &error);

    // i to stack
    code_generator_push(i);

    // 5 to stack
    code_generator_push(five);

    // i <= 5
    code_generator_operations(TOKEN_LEQ, true);

    // end of for loop IF
    code_generator_for_loop_if(forId);

    // for body
    code_generator_for_body(forId);

    // 0
    code_generator_push(zero);

    // b = 0
    code_generator_var_declare_token(b);

    // 1
    code_generator_push(one);

    // j = 1
    code_generator_var_declare_token(j);

    
    // start of for loop IF
    code_generator_for_label(++forId);

    add_scope(&stack, &error);
    
    symtable_insert(stack->local_sym, &a_name, &error);

    // j to stack
    code_generator_push(j);

    // 5 to stack
    code_generator_push(five);

    // j <= 5
    code_generator_operations(TOKEN_LEQ, true);

    // end of for loop IF
    code_generator_for_loop_if(forId);

    // for body
    code_generator_for_body(forId);

    // j to stack
    code_generator_push(j);

    // j to stack
    code_generator_push(j);

    // j*j
    code_generator_operations(TOKEN_MUL, true);

    // a = j*j
    code_generator_var_declare_token(a);

    // b to stack
    code_generator_push(b);

    // a to stack
    code_generator_push(a);

    // b+a
    code_generator_operations(TOKEN_ADD, true);

    // b = b+a
    code_generator_var_assign_token(b);

    // 1 to stack
    code_generator_push(one);

    // j to stack
    code_generator_push(j);

    // j++
    code_generator_operations(TOKEN_ADD, true);

    // j = j++
    code_generator_var_assign_token(j);

    // for end
    code_generator_for_loop_end(forId);

    pop_scope(&stack, &error);

    // b to stack
    code_generator_push(b);

    // a to stack
    code_generator_push(a);

    // a+b
    code_generator_operations(TOKEN_ADD, true);

    // a = a+b
    code_generator_var_assign_token(a);

    // 1 to stack
    code_generator_push(one);

    // i to stack
    code_generator_push(i);

    // i++
    code_generator_operations(TOKEN_ADD, true);

    // i = i++
    code_generator_var_assign_token(i);

    // for end
    code_generator_for_loop_end(forId-1);

    pop_scope(&stack, &error);

    code_generator_function_call_param_add("write", a);
    code_generator_function_call("write");

    return 0;
}
