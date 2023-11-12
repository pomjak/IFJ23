/**
 * @name IFJ23
 * @file code_generator.c
 * @brief Code generator
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 12.11.2023
 **/

#include "code_generator.h"
#include <stdio.h>

unsigned tmp_var_id = 0;

void code_generator_defvar(char *frame, char *varname, unsigned id){
    printf("DEFVAR %s@%s_%d\n",frame,varname, id);
}

void code_generator_prolog(){
	printf(".IFJcode23\n");
}

void code_generator_if_header(token_type_T operator, unsigned id){

    if(operator == TOKEN_EQ) {
        printf("\nJUMPIFNEQS $$ELSE_%u\n", id);
    } else if (operator == TOKEN_NEQ) {
        printf("\nJUMPIFEQS $$ELSE_%u\n", id);
    } else if (operator == TOKEN_LT) {
        printf("\nLTS\n");
		printf("PUSHS bool@true\n");
		printf("JUMPIFNEQS $$ELSE_%u\n", id);
    } else if (operator == TOKEN_LEQ) {
        printf("\nGTS\n");
	    printf("PUSHS bool@true\n");
		printf("JUMPIFEQS $$ELSE_%u\n", id);
    } else if (operator == TOKEN_GT) {
        printf("\nGTS\n");
		printf("PUSHS bool@true\n");
		printf("JUMPIFNEQS $$ELSE_%u\n", id);
    } else if (operator == TOKEN_GEQ) {
        printf("\nLTS\n");
		printf("PUSHS bool@true\n");
		printf("JUMPIFEQS $$ELSE_%u\n", id);
    }
}

void code_generator_if_else(unsigned id){
	printf("\nJUMP $$IF_END_%u\n", id);
	printf("LABEL $$ELSE_%u\n", id);
}

void code_generator_if_end(unsigned id){
	printf("\nLABEL $$IF_END_%u\n", id);
}

void code_generator_var_assign(char* var){
    
	if(strcmp(var, "_") != 0){
		printf("\nPOPS LF@%s\n", var);
	} else{
        code_generator_defvar("LF","TMP",tmp_var_id);
		printf("POPS LF@TMP_%u\n", tmp_var_id);
		tmp_var_id++;
	}
}