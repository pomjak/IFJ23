/**
 * @name IFJ23
 * @file code_generator.c
 * @brief Code generator
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 17.11.2023
 **/

#include "code_generator.h"
#include <stdio.h>

unsigned tmp_var_id = 0;

void code_generator_defvar(char *frame, char *varname, unsigned id){
    printf("\nDEFVAR %s@%s_%d\n",frame,varname, id);
}

void code_generator_prolog(){
	printf(".IFJcode23\n");
	code_generator_defvar("GF", "PARAM", 1);
	code_generator_defvar("GF", "PARAM", 2);
	code_generator_defvar("GF", "RESULT", 1);
}

void code_generator_if_header(unsigned id){
    printf("\nPUSHS bool@true\n");
    printf("JUMPIFNEQS $$ELSE_%u\n", id);
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

void code_generator_var_declare(char* variable){
	code_generator_defvar("LF",variable,0);
    printf("POPS LF@%s_%d\n", variable, 0);
}

void code_generator_eof(){
	printf("\nLABEL $$EOF\n");
}

void code_generator_for_loop_end(unsigned id){
    printf("\nJUMP $$FOR_%u\n",id);
    printf("LABEL $$FOR_END_%u\n",id);
}

void code_generator_for_loop_if(unsigned id){
	printf("\nPUSHS bool@true\n");
    printf("JUMPIFNEQS $$FOR_END_%u\n", id);
	printf("JUMP $$FOR_BODY_%u\n", id);
}

void code_generator_for_label(unsigned id){
	printf("\nLABEL $$FOR_%u\n", id);
}

void code_generator_for_body(unsigned id){
	printf("\nLABEL $$FOR_BODY_%u\n", id);
}

void code_generator_print_value(token_T token){    

    if(token.type == TOKEN_IDENTIFIER){
        printf("\nLF@%s_%d",token.value.string_val.str, 0);
    } else if (token.type == TOKEN_INT) {
			printf("\nint@%ld",token.value.int_val);
		} else if (token.type == TOKEN_DBL) {
			printf("\nfloat@%a",token.value.double_val);
		} else if (token.type == TOKEN_STRING) {
			printf("\nstring@");
            int token_length = strlen(token.value.string_val.str);

            for(int i = 0; i < token_length; i++){
                char c = token.value.string_val.str[i];
                if((c >= 0 && c <= 32) || c == '#' || c == '\\'){
                    printf("\\%03d",c);
                }
                else{
                    printf("%c", c);
                }
            }
		}
}

void code_generator_pushframe(){
    printf("\nPUSHFRAME\n");
}

void code_generator_popframe(){
    printf("\nPOPFRAME\n");
}

void code_generator_createframe(){
    printf("\nCREATEFRAME\n");
}

void code_generator_push(token_T token){
    if(token.type != TOKEN_IDENTIFIER &&
       token.type != TOKEN_INT &&
       token.type != TOKEN_DBL &&
       token.type != TOKEN_STRING) { 
		return; 
	}
       
    printf("\nPUSHS ");
    code_generator_print_value(token);
    printf("\n");
}

void code_generator_adds(){
    printf("\nADDS\n");
}

void code_generator_subs(){
    printf("\nSUBS\n");
}

void code_generator_muls(){
    printf("\nMULS\n");
}

void code_generator_divs(){
    printf("\nDIVS\n");
}

void code_generator_idivs(){
    printf("\nIDIVS\n");
}

void code_generator_concats(){
    //POPS PARAM_2
    printf("\nPOPS GF@PARAM_2\n");

    //POPS PARAM_1
    printf("POPS GF@PARAM_1\n");

    //CONCAT: RESULT = PARAM_1 + PARAM_2
    printf("CONCAT GF@RESULT_1 GF@PARAM_1 GF@PARAM_2\n");

    //PUSHS RESULT
    printf("PUSHS GF@RESULT_1\n");
}

void code_generator_clears(){
    printf("\nCLEARS\n");
}

void code_generator_function_call(char* name){

        if(strcmp(name,"readString") == 0){
            printf("READ GF@READED_1 string");
            printf("PUSHS GF@READED_1");
        }
        else if(strcmp(name,"readInt") == 0){
            printf("READ GF@READED_2 int");
            printf("PUSHS GF@READED_2");
        }
        else if((strcmp(name,"readDouble") == 0)){
            printf("READ GF@READED_3 float");
            printf("PUSHS GF@READED_3");
        }
        else if((strcmp(name,"write") == 0)){
            
           //TODO
        }
} 
