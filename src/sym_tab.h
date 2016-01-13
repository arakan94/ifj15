/******** sym_tab.h *********
*
* FIT VUT, IFJ 089
* Author: Šimon Urban, xurban63
* Summary: Symbol table - header file
*
*/

#ifndef SYM_TAB_INCLUDED
#define SYM_TAB_INCLUDED

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdbool.h>
	#include "ial.h"
	#include "stack.h"
	#include "error_codes.h"

	#define FUNC_START_UNDEF 0

	/*---Type definition---*/
	typedef enum data_types{
		TYPE_NON,
		TYPE_INT,			//int
		TYPE_DOUBLE,		//double
		TYPE_STRING,		//string
		TYPE_AUTO,			//auto
	} Tdata_type;		//data types

	typedef enum type_compare_errors {
		SAME_DATA_TYPES,		//same data types - ok
		IMPLICIT_CONVERSION,	//implicit conversion - ok
		INCOMPATIBLE_TYPES,		//incompatible data types - error
	} Tcompare;				//data types comparison result

	typedef enum sym_tab_errors{
		OK_ST,						//ok - symbol table
		ERR_ST_DIFF_FUNC_HEADER,	//error - symbol table - different function header
		ERR_ST_NODEF,				//error - symbol table - no definition
		ERR_ST_REDEF,				//error - symbol table - redefinition
		ERR_ST_OTHER,				//error - symbol table - other
		ERR_ST_99,					//error - symbol table - interpret error
	} Tsym_tab_error;			//symbol table error

	typedef struct{
		unsigned func_start;				//start of function
		Tdata_type data_type;		//return data typest_global_add_definition
		bool defined;				//is declared/defined?
		Tstack *local_sym_table;	//stack of TBinTree *
		Tstack *parameters;			//stack of Tparameter *
		unsigned var_count;			//number of variables in function
	} Tglobal_sym_tab_node;		//Functions

	typedef struct{
		Tdata_type data_type;		//variable data type
		bool defined;				//is defined?
		char *name;					//pointer to key - variable identifier
		unsigned offset;			//variable offset - unique value
		union {
			int int_val;			//int value
			double double_val;		//double value
			char *string_val;		//string value
		};
	} Tlocal_sym_tab_node;		//Variables

	typedef struct{
		char *name;					//parameter id
		Tdata_type data_type;		//parameter data type
	} Tparameter;				//Parameters

	/*---Global variables---*/
	TBinTree *global_sym_table;					//global symbol table
	Tglobal_sym_tab_node *temp_global_node;		//information about new function
	char *temp_global_node_name;				//identifier of new function
	Tglobal_sym_tab_node *current_func_data;	//pointer to currently processed function in global symbol table
	Tstack *all_local_symbol_tables;			//stack of all local symbol tables

	/*---Functions---*/

	Tsym_tab_error st_check_function_definitions();						//check if every declared function and main() is defined
	Tparameter *st_find_param(char *name, Tstack *parameter_stack);		//find parameter in given parameter stack
	Tlocal_sym_tab_node *st_get_nth_param(Tglobal_sym_tab_node *func_address, int n);		//return n-th parameter of given function
	Tsym_tab_error st_copy_params_into_table(Tglobal_sym_tab_node *global_node);	//copy parameters into local symbol table

	bool st_can_use_id(char *name, Tglobal_sym_tab_node *global_node);	//search for identifier on top of local symbol table stack, in parameters and in global symbol table
	Tcompare st_cmp_types(Tdata_type type1, Tdata_type type2);			//compare two data types

	//---Global symbol table functions
	//-most functions use global variables
	Tsym_tab_error st_init_global_sym_table();	//initialize global symbol table
	void st_free_global_sym_table();			//free global symbol table - including all nodes data

	Tsym_tab_error st_global_add_declaration();							//add temp_global_node into global symbol table as declaration
	Tsym_tab_error st_global_add_definition(unsigned function_start);	//add temp_global_node into global symbol table as definition

	Tdata_type st_get_function_type(char *name);//return return_type of function
	bool st_is_function_defined(char *name);	//check if function is defined
	TSymTable *st_find_func(char *name);		//search for function in global symbol table


	//---Local symbol table functions
	void st_free_local_sym_table();			//free all local symbol table of stack given by parameter (including data)

	Tsym_tab_error st_local_add_id(char *name, Tdata_type data_type, Tglobal_sym_tab_node *global_node);	//add new variable into local symbol table on top of local symbol table stack
	Tdata_type st_get_variable_type(char *name, Tglobal_sym_tab_node *global_node);				//return variable data type
	bool st_is_variable_defined(char *name, Tglobal_sym_tab_node *global_node);					//check if the variable is defined
	TSymTable *st_find_var(char *name, Tstack *sym_table_stack);								//search for identifier in given local symbol table stack

	Tsym_tab_error st_add_local_block(Tstack *sym_table_stack);				//add new local symbol table on top of local symbol table stack
	Tsym_tab_error st_remove_local_block(Tstack *sym_table_stack);			//remove local symbol table on top of local symbol table stack


#endif	//SYM_TAB_INCLUDED
