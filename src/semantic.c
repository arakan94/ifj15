/******** semantic.c *********
*
* FIT VUT, IFJ 089
* Author: Šimon Urban, xurban63
* Summary: Semantic
*
*/

#include "semantic.h"
#include "functions.h"

//add all built-in functions + main() declaration into global symbol table
static int insert_built_in_func()
{
	int temp = ERR_SEMANTIC;

	//add main() function declaration
	if ((temp = add_func(TYPE_INT, "main")) != OK)
	{
		return temp;
	}
	if ((temp = func_declaration()) != OK)
	{
		return temp;
	}
	//add int length(string s)
	if ((temp = add_func(TYPE_INT, "length")) != OK)
	{
		return temp;
	}
	if ((temp = add_param(TYPE_STRING, "s")) != OK)
	{
		return temp;
	}
	if ((temp = func_definition(BIN_LENGTH)) != OK)
	{
		return temp;
	}
	if ((temp = remove_block()) != OK)
	{
		return temp;
	}
	//add string substr(string s , int i , int n)
	if ((temp = add_func(TYPE_STRING, "substr")) != OK)
	{
		return temp;
	}
	if ((temp = add_param(TYPE_STRING, "s")) != OK)
	{
		return temp;
	}
	if ((temp = add_param(TYPE_INT, "i")) != OK)
	{
		return temp;
	}
	if ((temp = add_param(TYPE_INT, "n")) != OK)
	{
		return temp;
	}
	if ((temp = func_definition(BIN_SUBSTR)) != OK)
	{
		return temp;
	}
	if ((temp = remove_block()) != OK)
	{
		return temp;
	}
	//add string concat(string s1 , string s2)
	if ((temp = add_func(TYPE_STRING, "concat")) != OK)
	{
		return temp;
	}
	if ((temp = add_param(TYPE_STRING, "s1")) != OK)
	{
		return temp;
	}
	if ((temp = add_param(TYPE_STRING, "s2")) != OK)
	{
		return temp;
	}
	if ((temp = func_definition(BIN_CONCAT)) != OK)
	{
		return temp;
	}
	if ((temp = remove_block()) != OK)
	{
		return temp;
	}
	//add int find(string s , string search)
	if ((temp = add_func(TYPE_INT, "find")) != OK)
	{
		return temp;
	}
	if ((temp = add_param(TYPE_STRING, "s")) != OK)
	{
		return temp;
	}
	if ((temp = add_param(TYPE_STRING, "search")) != OK)
	{
		return temp;
	}
	if ((temp = func_definition(BIN_FIND)) != OK)
	{
		return temp;
	}
	if ((temp = remove_block()) != OK)
	{
		return temp;
	}
	//add string sort(string s)
	if ((temp = add_func(TYPE_STRING, "sort")) != OK)
	{
		return temp;
	}
	if ((temp = add_param(TYPE_STRING, "s")) != OK)
	{
		return temp;
	}
	if ((temp = func_definition(BIN_SORT)) != OK)
	{
		return temp;
	}
	if ((temp = remove_block()) != OK)
	{
		return temp;
	}
	return temp;
}

//initialize Global Symbol Table
//-return ERR_INTERPRET if malloc fail
int initialize_global_symbol_table()
{
	//initialize global variables
	global_sym_table = NULL;
	temp_global_node = NULL;
	temp_global_node_name = NULL;
	current_func_data = NULL;
	all_local_symbol_tables = NULL;
	//try initialize global symbol table
	if (st_init_global_sym_table() != OK_ST)
	{
		//initialization failed
		return ERR_INTERPRET;
	}
	else
	{
		//init ok
		return insert_built_in_func();
	}
}

//start new function
//-working with global variable
//-function not in global symbol table yet
//-return ERR_INTERPRET if allocation fails
int add_func(Tdata_type data_type, char *id)
{
	if (temp_global_node != NULL)
	{
		if (current_func_data == NULL)
		{
			//ok - new function outside of function
			//set new function data
			temp_global_node->data_type = data_type;

			free(temp_global_node_name);	//free previous function name
			temp_global_node_name = NULL;
			temp_global_node_name = malloc(strlen(id) + 1);
			if (temp_global_node_name == NULL)
			{
				//can't allocate memory
				return ERR_INTERPRET;
			}
			else
			{
				strcpy(temp_global_node_name, id);	//copy identifier into temp_global_node_name

				temp_global_node->parameters = stack_init();	//initialize stack for parameters
				if (temp_global_node->parameters == NULL)
				{
					//stack_init() failed
					free(temp_global_node_name);
					return ERR_INTERPRET;
				}
				else
				{
					return OK;
				}
			}
		}
		else
		{
			//new function found inside of another function
			fprintf(stderr, "New function header inside function definition.\n");
			return ERR_SEMANTIC;
		}
	}
	else
	{
		//called without initialized global symbol table
		fprintf(stderr, "Global symbol table not initialized.\n");
		return ERR_SEMANTIC;
	}
}

//add new parameter into function parameter stack
//-can add parameter with id already used by another parameter (check later)
//-return ERR_INTERPRET if malloc fails
int add_param(Tdata_type data_type, char *id)
{
	if (temp_global_node != NULL)
	{
		if (temp_global_node->parameters != NULL)
		{
			TSymTable *temp_func = st_find_func(id);
			if (temp_func == NULL)
			{
				if (strcmp(id, temp_global_node_name) != 0)
				{
					//ok - function of same identifier does not exist
					//allocate memory for new parameter
					Tparameter *temp = malloc(sizeof(Tparameter));
					if (temp == NULL)
					{
						//can't allocate memory
						return ERR_INTERPRET;
					}
					else
					{
						temp->data_type = data_type;
						temp->name = malloc(strlen(id) + 1);
						if (temp->name == NULL)
						{
							//can't allocate memory
							free(temp);
							return ERR_INTERPRET;
						}
						else
						{
							strcpy(temp->name, id);	//copy id into parameter name
							//push parameter address on top of stack
							Tstack_error temp_stack_err = stack_push(temp_global_node->parameters, temp);
							if (temp_stack_err == OK_STACK)
							{
								//new parameter added
								return OK;
							}
							else
							{
								//stack full - can't allocate more memory for stack elements
								return ERR_INTERPRET;
							}
						}
					}
				}
				else
				{
					//redefinition - same current function and parameter identifier
					fprintf(stderr, "Parameter identifier '%s' already used by function.\n", id);
					return ERR_UNDEF;
				}
			}
			else
			{
				//redefinition - identifier already used by function
				fprintf(stderr, "Parameter identifier '%s' already used by function.\n", id);
				return ERR_UNDEF;
			}
		}
		else
		{
			//not in function declaration - can't add parameter
			fprintf(stderr, "Can't add parameter '%s' outside of function.\n", id);
			return ERR_SEMANTIC;
		}
	}
	else
	{
		//called without initialized global symbol table
		fprintf(stderr, "Global symbol table not initialized.\n");
		return ERR_SEMANTIC;
	}
}

//return address of n-th parameter of given function
//-index starts on 1 (first parameter: n = 1, ...)
Tlocal_sym_tab_node *get_nth_param(Tglobal_sym_tab_node *func_address, int n)
{
	Tlocal_sym_tab_node *temp = st_get_nth_param(func_address, n);	//get n-th parameter of function given by parameter
	if (temp == NULL)
	{
		//parameter does not exist
		return NULL;
	}
	else
	{
		//parameter exists - return it's address
		return temp;
	}
}

//add function declaration with data in temp_global_node to global symbol table
//-call when FD -> ;
int func_declaration()
{
	if (temp_global_node != NULL)
	{
		//ok - global symbol table is initialized
		Tsym_tab_error temp = st_global_add_declaration();	//try add function declaration into global symbol table
		//set temp_global_node values to default
		temp_global_node->func_start = FUNC_START_UNDEF;
		temp_global_node->data_type = TYPE_NON;
		temp_global_node->defined = false;
		temp_global_node->local_sym_table = NULL;
		temp_global_node->parameters = NULL;
		//check result of st_global_add_declaration()
		switch (temp)
		{
			case OK_ST:
				//ok - new function added
				return OK;
				break;
			case ERR_ST_DIFF_FUNC_HEADER:
			case ERR_ST_REDEF:
				return ERR_UNDEF;
				break;
			case ERR_ST_99:
				return ERR_INTERPRET;
				break;
			default:
				return ERR_SEMANTIC;
				break;
		}
	}
	else
	{
		//global symbol table not initialized
		fprintf(stderr, "Global symbol table not initialized.\n");
		return ERR_SEMANTIC;
	}
}

//add function definition with data in temp_global_node to global symbol table
//-call when FD -> { STL }
int func_definition(unsigned function_start)
{
	if (temp_global_node != NULL)
	{
		//ok - global symbol table is initialized
		Tsym_tab_error temp = st_global_add_definition(function_start);	//try add function definition into global symbol table
		//set temp_global_node values to default
		temp_global_node->func_start = FUNC_START_UNDEF;
		temp_global_node->data_type = TYPE_NON;
		temp_global_node->defined = false;
		temp_global_node->local_sym_table = NULL;
		temp_global_node->parameters = NULL;
		//check result of st_global_add_definition()
		switch (temp)
		{
			case OK_ST:
				//ok - new function added
				current_func_data = (Tglobal_sym_tab_node *)((st_find_func(temp_global_node_name))->data_ptr);	//actualize current_func_data pointer
				return OK;
				break;
			case ERR_ST_DIFF_FUNC_HEADER:
			case ERR_ST_REDEF:
				return ERR_UNDEF;
				break;
			case ERR_ST_99:
				return ERR_INTERPRET;
				break;
			default:
				return ERR_SEMANTIC;
				break;
		}
	}
	else
	{
		//global symbol table not initialized
		fprintf(stderr, "Global symbol table not initialized.\n");
		return ERR_SEMANTIC;
	}
}

//add variable into local symbol table of current block
//-redefinition cause error
int add_var(Tdata_type data_type, char *id)
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		Tsym_tab_error temp = st_local_add_id(id, data_type, current_func_data);	//try add variable into local symbol table
		//check result of st_local_add_id()
		switch (temp)
		{
			case OK_ST:
				return OK;
				break;
			case ERR_ST_REDEF:
				fprintf(stderr, "Identifier '%s' already used.\n", id);
				return ERR_UNDEF;
				break;
			case ERR_ST_99:
				return ERR_INTERPRET;
				break;
			default:
				return ERR_SEMANTIC;
				break;
		}
	}
	else
	{
		//called outside of function
		fprintf(stderr, "Tried to add variable outside of function.\n");
		return ERR_SEMANTIC;
	}
}

//return true if var exists in current part of function
bool var_exists(char *id)
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		//search for variable in local symbol tables
		if (st_find_var(id, current_func_data->local_sym_table) == NULL)
		{
			//variable not found => does not exist
			return false;
		}
		else
		{
			//variable found => exists
			return true;
		}
	}
	else
	{
		//called outside of function
		return false;
	}
}

//return true if var is defined
bool is_var_defined(char *id)
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		return st_is_variable_defined(id, current_func_data);	//return true if variable exists and if is defined
	}
	else
	{
		//called outside of function
		return false;
	}
}

//return variable data type
Tdata_type get_var_type(char *id)
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		return st_get_variable_type(id, current_func_data);	//return type of variable or TYPE_NON if it does not exist
	}
	else
	{
		//called outside of function
		return TYPE_NON;
	}
}

//return variable node address
Tlocal_sym_tab_node *get_var(char *id)
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		TSymTable *temp = st_find_var(id, current_func_data->local_sym_table);

		if (temp == NULL)
		{
			//identifier not found
			return NULL;
		}
		else
		{
			//identifier found - return pointer to node
			return (Tlocal_sym_tab_node *)(temp->data_ptr);
		}
	}
	else
	{
		//called outside of function
		return NULL;
	}
}

//set int variable value
int set_var_value_int(char* id, int value)
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		TSymTable *temp = st_find_var(id, current_func_data->local_sym_table);	//search for variable in local symbol table stack
		if (temp == NULL)
		{
			//variable not found
			fprintf(stderr, "Variable '%s' does not exist.\n", id);
			return ERR_UNDEF;
		}
		else
		{
			//variable found
			Tlocal_sym_tab_node *var = (Tlocal_sym_tab_node *)(temp->data_ptr);
			var->defined = true;	//setting value of variable => variable is defined
			var->int_val = value;	//set int variable value
			return OK;
		}
	}
	else
	{
		//called outside of function
		return ERR_SEMANTIC;
	}
}

//set double variable value
int set_var_value_double(char* id, double value)
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		TSymTable *temp = st_find_var(id, current_func_data->local_sym_table);	//search for variable in local symbol table stack
		if (temp == NULL)
		{
			//variable not found
			fprintf(stderr, "Variable '%s' does not exist.\n", id);
			return ERR_UNDEF;
		}
		else
		{
			//variable found
			Tlocal_sym_tab_node *var = (Tlocal_sym_tab_node *)(temp->data_ptr);
			var->defined = true;		//setting value of variable => variable is defined
			var->double_val = value;	//set double variable value
			return OK;
		}
	}
	else
	{
		//called outside of function
		return ERR_SEMANTIC;
	}
}

//set string variable value
int set_var_value_string(char* id, char *value)
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		TSymTable *temp = st_find_var(id, current_func_data->local_sym_table);	//search for variable in local symbol table stack
		if (temp == NULL)
		{
			//variable not found
			fprintf(stderr, "Variable '%s' does not exist.\n", id);
			return ERR_UNDEF;
		}
		else
		{
			//variable found
			Tlocal_sym_tab_node *var = (Tlocal_sym_tab_node *)(temp->data_ptr);
			var->defined = true;		//setting value of variable => variable is defined
			var->string_val = value;	//set string variable value
			return OK;
		}
	}
	else
	{
		//called outside of function
		return ERR_SEMANTIC;
	}
}


//compare two data types
int type_compare(Tdata_type type1, Tdata_type type2)
{
	Tcompare temp = st_cmp_types(type1, type2);	//compare two data types given as parameters
	//check result of st_cmp_types()
	switch(temp)
	{
		case SAME_DATA_TYPES:
		case IMPLICIT_CONVERSION:
			return OK;
			break;
		case INCOMPATIBLE_TYPES:
			fprintf(stderr, "Incompatible data types.\n");
			return ERR_TYPE;
			break;
		default:
			return ERR_INTERPRET;
			break;
	}
}

//compare id and expression data types when assigning value
int assign_type_compare(char *id, Tdata_type data_type)
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		TSymTable *temp_sym_tab = st_find_var(id, current_func_data->local_sym_table);	//search for variable in local symbol table
		if (temp_sym_tab == NULL)
		{
			//identifier not found in local symbol tables
			fprintf(stderr, "Variable '%s' does not exist.\n", id);
			return ERR_UNDEF;
		}
		else
		{
			//identifier found in local symbol tables
			Tlocal_sym_tab_node *temp = (Tlocal_sym_tab_node *)(temp_sym_tab->data_ptr);

			//test for AUTO
			if (temp->data_type == TYPE_AUTO)
			{
				//variable data type is AUTO
				temp->data_type = data_type;	//change variable data type to assigned value data type
				return OK;
			}
			else
			{
				//variable data type is not AUTO
				if (st_cmp_types(temp->data_type, data_type) == INCOMPATIBLE_TYPES)
				{
					//incompatible data types
					fprintf(stderr, "Incompatible data types.\n");
					return ERR_TYPE;
				}
				else
				{
					//compatible data types
					return OK;
				}
			}
		}
	}
	else
	{
		//called outside of function
		fprintf(stderr, "Assigning outside of function.\n");
		return ERR_SEMANTIC;
	}
}

//return true if function is in global symbol table (is declared or defined)
bool func_exists(char *id)
{
	TSymTable *temp = st_find_func(id);	//search for function in global symbol table
	if (temp == NULL)
	{
		//not found => doesn't exist
		return false;
	}
	else
	{
		//found => exists
		return true;
	}
}

//return true if function is defined
bool is_func_defined(char *id)
{
	return st_is_function_defined(id);	//return true if function is defined
}

//return function data type
Tdata_type get_func_type(char *id)
{
	return st_get_function_type(id);	//return function data type
}

//return function node address
Tglobal_sym_tab_node *get_func(char *id)
{
	TSymTable *temp = st_find_func(id);
	if (temp == NULL)
	{
		//function not found
		return NULL;
	}
	else
	{
		return (Tglobal_sym_tab_node *)(temp->data_ptr);
	}
}

//add new local symbol table to local symbol table stack
int add_block()
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		Tsym_tab_error temp;

		//new block -> new symbol table
		//try add local symbol table for current block
		temp = st_add_local_block(current_func_data->local_sym_table);

		switch (temp)
		{
			case OK_ST:
				//new local symbol table successfully added
				return OK;
				break;
			//st_add_local_block() failed
			case ERR_ST_99:
				return ERR_INTERPRET;
				break;
			default:
				return ERR_SEMANTIC;
				break;
		}
	}
	else
	{
		//called outside of function
		fprintf(stderr, "Tried to add new block outside of function.\n");
		return ERR_SEMANTIC;
	}
}

//remove local symbol table from top of stack
int remove_block()
{
	if (current_func_data != NULL)
	{
		//ok - called inside of function
		//remove local symbol table for current block
		if (st_remove_local_block(current_func_data->local_sym_table) != OK_ST)
		{
			//st_remove_local_block() failed
			return ERR_SEMANTIC;
		}
		else
		{
			//local symbol table for current block successfully removed
			return OK;
		}
	}
	else
	{
		//called outside of function
		fprintf(stderr, "Tried to remove block outside of function.\n");
		return ERR_SEMANTIC;
	}
}

//check if every function in global symbol table is defined
//-use when semantic analysis ends
int every_func_defined()
{
	Tsym_tab_error temp = st_check_function_definitions();
	//check result of st_check_function_definitions
	switch (temp)
	{
		case OK_ST:
			return OK;
		case ERR_ST_NODEF:
			return ERR_UNDEF;
		default:
			return ERR_SEMANTIC;
	}
}

//free global symbol table including all data, local symbol tables etc.
void free_global_symbol_table()
{
	st_free_global_sym_table();
}
