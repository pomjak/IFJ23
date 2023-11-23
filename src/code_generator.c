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
unsigned func_param_id = 0;

void code_generator_defvar(char *frame, char *varname, unsigned id){
    printf("\nDEFVAR %s@%s_%d\n",frame,varname, id);
}

bool code_generator_need_function_frame(char* name) {
    const char* no_frame_funcions[] = {"readString", "readInt", "readDouble", "write", "Int2Double", "Double2Int", "length", "chr"};
    const unsigned no_frame_funcions_count = 8;

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
    code_generator_defvar("GF", "INT2CHAR", 1);
    code_generator_createframe();
    code_generator_pushframe();
    code_generator_function_ord();
    code_generator_substring();
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
        code_generator_defvar("LF","TMP", tmp_var_id);
		printf("POPS LF@TMP_%u\n", tmp_var_id);
		tmp_var_id++;
	}
}

void code_generator_var_declare_token(token_T token){
    code_generator_var_declare(token.value.string_val.str);
}

void code_generator_var_declare(char* variable){
	code_generator_defvar("LF", variable, 0);
    printf("POPS LF@%s_%d\n", variable, 0);
}

void code_generator_copy_var_to_new_frame(char* variable){
    code_generator_defvar("TF", variable, 0);
    printf("MOVE TF@%s_%d LF@%s_%d\n", variable, 0, variable, 0);
}

void code_generator_eof(){
	printf("\nLABEL $$EOF\n");
}

void code_generator_push(token_T token){
    if(token.type != TOKEN_IDENTIFIER &&
       token.type != TOKEN_NIL &&
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
    } else if (token.type == TOKEN_NIL) {
	    printf("nil@nil");
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

    func_param_id = 0;

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

void code_generator_function_call_param_add_token(token_T token_name, token_T token_value){
    code_generator_function_call_param_add(token_name.value.string_val.str, token_value);
}

void code_generator_function_call_param_add(char* name, token_T token){

    if(code_generator_need_function_frame(name)) {
        if (func_param_id == 0) {
            code_generator_createframe();
        }

        code_generator_defvar("TF", "??", func_param_id);
        printf("MOVE TF@??_%d ", func_param_id);
        code_generator_print_value(token);
        printf("\n");

        func_param_id++;
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
    } else if(strcmp(name, "chr") == 0){
        printf("INT2CHAR GF@INT2CHAR_1 ");
        code_generator_print_value(token);
        printf("\n");
        printf("PUSHS GF@INT2CHAR_1\n");
    }
}

void code_generator_function_ord(){
    code_generator_function_label("ord");

    code_generator_defvar("LF", "length", 0);
    printf("STRLEN LF@length_0 LF@??_0\n");

    printf("JUMPIFNEQ ORD_NOT0 int@0 LF@length_0\n");
    printf("PUSHS int@0\n");
    code_generator_return();

    printf("LABEL ORD_NOT0\n");
    code_generator_defvar("LF", "ord_value", 0);
    printf("STRI2INT LF@ord_value_0 LF@??_0 int@0\n");
    printf("PUSHS LF@ord_value_0\n");

    code_generator_function_end("ord");
}

void code_generator_substring(){
    code_generator_function_label("substring");

    code_generator_defvar("LF", "length", 0);
    printf("STRLEN LF@length_0 LF@??_0\n");

    code_generator_defvar("LF", "condition", 0);

    printf("LT LF@condition_0 LF@??_1 int@0\n");
    printf("JUMPIFEQ SUSTRING_nil LF@condition_0 bool@true\n");

    printf("LT LF@condition_0 LF@??_2 int@0\n");
    printf("JUMPIFEQ SUSTRING_nil LF@condition_0 bool@true\n");

    printf("GT LF@condition_0 LF@??_1 LF@??_2\n");
    printf("JUMPIFEQ SUSTRING_nil LF@condition_0 bool@true\n");

    printf("LT LF@condition_0 LF@??_1 LF@length_0\n");
    printf("JUMPIFNEQ SUSTRING_nil LF@condition_0 bool@true\n");

    printf("GT LF@condition_0 LF@??_2 LF@length_0\n");
    printf("JUMPIFEQ SUSTRING_nil LF@condition_0 bool@true\n");

    code_generator_defvar("LF", "result", 0);
    code_generator_defvar("LF", "char", 0);

    printf("MOVE LF@result_0 string@\n");

    printf("LABEL SUSTRING_loop\n");

    printf("LT LF@condition_0 LF@??_1 LF@??_2\n");
    printf("JUMPIFNEQ SUSTRING_loop_end LF@condition_0 bool@true\n");

    printf("GETCHAR LF@char_0 LF@??_0 LF@??_1\n");
    printf("CONCAT LF@result_0 LF@result_0 LF@char_0\n");
    printf("ADD LF@??_1 int@1 LF@??_1\n");

    printf("JUMP SUSTRING_loop\n");
    printf("LABEL SUSTRING_loop_end\n");

    printf("PUSHS LF@result_0\n");
    code_generator_return();

    printf("LABEL SUSTRING_nil\n");
    printf("PUSHS nil@nil\n");

    code_generator_function_end("substring");
}

void code_generator_function_label_token(token_T token){
    code_generator_function_label(token.value.string_val.str);
}

void code_generator_function_label(char* name){
    printf("\nJUMP $$FUNCTION_END_%s\n", name);
    printf("\nLABEL $$FUNCTION_%s\n", name);
    code_generator_pushframe();
}

void code_generator_param_map(char *param_name, unsigned param_id){
    code_generator_defvar("LF", param_name, 0);
    printf("MOVE LF@%s_%d LF@??_%d\n", param_name, 0, param_id);
}

void code_generator_function_end(char* name){
    code_generator_popframe();
    printf("RETURN\n");
    printf("\nLABEL $$FUNCTION_END_%s\n", name);
}

void code_generator_return(){
    code_generator_popframe();
    printf("RETURN\n");
}