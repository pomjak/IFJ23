/**
 * @name IFJ23
 * @file code_generator.h
 * @brief Code generator header
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 19.11.2023
 **/

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "lexical_analyzer.h"

/**
 * Creates variable definition
 * @param frame frame of declared variable
 * @param varname name of declared variable
 * @param id id of declared variable (default 0)
 * 
*/
void code_generator_defvar(char *frame, char *varname, unsigned id);

/**
 * Checks if function needs new data frame
 * @param name name of function
 * @return bool
*/
bool code_generator_need_function_frame(char* name);

/**
 * Creates prolog of program (needed header)
*/
void code_generator_prolog();

/**
 * Creates header of if
 * @pre On stack has to be value (bool) of condition
 * @param id global unique id of if
*/
void code_generator_if_header(unsigned id);

/**
 * Creates else branch for if
 * @param id global unique id of if
*/
void code_generator_if_else(unsigned id);

/**
 * Closes branches if and else
 * @param id global unique id of if
*/
void code_generator_if_end(unsigned id);

/**
 * Token version of code_generator_var_assign
 * @param token token of var identifier
*/
void code_generator_var_assign_token(token_T token);

/**
 * Sets value from stack to variable
 * @param var name of variable
*/
void code_generator_var_assign(char* var);

/**
 * Token version of code_generator_var_declare
 * @param token token of var identifier
*/
void code_generator_var_declare_token(token_T token);

/**
 * Declares and sets value from stack to variable (pop)
 * @param var name of variable
*/
void code_generator_var_declare(char* variable);

/**
 * Creates eof label
*/
void code_generator_eof();

/**
 * Pushs value of token to stack
 * @param token token to push (identifier/string/int/double)
*/
void code_generator_push(token_T token);

/**
 * Closes for loop
 * @param id global unique id of for
*/
void code_generator_for_loop_end(unsigned id);

/**
 * Creates header of if for for loop
 * @pre On stack has to be value (bool) of condition
 * @param id global unique id of for
*/
void code_generator_for_loop_if(unsigned id);

/**
 * Creates label of for if calculation
 * @param id global unique id of for
*/
void code_generator_for_label(unsigned id);

/**
 * Creates body of for
 * @param id global unique id of for
*/
void code_generator_for_body(unsigned id);

/**
 * Prints value of token in assembler
 * @param token token to print (identifier/string/int/double)
*/
void code_generator_print_value(token_T token);

/**
 * Pushs data frame
*/
void code_generator_pushframe();

/**
 * Creates data frame
*/
void code_generator_createframe();

/**
 * Pops data frame
*/
void code_generator_popframe();

/**
 * Creates operations on stack
 * @param operator token type of operator
 * @param is_int does operation in ints
*/
void code_generator_operations(token_type_T operator, bool is_int);

/**
 * Creates operation concat on stack
*/
void code_generator_concats();

/**
 * Clears stact
*/
void code_generator_clears();

/**
 * Token version of code_generator_function_call
*/
void code_generator_function_call_token(token_T token);

/**
 * Creates call of function
 * @param name name of function
*/
void code_generator_function_call(char* name);

/**
 * Token version of code_generator_function_call_param_add
 * @param token_name
 * @param token_param_name
 * @param token_value
*/
void code_generator_function_call_param_add_token(token_T token_name, token_T token_param_name, token_T token_value);

/**
 * Add parameter to future called function
 * @pre Before adding the first parameter has to be called code_generator_createframe()
 * @param name name of function
 * @param param_name name of parameter
 * @param value value of parameter
*/
void code_generator_function_call_param_add(char* name, char* param_name, token_T value);

/**
 * Token version of code_generator_function_label
 * @param token
*/
void code_generator_function_label_token(token_T token);

/**
 * Creates label of function (declaration of header)
 * @param name name of function
*/
void code_generator_function_label(char* name);

/**
 * Closes function
 * @param name name of function
*/
void code_generator_function_end(char* name);

/**
 * Creates return from function
 * @param token return value (identifier/string/int/double)
*/
void code_generator_return(token_T token);

#endif