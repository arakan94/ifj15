/******** semantic.h *********
*
* FIT VUT, IFJ 089
* Author: Šimon Urban, xurban63
* Summary: Semantic - header file
*
*/

#ifndef SEMANTIC_INCLUDED
#define SEMANTIC_INCLUDED

	#include "sym_tab.h"

	//initialize Global Symbol Table
	int initialize_global_symbol_table();

	//start new function - CALL before saving parameters!
	int add_func(Tdata_type data_type, char *id);

	//add new parameter into function parameter stack
	int add_param(Tdata_type data_type, char *id);
	//return data type of nth parameter of given function
	Tlocal_sym_tab_node *get_nth_param(Tglobal_sym_tab_node *func_address, int n);

	//add function declaration to global symbol table
	int func_declaration();
	//add function definition to global symbol table
	int func_definition(unsigned function_start);

	//add variable into local symbol table of current block
	int add_var(Tdata_type data_type, char *id);
	//return true, if var exists in current part of function
	bool var_exists(char *id);
	//return true, if var is defined
	bool is_var_defined(char *id);
	//return variable data type
	Tdata_type get_var_type(char *id);
	//return variable node address
	Tlocal_sym_tab_node *get_var(char *id);
	//set int variable value
	int set_var_value_int(char* id, int value);
	//set double variable value
	int set_var_value_double(char* id, double value);
	//set string variable value
	int set_var_value_string(char* id, char *value);

	//compare two data types
	int type_compare(Tdata_type type1, Tdata_type type2);
	//compare id and expression data types when assigning value
	int assign_type_compare(char *id, Tdata_type data_type);

	//return true, if function is in global symbol table
	bool func_exists(char *id);
	//return true, if function is defined
	bool is_func_defined(char *id);
	//return function data type
	Tdata_type get_func_type(char *id);
	//return function node address
	Tglobal_sym_tab_node *get_func(char *id);

	//add new local symbol table to local symbol table stack
	int add_block();
	//remove local symbol table from top of stack
	int remove_block();

	//check if every function in global symbol table is defined
	int every_func_defined();

	//free global symbol table including all data
	void free_global_symbol_table();

#endif	//SEMANTIC_INCLUDED
