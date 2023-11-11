/**
 * @name IFJ23
 * @file error.h
 * @brief Definitions of error codes
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @author 
 * @date 08.10.2023
 **/

#ifndef ERROR_H
#define ERROR_H

#define ERR_NO_ERR 0
#define ERR_LEXICAL 1
#define ERR_SYNTAX 2
#define ERR_UNDEFINED_FUNCTION 3
#define ERR_VAR_REDEFINE 3
#define ERR_FUNCTION_PARAMETER 4  //wrong function parameter type, number of prameter, wrong return type
#define ERR_UNDEFINED_VARIABLE 5
#define ERR_FUNCTION_RETURN 6 // missing/useless  expression
#define ERR_UNCOMPATIBILE_TYPE 7
#define ERR_MISSING_TYPE 8
#define ERR_SEMANTIC_ERR 9
#define ERR_INTERNAL 99

/**
 * @brief Prints error and message to stderr
 * 
 */
void print_error(int error_code, const char * msg, ...);

#endif
