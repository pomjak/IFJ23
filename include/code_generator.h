/**
 * @name IFJ23
 * @file code_generator.h
 * @brief Code generator header
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 12.11.2023
 **/

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "lexical_analyzer.h"

void code_generator_defvar(char *frame, char *varname, unsigned id);

void code_generator_prolog();

void code_generator_if_header(token_type_T operator, unsigned id);

void code_generator_if_else(unsigned id);

void code_generator_if_end(unsigned id);

void code_generator_var_assign(char* var);

#endif