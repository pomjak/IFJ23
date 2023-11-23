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
#define ERR_LEXICAL 1               /*Lexical analysis error*/
#define ERR_SYNTAX 2                /*Invalid input program syntax*/
#define ERR_UNDEFINED_FUNCTION 3    /*Reference to an undefined function*/
#define ERR_REDEFINING_VARIABLE 3   /*Attempting to redefine a variable*/
#define ERR_RETURN_TYPE 4           /*Invalid function return type*/
#define ERR_FUNCTION_PARAMETER 4    /*Invalid number or type of parameters*/
#define ERR_UNDEFINED_VARIABLE 5    /*Undefined or uninitialized variable*/
#define ERR_FUNCTION_RETURN 6       /*Extra or missing expression in function return*/
#define ERR_INCOMPATIBILE_TYPE 7    /*Incompatible types of expression operands*/
#define ERR_MISSING_TYPE 8          /*Var or parameter type not specified and unable to implicitly define*/
#define ERR_SEMANTIC 9              /*General sematic error*/
#define ERR_INTERNAL 99             /*Internal program error*/

/**
 * @brief Prints error and message to stderr
 * 
 */
void print_error(int error_code, const char * msg, ...);

#endif
