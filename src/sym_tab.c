/******** sym_tab.c *********
*
* FIT VUT, IFJ 089
* Author: Šimon Urban, xurban63
* Summary: Symbol table - Functions used by semantic.c
*
*/

#include "sym_tab.h"

//dispose parameter stack including all data
static void free_parameter_stack(Tstack *s)
{
	if (s != NULL)
	{
		Tparameter *temp = NULL;
		while (!(stack_empty(s)))
		{
			//free parameter data
			temp = (Tparameter *)stack_pop(s);
			free(temp->name);
			free(temp);
		}
		//free parameter stack
		stack_dispose(s);
	}
}

//check global symbol table for non defined functions
static void st_every_function_defined(TSymTable *node, bool *all_func_defined)
{
	if(node->left_child != NULL)
	{
		//check left child
		st_every_function_defined(node->left_child, all_func_defined);
	}
	if(node->right_child != NULL)
	{
		//check right child
		st_every_function_defined(node->right_child, all_func_defined);
	}

	Tglobal_sym_tab_node *data = (Tglobal_sym_tab_node *)(node->data_ptr);
	if (data->defined == false)
	{
		//function not defined => not all functions are defined
		*all_func_defined = false;
	}
}

//check if every function and main() is defined
Tsym_tab_error st_check_function_definitions()
{
	bool main_defined = st_is_function_defined("main");
	bool all_func_defined = true;

	//check all functions in tree
	if (global_sym_table != NULL)
	{
		if (global_sym_table->root != NULL)
		{
			st_every_function_defined(global_sym_table->root, &all_func_defined);
		}
	}

	//check if main is defined and if all other functions are defined
	if ( (main_defined == true) && (all_func_defined == true) )
	{
		//main() and any other declared function defined
		return OK_ST;
	}
	else
	{
		//error - main() or any other declared function not defined
		if (main_defined == true)
		{
			fprintf(stderr, "Declared function not defined.\n");
		}
		else
		{
			fprintf(stderr, "main() definition not found.\n");
		}
		return ERR_ST_NODEF;
	}
}

//initialize new function added into global symbol table
static Tsym_tab_error new_func_init(Tglobal_sym_tab_node *global_node)
{
	if (global_node != NULL)
	{
		Tsym_tab_error add_block_error;

		//initialize local symbol table
		global_node->local_sym_table = stack_init();
		if (global_node->local_sym_table == NULL)
		{
			//can't allocate memory for stack
			return ERR_ST_99;
		}
		else
		{
			//add new block: function body + parameters
			add_block_error = st_add_local_block(global_node->local_sym_table);
			if (add_block_error != OK)
			{
				//add_block() failed
				return add_block_error;
			}
			else
			{
				//first block added
				global_node->var_count = 0;	//set default variable count to 0
				//copy parameters into local symbol table
				return st_copy_params_into_table(global_node);
			}
		}
	}
	else
	{
		//global node given by parameter is NULL
		return ERR_ST_OTHER;
	}
}

//find parameter in given parameter stack
//return NULL if parameter does not exist
Tparameter *st_find_param(char *name, Tstack *parameter_stack)
{
	if (parameter_stack != NULL)
	{
		Tparameter *searched_param = NULL;

		int number_of_parameters = parameter_stack->top - parameter_stack->base;	//get parameter count
		for(int i = 0; i < number_of_parameters; i++)
		{
			//compare name of i-th parameter with searched name
			if (strcmp(name, ((Tparameter *)(*(parameter_stack->base + i)))->name) == 0)
			{
				//found
				searched_param = (Tparameter *)(*(parameter_stack->base + i));
				break;
			}
		}

		return searched_param;	//return NULL if parameter does not exist
	}
	else
	{
		//given parameter stack is NULL
		return NULL;
	}
}

//return n-th parameter of given function
//-return NULL if function or parameter does not exist
Tlocal_sym_tab_node *st_get_nth_param(Tglobal_sym_tab_node *func_address, int n)
{
	if (n <= 0)
	{
		//negative or 0-th parameter does not exist
		return NULL;
	}
	else
	{
		if (func_address == NULL)
		{
			//function node pointer is NULL
			return NULL;
		}
		else
		{
			//ok - function node pointer is not NULL
			//find parameter name
			char *temp_name = NULL;
			Tparameter *temp_param = NULL;
			Tstack *temp_stack = func_address->parameters;

			if (temp_stack != NULL)
			{
				if ((temp_stack->top - temp_stack->base) < n)
				{
					//function does not have n-th parameter
					return NULL;
				}
				else
				{
					//get n-th parameter
					temp_param = ((Tparameter *)(*(temp_stack->base + n - 1)));
					if (temp_param != NULL)
					{
						//save pointer to parameter identifier
						temp_name = temp_param->name;
					}
					else
					{
						//found parameter is NULL
						return NULL;
					}
				}
			}
			else
			{
				//function parameter stack is NULL - can't find parameter
				return NULL;
			}

			//parameter name found
			//find parameter in local symbol table
			if (func_address->local_sym_table != NULL)
			{
				TSymTable *searched_id = NULL;

				//search identifier in local symbol table for function body
				search_in_tree(((TBinTree *)(*(func_address->local_sym_table->base)))->root, temp_name, &searched_id);

				if (searched_id != NULL)
				{
					//parameter node in local symbol table found
					return (Tlocal_sym_tab_node *)(searched_id->data_ptr);
				}
				else
				{
					//parameter node not found in local symbol table
					return NULL;
				}
			}
			else
			{
				//function local symbol table is set to NULL
				return NULL;
			}
		}
	}
}

//copy parameters into local symbol table
Tsym_tab_error st_copy_params_into_table(Tglobal_sym_tab_node *global_node)
{
	if (global_node != NULL)
	{
		//ok - inside of function
		Tstack *temp_param = global_node->parameters;
		Tparameter *temp = NULL;
		TSymTable *searched_id = NULL;
		Tlocal_sym_tab_node *data = NULL;
		Tstack *temp_local = global_node->local_sym_table;

		if ((temp_local != NULL) && (temp_param != NULL))
		{
			if (stack_empty(temp_local))
			{
				//local symbol table stack is empty - can't copy parameters
				return ERR_ST_OTHER;
			}
			else
			{
				//local symbol table stack not empty - copy parameters
				int number_of_parameters = temp_param->top - temp_param->base;	//get parameter count

				for(int i = 0; i < number_of_parameters; i++)
				{
					//copy parameter into local symbol table
					temp = (Tparameter *)(*(temp_param->base + i));

					//search for identifier in local symbol table on top of local symbol table stack
					search_in_tree(((TBinTree *)(stack_top(temp_local)))->root, temp->name, &searched_id);
					if (searched_id != NULL)
					{
						//identifier already in local symbol table
						return ERR_ST_OTHER;
					}
					else
					{
						//identifier not in local symbol table - add
						data = malloc(sizeof(Tlocal_sym_tab_node));
						if (data == NULL)
						{
							//can't allocate memory for data
							return ERR_ST_99;
						}
						else
						{
							//memory for data allocated
							//copy parameter values into data
							data->data_type = temp->data_type;
							data->defined = false;	//parameters are always defined
							data->offset = global_node->var_count;	//set variable offset
							(global_node->var_count)++;	//increment variable count
							if (insert_into_tree(&(((TBinTree *)stack_top(temp_local))->root), temp->name, data) == OK)
							{
								//data inserted into tree
								//copy just address of key (key can't change)
								TSymTable *temp_var = st_find_var(temp->name, temp_local);
								if (temp_var != NULL)
								{
									data->name = temp_var->key;
								}
								else
								{
									return ERR_ST_99;
								}
							}
							else
							{
								//insert_into_tree() failed
								return ERR_ST_99;
							}
						}
					}
				}
				return OK_ST;
			}
		}
		else
		{
			//local symbol table stack or parameter stack is NULL
			return ERR_ST_OTHER;
		}
	}
	else
	{
		//called outside of function
		return ERR_ST_OTHER;
	}
}

//search for identifier on top of local symbol table stack, in parameters and in global symbol table
bool st_can_use_id(char *name, Tglobal_sym_tab_node *global_node)
{
	if (global_node != NULL)
	{
		if ((global_node->local_sym_table != NULL) && (global_node->parameters != NULL))
		{
			bool found = false;
			TSymTable *searched_id = NULL;
			//search for identifier in local symbol table on top of local symbol table stack
			search_in_tree(((TBinTree *)(stack_top(global_node->local_sym_table)))->root, name, &searched_id);
			if (searched_id != NULL)
			{
				//identifier found
				found = true;
			}

			if (found == false)
			{
				//not found in local symbol tables
				//search for identifier in parameters
				if (st_find_param(name, global_node->parameters) != NULL)
				{
					//identifier found
					found = true;
				}
			}

			if (found == false)
			{
				//not found in local symbol tables and parameter stack
				//search for identifier in global symbol table
				searched_id = st_find_func(name);
				if (searched_id != NULL)
				{
					//identifier found
					found = true;
				}
			}

			//can use identifier only when found == false
			return !(found);
		}
		else
		{
			//given global node parameter stack or local symbol table stack is NULL
			return false;
		}
	}
	else
	{
		//given global node is NULL
		return false;
	}
}

//compare two data types
Tcompare st_cmp_types(Tdata_type type1, Tdata_type type2)
{
	if (type1 == type2)
	{
		//same data types
		return SAME_DATA_TYPES;
	}
	else
	{
		//different data types
		if ( ((type1 == TYPE_INT) && (type2 == TYPE_DOUBLE)) || ((type1 == TYPE_DOUBLE) && (type2 == TYPE_INT)) )
		{
			//implicit conversion
			return IMPLICIT_CONVERSION;
		}
		else
		{
			//incompatible data types
			return INCOMPATIBLE_TYPES;
		}
	}
}

//---GLOBAL SYMBOL TABLE---//

//initialize global symbol table
Tsym_tab_error st_init_global_sym_table()
{
	global_sym_table = initialize_tree();
	if (global_sym_table == NULL)
	{
		//initialize_tree() failed
		return ERR_ST_99;
	}
	else
	{
		//tree initialized
		temp_global_node = malloc(sizeof(Tglobal_sym_tab_node));
		if (temp_global_node == NULL)
		{
			//can't allocate memory for temp_global_node
			free(global_sym_table);
			return ERR_ST_99;
		}
		else
		{
			//global symbol table and temp_global_node initialized
			all_local_symbol_tables = stack_init();
			if (all_local_symbol_tables == NULL)
			{
				//stack_init() failed
				free(global_sym_table);
				free(temp_global_node);
				return ERR_ST_99;
			}
			else
			{
				//set default values
				temp_global_node->parameters = NULL;
				temp_global_node->local_sym_table = NULL;
				temp_global_node->defined = false;
				temp_global_node->data_type = TYPE_NON;
				return OK_ST;
			}
		}
	}
}

//modified destroy_branch() from bintree - used in st_free_global_sym_table()
static void destroy_branch_global(TSymTablePointer node)
{
	if(node->left_child != NULL)
	{
		//destroy left child and its children
		destroy_branch_global(node->left_child);
	}
	if(node->right_child != NULL)
	{
		//destroy right child and its children
		destroy_branch_global(node->right_child);
	}

	//---free memory---
	//free parameters
	free_parameter_stack(((Tglobal_sym_tab_node *)(node->data_ptr))->parameters);
	//free local symbol table stack - binary trees still exists
	Tstack *temp = ((Tglobal_sym_tab_node *)(node->data_ptr))->local_sym_table;
	if (temp != NULL)
	{
		stack_dispose(temp);
	}
	//free global symbol table node data
	free((Tglobal_sym_tab_node *)(node->data_ptr));
	//free global symbol table node key
	free(node->key);
	//free global symbol table node
	free(node);
}

//free global symbol table - including all nodes data - modified delete_tree() from bintree
void st_free_global_sym_table()
{
	//free global symbol table
	if (global_sym_table != NULL)
	{
		//ok - global symbol table is initialized

		if (global_sym_table->root != NULL)
		{
			//free global symbol node including data
			destroy_branch_global(global_sym_table->root);
			global_sym_table->root = NULL;
			//free tree
			free(global_sym_table);
		}
		else
		{
			//global symbol table empty - free tree
			free(global_sym_table);
		}

		//free temp_global_node-> parameters if it's not NULL
		if (temp_global_node != NULL)
		{
			//free temp_global_node->parameters if they are not NULL
			if (temp_global_node->parameters != NULL)
			{
				free_parameter_stack(temp_global_node->parameters);
			}
		}

		//free temp_global_node
		free(temp_global_node);

		//free temp_global_node_name
		free(temp_global_node_name);

		//free all local symbol tables
		st_free_local_sym_table();
	}
}

//compare two function parameters - used in same_function_headers()
static bool same_function_parameters(Tstack *params1, Tstack *params2)
{
	if ((params1 != NULL) && (params2 != NULL))
	{
		if ((params1->top - params1->base) == (params2->top - params2->base))
		{
			//same number of parameters
			int number_of_parameters = params1->top - params1->base;
			bool same = true;
			//compare parameters
			for(int i = 0; i < number_of_parameters; i++)
			{
				//compare data types of i-th parameter from both stacks
				if (((Tparameter *)(*(params1->base + i)))->data_type != ((Tparameter *)(*(params2->base + i)))->data_type)
				{
					//different data types
					same = false;
					break;
				}
				//compare identifiers types of i-th parameter from both stacks
				if (strcmp(((Tparameter *)(*(params1->base + i)))->name, ((Tparameter *)(*(params2->base + i)))->name) != 0)
				{
					//different identifiers
					same = false;
					break;
				}
			}
			return same;
		}
		else
		{
			//different number of parameters => different parameters
			return false;
		}
	}
	else
	{
		//at least one of given parameter stacks is NULL
		return false;
	}
}

//compare two function headers - used in global_add_declaration/definition()
static bool same_function_headers(Tglobal_sym_tab_node *func1, Tglobal_sym_tab_node *func2)
{
	if ((func1 != NULL) && (func2 != NULL))
	{
		if (func1->data_type == func2->data_type)
		{
			//return data types are same
			if (same_function_parameters(func1->parameters, func2->parameters))
			{
				//parameters are same
				return true;
			}
			else
			{
				//parameters are different
				return false;
			}
		}
		else
		{
			//return types are different
			return false;
		}
	}
	else
	{
		//at least one of given nodes is NULL
		return false;
	}
}

//test uniqueness of identifiers in stack
static bool all_params_unique(Tstack *params)
{
	if (params != NULL)
	{
		bool unique_params = true;

		int number_of_parameters = params->top - params->base;	//get parameter count
		for(int i = 0; i < number_of_parameters - 1; i++)
		{
			for(int j = i + 1; j < number_of_parameters; j++)
			{
				//compare i-th parameter name with all j-th parameters, where j > i
				if (strcmp(((Tparameter *)(*(params->base + i)))->name, ((Tparameter *)(*(params->base + j)))->name) == 0)
				{
					//same parameter names
					unique_params = false;
					break;
				}
			}
		}

		return unique_params;
	}
	else
	{
		//stack given as parameter is NULL
		return false;
	}
}

//add temp_global_node to global symbol table as declaration
Tsym_tab_error st_global_add_declaration()
{
	if (temp_global_node != NULL)
	{
		temp_global_node->defined = false;	//declaration => not defined

		TSymTable *temp = st_find_func(temp_global_node_name);  //search for function in global symbol table
		if (temp == NULL)
		{
			//new function identifier

			temp_global_node->func_start = FUNC_START_UNDEF;
			Tglobal_sym_tab_node *data = malloc(sizeof(Tglobal_sym_tab_node));
			if (data == NULL)
			{
				//can't allocate memory for data
				return ERR_ST_99;
			}
			else
			{
				//memory for data allocated
				if (all_params_unique(temp_global_node->parameters))
				{
					//parameter names unique
					//copy temp_global_node into data
					data->func_start = temp_global_node->func_start;
					data->data_type = temp_global_node->data_type;
					data->defined = temp_global_node->defined;
					data->local_sym_table = temp_global_node->local_sym_table;
					data->parameters = temp_global_node->parameters;
					//insert new function declaration into global symbol table
					if (insert_into_tree(&(global_sym_table->root), temp_global_node_name, data) == OK)
					{
						//data inserted into tree
						temp = st_find_func(temp_global_node_name);
						if (temp == NULL)
						{
							//new function not found
							return ERR_ST_99;
						}
						else
						{
							//new function found in local symbol table
							//initialize new function data
							return new_func_init((Tglobal_sym_tab_node *)(temp->data_ptr));
						}
					}
					else
					{
						//insert_into_tree() failed
						return ERR_ST_99;
					}
				}
				else
				{
					//redefinition of parameter
					fprintf(stderr, "Same identifier used by two or more parameters.\n");
					free(data);
					free_parameter_stack(temp_global_node->parameters);
					return ERR_ST_REDEF;
				}
			}
		}
		else
		{
			//function already in symbol table
			//compare function headers
			if (same_function_headers((Tglobal_sym_tab_node *)(temp->data_ptr), temp_global_node) == false)
			{
				//different function headers
				fprintf(stderr, "Different function header of declaration and previous declaration/definition.\n");
				//free current function parameter stack
				free_parameter_stack(temp_global_node->parameters);
				return ERR_ST_DIFF_FUNC_HEADER;
			}
			else
			{
				//same function headers
				//free current function parameter stack
				free_parameter_stack(temp_global_node->parameters);
				return OK_ST;
			}
		}
	}
	else
	{
		//global symbol table not initialized
		return ERR_ST_OTHER;
	}
}

//add temp_global_node to global symbol table as definition
Tsym_tab_error st_global_add_definition(unsigned function_start)
{
	if (temp_global_node != NULL)
	{
		temp_global_node->func_start = function_start;
		temp_global_node->defined = true;	//definition => defined

		TSymTable *temp = st_find_func(temp_global_node_name);  //search for function in global symbol table
		if (temp == NULL)
		{
			//new function identifier

			//local symbol table stack initialized
			Tglobal_sym_tab_node *data = malloc(sizeof(Tglobal_sym_tab_node));
			if (data == NULL)
			{
				//can't allocate memory for data
				return ERR_ST_99;
			}
			else
			{
				//memory for data allocated
				if (all_params_unique(temp_global_node->parameters))
				{
					//parameter names unique
					//copy temp_global_node into data
					data->func_start = temp_global_node->func_start;
					data->data_type = temp_global_node->data_type;
					data->defined = temp_global_node->defined;
					data->local_sym_table = temp_global_node->local_sym_table;
					data->parameters = temp_global_node->parameters;
					//insert new function definition into global symbol table
					if (insert_into_tree(&(global_sym_table->root), temp_global_node_name, data) == OK)
					{
						//data inserted into tree
						temp = st_find_func(temp_global_node_name);
						if (temp == NULL)
						{
							//new function not found
							return ERR_ST_99;
						}
						else
						{
							//new function found in local symbol table
							//initialize new function data
							return new_func_init((Tglobal_sym_tab_node *)(temp->data_ptr));
						}
					}
					else
					{
						//insert_into_tree() failed
						return ERR_ST_99;
					}
				}
				else
				{
					//redefinition of parameter
					fprintf(stderr, "Same identifier used by two or more parameters.\n");
					free(data);
					free_parameter_stack(temp_global_node->parameters);
					return ERR_ST_REDEF;
				}
			}
		}
		else
		{
			//function already in symbol table
			if (((Tglobal_sym_tab_node *)(temp->data_ptr))->defined == true)
			{
				//redefinition of function
				fprintf(stderr, "Function redefinition.\n");
				//free current function parameter stack
				free_parameter_stack(temp_global_node->parameters);
				return ERR_ST_REDEF;
			}
			else
			{
				//function in symbol table declared, not defined - compare function headers
				if (same_function_headers((Tglobal_sym_tab_node *)(temp->data_ptr), temp_global_node) == false)
				{
					//different function headers
					fprintf(stderr, "Different function header of definition and previous declaration/definition.\n");
					//free current function parameter stack
					free_parameter_stack(temp_global_node->parameters);
					return ERR_ST_DIFF_FUNC_HEADER;
				}
				else
				{
					//free current function parameter stack
					free_parameter_stack(temp_global_node->parameters);
					((Tglobal_sym_tab_node *)(temp->data_ptr))->defined = true;
					((Tglobal_sym_tab_node *)(temp->data_ptr))->func_start = temp_global_node->func_start;
					return OK_ST;
				}
			}
		}
	}
	else
	{
		//global symbol table not initialized
		return ERR_ST_OTHER;
	}
}

//return return_type of function
Tdata_type st_get_function_type(char *name)
{
	TSymTable *temp = st_find_func(name);	//search for function in global symbol table
	if (temp == NULL)
	{
		//function does not exist
		return TYPE_NON;
	}
	else
	{
		//function found - return function data type
		return ((Tglobal_sym_tab_node *)(temp->data_ptr))->data_type;
	}
}

//check if function is defined
bool st_is_function_defined(char *name)
{
	TSymTable *temp = st_find_func(name);	//search for function in global symbol table
	if (temp == NULL)
	{
		//function does not exist => is not defined
		return false;
	}
	else
	{
		//function found - return true if it is defined
		return ((Tglobal_sym_tab_node *)(temp->data_ptr))->defined;
	}
}

//search for function in global symbol table
TSymTable *st_find_func(char *name)
{
	TSymTable *searched_id = NULL;

	if (global_sym_table != NULL)
	{
		//search for identifier in global symbol table
		search_in_tree(global_sym_table->root, name, &searched_id);
	}

	return searched_id;	//return NULL if not found of global_sym_table is NULL
}

//---LOCAL SYMBOL TABLE---//

//modified destroy_branch() from bintree - free local symbol table node
static void destroy_branch_local(TSymTablePointer node)
{
	if(node->left_child != NULL)
	{
		//destroy left child and its children
		destroy_branch_local(node->left_child);
	}
	if(node->right_child != NULL)
	{
		//destroy right child and its children
		destroy_branch_local(node->right_child);
	}
	//--free memory--

	Tlocal_sym_tab_node *temp = (Tlocal_sym_tab_node *)(node->data_ptr);
	if ((temp->defined == true) && (temp->data_type == TYPE_STRING))
	{
		free(temp->string_val);
	}
	free(temp);

	//free global symbol table node key
	free(node->key);
	//free global symbol table node
	free(node);
}

//free all local symbol tables (including data)
void st_free_local_sym_table()
{
	TBinTree *temp = NULL;
	//free all local symbol tables on stack
	while (!(stack_empty(all_local_symbol_tables)))
	{
		temp = (TBinTree *)stack_pop(all_local_symbol_tables);	//get local symbol table
		if (temp != NULL)
		{
			//free binary tree(local symbol table) including all data
			if (temp->root != NULL)
			{
				destroy_branch_local(temp->root);
				temp->root = NULL;
				free(temp);
			}
			else
			{
				free(temp);
			}
		}
	}
	//free all_local_symbol_tables stack
	stack_dispose(all_local_symbol_tables);
}


//add new variable into local symbol table on top of local symbol table stack
Tsym_tab_error st_local_add_id(char *name, Tdata_type data_type, Tglobal_sym_tab_node *global_node)
{
	if (global_node != NULL)
	{
		if (global_node->local_sym_table != NULL)
		{
			//ok - called inside of function
			bool can_use_identifier = st_can_use_id(name, current_func_data);

			if (can_use_identifier == true)
			{
				//not found - new identifier -> insert into local symbol table
				Tlocal_sym_tab_node *data = malloc(sizeof(Tlocal_sym_tab_node));
				if (data == NULL)
				{
					//can't allocate memory for data
					return ERR_ST_99;
				}
				else
				{
					//set values of elements from data
					data->data_type = data_type;
					data->defined = false;
					data->offset = global_node->var_count;	//set variable offset
					(global_node->var_count)++;	//increment variable count
					//insert new variable into local symbol table
					if (insert_into_tree(&(((TBinTree *)stack_top(global_node->local_sym_table))->root), name, data) == OK)
					{
						//data inserted into tree
						//copy just address of key (key can't change)
						TSymTable *temp_var = st_find_var(name, global_node->local_sym_table);
						if (temp_var != NULL)
						{
							data->name = temp_var->key;
							return OK_ST;
						}
						else
						{
							return ERR_ST_99;
						}
					}
					else
					{
						//insert_into_tree() failed
						return ERR_ST_99;
					}
				}
			}
			else
			{
				//identifier found - can't use identifier
				return ERR_ST_REDEF;
			}
		}
		else
		{
			//local symbol table stack is NULL
			return ERR_ST_OTHER;
		}
	}
	else
	{
		//called outside of function
		return ERR_ST_OTHER;
	}

}

//return variable data type
Tdata_type st_get_variable_type(char *name, Tglobal_sym_tab_node *global_node)
{
	if (global_node != NULL)
	{
		TSymTable *temp = st_find_var(name, global_node->local_sym_table);	//search for variable in local symbol table
		if (temp == NULL)
		{
			//variable not found in local symbol table
			return TYPE_NON;
		}
		else
		{
			//variable found - return its data type
			return ((Tlocal_sym_tab_node *)(temp->data_ptr))->data_type;
		}
	}
	else
	{
		//global node given as parameter is NULL
		return TYPE_NON;
	}
}

//check if the variable is defined
bool st_is_variable_defined(char *name, Tglobal_sym_tab_node *global_node)
{
	if (global_node != NULL)
	{
		TSymTable *temp = st_find_var(name, global_node->local_sym_table);	//search for variable in local symbol table
		if (temp == NULL)
		{
			//variable not found in local symbol table
			return false;

		}
		else
		{
			//variable found - return true if it is defined
			return ((Tlocal_sym_tab_node *)(temp->data_ptr))->defined;
		}
	}
	else
	{
		//global node given as parameter is NULL
		return false;
	}
}

//add new local symbol table on top of local symbol table stack
Tsym_tab_error st_add_local_block(Tstack *sym_table_stack)
{
	if (sym_table_stack != NULL)
	{
		TBinTree *new_table = initialize_tree();
		if (new_table == NULL)
		{
			//can't initialize new local symbol table
			return ERR_ST_99;
		}
		else
		{
			//new local symbol table initialized
			//push new local symbol table on top of local symbol table stack
			Tstack_error temp_stack_err = stack_push(sym_table_stack, new_table);
			if (temp_stack_err == OK_STACK)
			{
				//push ok - push new local symbol table address on top of all_local_symbol_tables stack too
				temp_stack_err = stack_push(all_local_symbol_tables, new_table);
				if (temp_stack_err == OK_STACK)
				{
					//second push ok
					return OK_ST;
				}
				else
				{
					//stack full - can't allocate more memory for stack elements
					return ERR_ST_99;
				}
			}
			else
			{
				//stack full - can't allocate more memory for stack elements
				free(new_table);
				return ERR_ST_99;
			}
		}
	}
	else
	{
		//given local symbol table stack is NULL
		return ERR_ST_OTHER;
	}
}

//remove local symbol table on top of local symbol table stack
Tsym_tab_error st_remove_local_block(Tstack *sym_table_stack)
{
	if (sym_table_stack != NULL)
	{
		if (stack_empty(sym_table_stack))
		{
			//given local symbol table stack is empty - pop on empty stack
			return ERR_ST_OTHER;
		}
		else
		{
			//pop local symbol table from top of stack
			stack_pop(sym_table_stack);
			if (stack_empty(sym_table_stack))
			{
				//last block removed - end of function
				current_func_data = NULL;
			}
			return OK_ST;
		}
	}
	else
	{
		return ERR_ST_OTHER;
	}
}


//search for identifier in given local symbol table stack
TSymTable *st_find_var(char *name, Tstack *sym_table_stack)
{
	TSymTable *searched_id = NULL;

	if (sym_table_stack != NULL)
	{
		//search identifier in stack of local symbol table
		int number_of_elements = sym_table_stack->top - sym_table_stack->base;	//get local symbol table count
		for(int i = number_of_elements - 1; i >= 0; i--)
		{
			//search for identifier in local symbol table
			search_in_tree(((TBinTree *)(*(sym_table_stack->base + i)))->root, name, &searched_id);
			if (searched_id != NULL)
			{
				//variable found
				break;
			}
		}
	}

	return searched_id;	//return identifier address in local symbol table or NULL if not found
}
