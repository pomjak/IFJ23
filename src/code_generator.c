/**
 * @name IFJ23
 * @file code_generator.c
 * @brief Code generator
 * @author Marie Kolarikova <xkolar77@stud.fit.vutbr.cz>
 * @date 19.11.2023
 **/

#include "code_generator.h"
#include <stdio.h>

unsigned tmp_var_id = 0;

void code_generator_defvar(char *frame, char *varname, unsigned id){
    printf("\nDEFVAR %s@%s_%d\n",frame,varname, id);
}

bool code_generator_need_function_frame(char* name) {
    const char* no_frame_funcions[] = {"readString", "readInt", "readDouble", "write", "Int2Double", "Double2Int", "length"};
    const unsigned no_frame_funcions_count = 7;

    for(unsigned i = 0; i < no_frame_funcions_count; i++){
        if(strcmp(name, no_frame_funcions[i]) == 0) {
            return false;  
        }
    }

    return true;

}

void code_generator_prolog(){
	printf(".IFJcode23\n");
	code_generator_defvar("GF", "PARAM", 1);
	code_generator_defvar("GF", "PARAM", 2);
	code_generator_defvar("GF", "RESULT", 1);
    code_generator_defvar("GF", "LENGTH", 1);
    code_generator_defvar("GF", "READED", 1);
    code_generator_defvar("GF", "READED", 2);
    code_generator_defvar("GF", "READED", 3);
    code_generator_createframe();
    code_generator_pushframe();
}

/**
 * If
 */

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

/**
 * Variable
 */

void code_generator_var_assign_token(token_T token){
    code_generator_var_assign(token.value.string_val.str);
}

void code_generator_var_assign(char* var){
    
	if(strcmp(var, "_") != 0){
		printf("\nPOPS LF@%s_%d\n", var, 0);
	} else{
        code_generator_defvar("LF","TMP",tmp_var_id);
		printf("POPS LF@TMP_%u\n", tmp_var_id);
		tmp_var_id++;
	}
}

void code_generator_var_declare_token(token_T token){
    code_generator_var_declare(token.value.string_val.str);
}

void code_generator_var_declare(char* variable){
	code_generator_defvar("LF",variable,0);
    printf("POPS LF@%s_%d\n", variable, 0);
}

void code_generator_eof(){
	printf("\nLABEL $$EOF\n");
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

/**
 * For loop
 */

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

/**
 * Value
 */

void code_generator_print_value(token_T token){

    if(token.type == TOKEN_IDENTIFIER){
        printf("LF@%s_%d",token.value.string_val.str, 0);
    } else if (token.type == TOKEN_INT) {
	    printf("int@%d",token.value.int_val);
	} else if (token.type == TOKEN_DBL) {
		printf("float@%a",token.value.double_val);
	} else if (token.type == TOKEN_STRING) {
		printf("string@");
        int token_length = strlen(token.value.string_val.str);

        for(int i = 0; i < token_length; i++){
            char c = token.value.string_val.str[i];
            if((c >= 0 && c <= 32) || c == '#' || c == '\\'){
                printf("\\%03d",c);
            } else{
                printf("%c", c);
            }
        }
	}
}

/**
 * Frame
 */

void code_generator_pushframe(){
    printf("\nPUSHFRAME\n");
}

void code_generator_popframe(){
    printf("\nPOPFRAME\n");
}

void code_generator_createframe(){
    printf("\nCREATEFRAME\n");
}

/**
 * Operations
 */

void code_generator_operations(token_type_T operator, bool is_int){

    if(operator == TOKEN_ADD) {
        printf("\nADDS\n");
    } else if (operator == TOKEN_SUB) {
        printf("\nSUBS\n");
    } else if (operator == TOKEN_MUL) {
        printf("\nMULS\n");
    } else if (operator == TOKEN_DIV && !is_int) {
        printf("\nDIVS\n");
    } else if (operator == TOKEN_DIV && is_int) {
        printf("\nIDIVS\n");
    } else if (operator == TOKEN_LT) {
        printf("\nLTS\n");
    } else if (operator == TOKEN_LEQ) {
        printf("\nGTS\n");
        printf("\nNOTS\n");
    } else if (operator == TOKEN_GT) {
        printf("\nGTS\n");
    } else if (operator == TOKEN_GEQ) {
        printf("\nLTS\n");
        printf("\nNOTS\n");
    } else if (operator == TOKEN_EQ) {
        printf("\nEQS\n");
    } else if (operator == TOKEN_NEQ) {
        printf("\nEQS\n");
        printf("\nNOTS\n");
    }
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

/**
 * Function
 */

void code_generator_function_call_token(token_T token){
    code_generator_function_call(token.value.string_val.str);
}

void code_generator_function_call(char* name){

    if(strcmp(name,"readString") == 0){
        printf("READ GF@READED_1 string\n");
        printf("PUSHS GF@READED_1\n");
    } else if(strcmp(name,"readInt") == 0){
        printf("READ GF@READED_2 int\n");
        printf("PUSHS GF@READED_2\n");
    } else if((strcmp(name,"readDouble") == 0)){
        printf("READ GF@READED_3 float\n");
        printf("PUSHS GF@READED_3\n");
    } else if (code_generator_need_function_frame(name)) {
        printf("CALL $$FUNCTION_%s\n", name);
    }
}

void code_generator_function_call_param_add_token(token_T token_name, token_T token_param_name, token_T token_value){
    code_generator_function_call_param_add(token_name.value.string_val.str, token_param_name.value.string_val.str, token_value);
}

void code_generator_function_call_param_add(char* name, char* param_name, token_T token){

    if(code_generator_need_function_frame(name)) {
        code_generator_defvar("TF", param_name, 0);
        printf("MOVE TF@%s_%d ",param_name, 0);
        code_generator_print_value(token);
        printf("\n");
    }

    if(strcmp(name,"write") == 0){
        printf("WRITE ");
        code_generator_print_value(token);
        printf("\n");
    } else if(strcmp(name,"Int2Double") == 0){
        code_generator_push(token);
        printf("INT2FLOATS\n");  
    } else if((strcmp(name,"Double2Int") == 0)){
        code_generator_push(token);
        printf("FLOAT2INTS\n");
    } else if((strcmp(name,"length") == 0)){
        printf("STRLEN GF@LENGTH_1 ");
        code_generator_print_value(token);
        printf("\n");
        printf("PUSHS GF@LENGTH_1\n");
    }
}

void code_generator_function_label_token(token_T token){
    code_generator_function_label(token.value.string_val.str);
}

void code_generator_function_label(char* name){
    printf("\nJUMP $$FUNCTION_END_%s\n", name);
    printf("\nLABEL $$FUNCTION_%s\n", name);
    code_generator_pushframe();
}

void code_generator_function_end(char* name){
    code_generator_popframe();
    printf("RETURN\n");
    printf("\nLABEL $$FUNCTION_END_%s\n", name);
}

void code_generator_return(token_T token){
    code_generator_push(token);
    code_generator_popframe();
    printf("RETURN\n");
}