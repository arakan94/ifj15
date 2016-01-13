/******** testfile.c *********
*
* FIT VUT, IFJ 089
* Author: Šimon Urban, xurban63
* Date: 27. 11. 2015
* Summary: Semantic analysis testfile
* Notes: Final version
*
*/

#include "semantic.h"

void err(int error, char *message)
{
	switch(error)
	{
		case OK:
			break;
		case ERR_INTERPRET:
			fprintf(stderr, "ERROR: 99 : %s\n", message);
			break;
		case ERR_AUTO:
			fprintf(stderr, "ERROR: TYPE_AUTO : %s\n", message);
			break;
		case ERR_TYPE:
			fprintf(stderr, "ERROR: COMPATIBILITY : %s\n", message);
			break;
		case ERR_UNDEF:
			fprintf(stderr, "ERROR: NODEF/REDEF : %s\n", message);
			break;
		case ERR_SEMANTIC:
			fprintf(stderr, "ERROR: UNKNOWN ERROR : %s\n", message);
			break;
	}
}

void print_tree(TSymTable *node)
{
	if(node == NULL)
		printf("Tree is empty.\n");
	else{
		if(node->left_child != NULL)
		{
			//Print left child and and call print_tree on it
			printf("%s <--|%s|\n", node->left_child->key, node->key);
			print_tree(node->left_child);
		}
		else
		{
			printf("NULL <--|%s|\n", node->key);
		}

		if(node->right_child != NULL)
		{
			//Print right child and call print_tree on it
			printf("|%s|--> %s\n", node->key, node->right_child->key);
			print_tree(node->right_child);
		}
		else
		{
			printf("|%s|--> NULL\n", node->key);
		}
	}
}

int main()
{
	int test_start_address = 123456;

	int error = OK;

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	//init global symbol table
	printf("TEST 0: INIT\n");
	error = initialize_global_symbol_table();
	err(error, "Initialize global symbol table");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 1: Add function declaration and definition\n");
	printf(
		"--------------------------------------------------------------\n"
		"int func1();\n"
		"\n"
		"int func1()\n"
		"{\n"
		"}\n"
		"--------------------------------------------------------------\n"
	);
	printf("---No error expected:---\n");
	//add functoin declaration
	error = add_func(TYPE_INT, "func1");
	err(error, "add_func() failed");
	error = func_declaration();
	err(error, "func_declaration() failed");

	//add function definition
	error = add_func(TYPE_INT, "func1");
	err(error, "add_func() failed");
	error = func_definition(test_start_address);
		err(error, "func_definition() failed");
	error = remove_block();
	err(error, "remove_block() failed");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 2: Add function declaration, definition and declaration again (with same parameters)\n");
	printf(
		"--------------------------------------------------------------\n"
		"int func_with_param(double a, string str);\n"
		"\n"
		"int func_with_param(double a, string str);\n"
		"\n"
		"int func_with_param(double a, string str)\n"
		"{\n"
		"}\n"
		"--------------------------------------------------------------\n"
	);
	printf("---No error expected:---\n");
	//add function declaration
	error = add_func(TYPE_INT, "func_with_param");
	err(error, "add_func() failed");
	error = add_param(TYPE_DOUBLE, "a");
	err(error, "add_param() failed");
	error = add_param(TYPE_STRING, "str");
	err(error, "add_param() failed");
	error = func_declaration();
	err(error, "func_declaration() failed");

	//add function definition
	error = add_func(TYPE_INT, "func_with_param");
	err(error, "add_func() failed");
	error = add_param(TYPE_DOUBLE, "a");
	err(error, "add_param() failed");
	error = add_param(TYPE_STRING, "str");
	err(error, "add_param() failed");
	error = func_definition(test_start_address);
		err(error, "func_definition() failed");
		printf("PRINT LOCAL SYMBOL TABLE for 1. block of func_with_param():\n");
		print_tree(((TBinTree *)(stack_top(current_func_data->local_sym_table)))->root);
		printf("\n");
		Tlocal_sym_tab_node *get_param_test = NULL;
		//print data types of params
		printf("Existing parameters data type:\n");
		if ((get_param_test = get_nth_param(current_func_data, 1)) != NULL)
			printf("	Type of 1. param: %d\n", get_param_test->data_type);
		else
			printf("	NULL\n");
		if ((get_param_test = get_nth_param(current_func_data, 2)) != NULL)
			printf("	Type of 2. param: %d\n", get_param_test->data_type);
		else
			printf("	NULL\n");
		//now nonexisting parameters
		printf("Nonexisting parameters data type:\n");
		if ((get_param_test = get_nth_param(current_func_data, 3)) != NULL)
			printf("	Type of 3. param: %d\n", get_param_test->data_type);
		else
			printf("	NULL\n");
		if ((get_param_test = get_nth_param(current_func_data, -3)) != NULL)
			printf("	Type of -3. param: %d\n", get_param_test->data_type);
		else
			printf("	NULL\n");
		if ((get_param_test = get_nth_param(current_func_data, 0)) != NULL)
			printf("	Type of 0. param: %d\n", get_param_test->data_type);
		else
			printf("	NULL\n");
	error = remove_block();
	err(error, "remove_block() failed");

	//add function declaration
	error = add_func(TYPE_INT, "func_with_param");
	err(error, "add_func() failed");
	error = add_param(TYPE_DOUBLE, "a");
	err(error, "add_param() failed");
	error = add_param(TYPE_STRING, "str");
	err(error, "add_param() failed");
	error = func_declaration();
	err(error, "func_declaration() failed");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 3: Check if every declared function and main() is defined before and after main() definition\n");

	printf("Check if every declared function and main() is defined:\n");
	error = every_func_defined();
	if (error == OK)
	{
		printf("	- OK\n");
	}
	else
	{
		printf("	- ERROR - main() or any other declared function definition not found (expected)\n");
	}

	printf("\nAdd function int main()\n");
	printf(
		"--------------------------------------------------------------\n"
		"int main()\n"
		"{\n"
		"}\n"
		"--------------------------------------------------------------\n"
	);
	printf("---No error expected:---\n");
	//add function definition
	error = add_func(TYPE_INT, "main");
	err(error, "add_func() failed");
	error = func_definition(test_start_address);
		err(error, "func_definition() failed");
		error = remove_block();
	err(error, "remove_block() failed");

	printf("Check if every declared function and main() is defined:\n");
	error = every_func_defined();
	if (error == OK)
	{
		printf("	- OK\n");
	}
	else
	{
		printf("	- ERROR - main() or any other declared function definition not found\n");
	}

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");


	printf("\nTEST 4: Add function declaration and definition (with different parameter identifier)\n");
	printf(
		"--------------------------------------------------------------\n"
		"string func_with_different_param(double a);\n"
		"\n"
		"string func_with_different_param(double b)\n"
		"{\n"
		"}\n"
		"\n"
		"--------------------------------------------------------------\n"
	);
	printf("---1 error expected:---\n");
	//add function declaration
	error = add_func(TYPE_STRING, "func_with_different_param");
	err(error, "add_func() failed");
	error = add_param(TYPE_DOUBLE, "a");
	err(error, "add_param() failed");
	error = func_declaration();
	err(error, "func_declaration() failed");

	//add function definition
	error = add_func(TYPE_STRING, "func_with_different_param");
	err(error, "add_func() failed");
	error = add_param(TYPE_DOUBLE, "b");
	err(error, "add_param() failed");
	error = func_definition(test_start_address);
	err(error, "func_definition() failed (expected)");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 5: Add function declaration and definition (with different parameter data type)\n");
	printf(
		"--------------------------------------------------------------\n"
		"int func_with_different_param_type(double a);\n"
		"\n"
		"int func_with_different_param_type(string a)\n"
		"{\n"
		"}\n"
		"--------------------------------------------------------------\n"
	);
	printf("---1 error expected:---\n");
	//add function declaration
	error = add_func(TYPE_INT, "func_with_different_param_type");
	err(error, "add_func() failed");
	error = add_param(TYPE_DOUBLE, "a");
	err(error, "add_param() failed");
	error = func_declaration();
	err(error, "func_declaration() failed");

	//add function definition
	error = add_func(TYPE_INT, "func_with_different_param_type");
	err(error, "add_func() failed");
	error = add_param(TYPE_STRING, "a");
	err(error, "add_param() failed");
	error = func_definition(test_start_address);
	err(error, "func_definition() failed (expected)");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 6: Add function declaration and definition (with different parameter count)\n");
	printf(
		"--------------------------------------------------------------\n"
		"int diff_param_count(double a);\n"
		"\n"
		"int diff_param_count(double a, string b)\n"
		"{\n"
		"}\n"
		"--------------------------------------------------------------\n"
	);
	printf("---1 error expected:---\n");
	//add function declaration
	error = add_func(TYPE_INT, "diff_param_count");
	err(error, "add_func() failed");
	error = add_param(TYPE_DOUBLE, "a");
	err(error, "add_param() failed");
	error = func_declaration();
	err(error, "func_declaration() failed");

	//add function definition
	error = add_func(TYPE_INT, "diff_param_count");
	err(error, "add_func() failed");
	error = add_param(TYPE_DOUBLE, "a");
	err(error, "add_param() failed");
	error = add_param(TYPE_STRING, "b");
	err(error, "add_param() failed");
	error = func_definition(test_start_address);
	err(error, "func_definition() failed (expected)");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 7: Add function declaration and definition (with different function data type)\n");
	printf(
		"--------------------------------------------------------------\n"
		"int diff_type();\n"
		"\n"
		"string diff_type()\n"
		"{\n"
		"}\n"
		"--------------------------------------------------------------\n"
	);
	printf("---1 error expected:---\n");
	//add function declaration
	error = add_func(TYPE_INT, "diff_type");
	err(error, "add_func() failed");
	error = func_declaration();
	err(error, "func_declaration() failed");

	//add function definition
	error = add_func(TYPE_STRING, "diff_type");
	err(error, "add_func() failed");
	error = func_definition(test_start_address);
	err(error, "func_definition() failed (expected)");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 8: Add function definition and redefinition\n");
	printf(
		"--------------------------------------------------------------\n"
		"double redefinition(//try add variable while not in function body)\n"
		"{\n"
		"	//try add parameter in function body\n"
		"}\n"
		"\n"
		"double redefinition()\n"
		"{\n"
		"}\n"
		"--------------------------------------------------------------\n"
	);
	printf("---3 errors expected:---\n");
	//add function definition
	error = add_func(TYPE_DOUBLE, "redefinition");
	err(error, "add_func() failed");
	error = add_var(TYPE_DOUBLE, "asdf");
	err(error, "add_var() failed (expected)");
	error = func_definition(test_start_address);
		err(error, "func_definition() failed");
		error = add_param(TYPE_DOUBLE, "a");
		err(error, "add_param() failed (expected)");
	error = remove_block();
	err(error, "remove_block() failed");

	//add function redefinition
	error = add_func(TYPE_DOUBLE, "redefinition");
	err(error, "add_func() failed");
	error = func_definition(test_start_address);
	err(error, "func_definition() failed (expected)");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 9: Add function definition (with variables and blocks)\n");
	printf(
		"--------------------------------------------------------------\n"
		"int vars_blocks(double a)\n"
		"{\n"
		"	a = 42;\n"
		"	int test;\n"
		"	test = 42;\n"
		"	double lorem;\n"
		"	lorem = \"str\";\n"
		"	lorem = 42;\n"
		"	double zzz;\n"
		"	double zzz;\n"
		"	double a;\n"
		"	{\n"
		"		string zzz;\n"
		"		zzz = \"lorem ipsum dolor sit amet\"\n"
		"		int i;\n"
		"		double pom = 30;\n"
		"		{\n"
		"			cout << zzz;\n"
		"		}\n"
		"	}\n"
		"}\n"
		"\n"
		"--------------------------------------------------------------\n"
	);
	printf("---3 errors expected:---\n");
	//add function definition
	error = add_func(TYPE_INT, "vars_blocks");
	err(error, "add_func() failed");
	error = add_param(TYPE_DOUBLE, "a");
	err(error, "add_param() failed");
	error = func_definition(test_start_address);
		err(error, "func_definition() failed");
		//-------------------------------
		printf("Test: var_exists(\"a\"): %s (true expected)\n", var_exists("a")?"true":"false");
		printf("Test: var_exists(\"test\"): %s (false expected)\n", var_exists("test")?"true":"false");
		//a = 42;
		error = assign_type_compare("a", TYPE_INT);
		err(error, "assign_type_compare() failed");
		error = set_var_value_double("a", 42);
		err(error, "set_var_value_double() failed");
		printf("Test: is_var_defined(\"a\"): %s (true expected - parameters are always defined)\n", is_var_defined("a")?"true":"false");
		printf("Test: get_var_type(\"a\"): %s (DOUBLE expected)\n", (get_var_type("a")==3)?"STRING":((get_var_type("a")==2)?"DOUBLE":((get_var_type("a")==1)?"INT":"XXX")));
		//int test;
		error = add_var(TYPE_INT, "test");
		err(error, "add_var() failed");
		printf("Test: var_exists(\"test\") after it's declaration: %s (true expected)\n", var_exists("test")?"true":"false");
		//test = 42;
		error = assign_type_compare("test", TYPE_INT);
		err(error, "assign_type_compare() failed");
		error = set_var_value_int("test", 42.5);
		err(error, "set_var_value_int() failed");
		printf("Test: is_var_defined(\"test\"): %s (true expected)\n", is_var_defined("test")?"true":"false");
		//double lorem;
		error = add_var(TYPE_DOUBLE, "lorem");
		err(error, "add_var() failed");
		printf("Test: is_var_defined(\"lorem\"): %s (false expected)\n", is_var_defined("lorem")?"true":"false");
		//lorem = "str";
		error = assign_type_compare("lorem", TYPE_STRING);
		err(error, "assign_type_compare() failed (expected)");
		printf("Test: is_var_defined(\"lorem\"): %s (false expected)\n", is_var_defined("lorem")?"true":"false");
		//lorem = 42;
		error = assign_type_compare("lorem", TYPE_INT);
		err(error, "assign_type_compare() failed");
		error = set_var_value_double("lorem", 42);
		err(error, "set_var_value_double() failed");
		printf("Test: is_var_defined(\"lorem\"): %s (true expected)\n", is_var_defined("lorem")?"true":"false");
		//double zzz;
		error = add_var(TYPE_DOUBLE, "zzz");
		err(error, "add_var() failed");
		//double zzz;
		error = add_var(TYPE_DOUBLE, "zzz");
		err(error, "add_var() failed (expected)");
		//double a
		error = add_var(TYPE_DOUBLE, "a");
		err(error, "add_var() failed (expected)");
		//add new block
		error = add_block();
			err(error, "add_block() failed");
			//-------------------------------
			//string zzz;
			error = add_var(TYPE_STRING, "zzz");
			err(error, "add_var() failed");
			error = assign_type_compare("zzz", TYPE_STRING);
			err(error, "assign_type_compare() failed");
			error = set_var_value_string("zzz", "lorem ipsum dolor sit amet");
			err(error, "set_var_value_string() failed");
			printf("Test: get_var_type(\"zzz\"): %s (STRING expected)\n", (get_var_type("zzz")==3)?"STRING":((get_var_type("zzz")==2)?"DOUBLE":((get_var_type("zzz")==1)?"INT":"XXX")));
			//int i;
			error = add_var(TYPE_INT, "i");
			err(error, "add_var() failed;");
			//double pom = 30;
			error = add_var(TYPE_DOUBLE, "pom");
			err(error, "add_var() failed");
			error = assign_type_compare("pom", TYPE_INT);
			err(error, "assign_type_compare() failed");
			error = add_block();
				err(error, "add_block() failed");
				//-------------------------------
				printf("Value stored in variable zzz: ");
				printf("\"%s\"\n", ((Tlocal_sym_tab_node *)(st_find_var("zzz", current_func_data->local_sym_table)->data_ptr))->string_val);
				error = remove_block();
			err(error, "remove_block() failed");
			//-------------------------------
			error = remove_block();
		err(error, "remove_block() failed");
		//-------------------------------
		printf("Test: get_var_type(\"zzz\"): %s (DOUBLE expected - is in first block)\n", (get_var_type("zzz")==3)?"STRING":((get_var_type("zzz")==2)?"DOUBLE":((get_var_type("zzz")==1)?"INT":"XXX")));

		printf("Test: get_var(\"test\")\n");
		Tlocal_sym_tab_node *asdf = get_var("test");
		if (asdf == NULL)
		{
			printf("	get_var(\"test\") returned NULL (error)\n");
		}
		else
		{
			printf("	Got var name: %s\n", asdf->name);
			printf("	Got var type %d\n", asdf->data_type);
			printf("	Got var defined: %d\n", asdf->defined);
		}

		printf("Test get_var(\"non_existing\")\n");
		asdf = get_var("non_existing");
		if (asdf == NULL)
		{
			printf("	get_var(\"non_existing\") returned NULL (expected)\n");
		}
		else
		{
			printf("	Got var name: %s\n", asdf->name);
			printf("	Got var type %d\n", asdf->data_type);
			printf("	Got var defined: %d\n", asdf->defined);
		}

		error = remove_block();
	err(error, "remove_block() failed");
	//-------------------------------

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 10: Test functions which should be used inside of function only\n");
	printf(
		"--------------------------------------------------------------\n"
		"double asdf;\n"
		"asdf = 42;\n"
		"{\n"
		"}\n"
		"--------------------------------------------------------------\n"
	);
	printf("---4 errors expected:---\n");

	printf("Test: var_exists(\"zzz\"): %s (false expected)\n", var_exists("zzz")?"true":"false");
	printf("Test: is_var_defined(\"test\"): %s (false expected)\n", is_var_defined("test")?"true":"false");
	printf("Test: get_var_type(\"lorem\"): %s (XXX expected)\n", (get_var_type("lorem")==3)?"STRING":((get_var_type("lorem")==2)?"DOUBLE":((get_var_type("lorem")==1)?"INT":"XXX")));

	error = add_var(TYPE_DOUBLE, "asdf");
	err(error, "add_var() failed (expected)");
	error = assign_type_compare("asdf", TYPE_INT);
	err(error, "assign_type_compare() failed (expected)");
	error = add_block();
	err(error, "add_block() failed (expected)");
	error = remove_block();
	err(error, "remove_block() failed (expected)");

	printf("Test: get_var(\"test\")\n");
	Tlocal_sym_tab_node *qwer = get_var("test");
	if (qwer == NULL)
	{
		printf("	get_var(\"test\") returned NULL\n");
	}
	else
	{
		printf("	Got var name: %s\n", qwer->name);
		printf("	Got var type %d\n", qwer->data_type);
		printf("	Got var defined: %d\n", qwer->defined);
	}

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 11: Add new function declaration inside of another function definition and another function declaration outside of function\n");
	printf(
		"--------------------------------------------------------------\n"
		"int new_func()\n"
		"{\n"
		"	int func_inside_of_func();\n"
		"}\n"
		"\n"
		"int func_outside_of_func();\n"
		"--------------------------------------------------------------\n"
	);
	printf("---2 errors expected:---\n");
	//add function definition
	error = add_func(TYPE_INT, "new_func");
	err(error, "add_func() failed");
	error = func_definition(test_start_address);
		err(error, "func_definition() failed");

		//add function declaration
		error = add_func(TYPE_INT, "func_inside_of_func");
		err(error, "add_func() failed (expected)");
		error = func_declaration();
		err(error, "func_declaration() failed (expected)");

		error = remove_block();
	err(error, "remove_block() failed");

	//add function declaration
	error = add_func(TYPE_INT, "func_outside_of_func");
	err(error, "add_func() failed");
	error = func_declaration();
	err(error, "func_declaration() failed");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST 12: Add new function declaration with parameter using same identifier as function\n");
	printf(
		"--------------------------------------------------------------\n"
		"double same_id(int same_id);\n"
		"--------------------------------------------------------------\n"
	);
	printf("---1 errors expected:---\n");
	//add function definition
	error = add_func(TYPE_DOUBLE, "same_id");
	err(error, "add_func() failed");
	error = add_param(TYPE_INT, "same_id");
	err(error, "add_param() failed (expected)");
	error = func_declaration();
	err(error, "func_declaration() failed");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("\nTEST x: Other tests\n");
	printf("---2 errors expected:---\n");

	printf("Test: get_func(\"func_with_param\")\n");
	Tglobal_sym_tab_node *global_asdf = get_func("func_with_param");
	if (global_asdf == NULL)
	{
		printf("	get_func(func_with_param) returned NULL (error)\n");
	}
	else
	{
		printf("	Got func type %d\n", global_asdf->data_type);
		printf("	Got func defined: %d\n", global_asdf->defined);
	}
	printf("Test: get_func(\"notexisting\")\n");
	global_asdf = get_func("notexisting");
	if (global_asdf == NULL)
	{
		printf("	get_func(\"notexisting\") returned NULL (expected)\n");
	}
	else
	{
		printf("	Got func type %d\n", global_asdf->data_type);
		printf("	Got func defined: %d\n", global_asdf->defined);
	}

	printf("\nTry all possible combinations of comparisson:\n");
	error = type_compare(TYPE_INT, TYPE_INT);
	err(error, "Compare types : int-int");
	error = type_compare(TYPE_DOUBLE, TYPE_DOUBLE);
	err(error, "Compare types : double-double");
	error = type_compare(TYPE_STRING, TYPE_STRING);
	err(error, "Compare types : string-string");
	error = type_compare(TYPE_INT, TYPE_DOUBLE);
	err(error, "Compare types : int-double");
	error = type_compare(TYPE_DOUBLE, TYPE_STRING);
	err(error, "Compare types : double-string");
	error = type_compare(TYPE_INT, TYPE_STRING);
	err(error, "Compare types : int-string");

	printf("\n");

	printf("Test: func_exists(\"func1\"): %s (true expected)\n", func_exists("func1")?"true":"false");
	printf("Test: func_exists(\"lorem\"): %s (false expected)\n", func_exists("lorem")?"true":"false");
	printf("Test: func_exists(\"diff_type\"): %s (true expected)\n", func_exists("diff_type")?"true":"false");

	printf("\n");

	printf("Print parameter names of func_with_param()\n");
	Tglobal_sym_tab_node *func_address = get_func("func_with_param");
	Tlocal_sym_tab_node *var = get_nth_param(func_address, 1);
	if (var != NULL)
		printf("First param: %s\n", var->name);
	else
		printf("First parameter not found\n");
	var = get_nth_param(func_address, 2);
	if (var != NULL)
		printf("Second param: %s\n", var->name);
	else
		printf("Second parameter not found\n");
	var = get_nth_param(func_address, 3);
	if (var != NULL)
		printf("Third param: %s\n", var->name);
	else
		printf("Third parameter not found\n");

	printf("\n\n\n");
	printf("Print info about all functions:\n");
	char *test_string;
	//0 - main
	test_string = "main";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n",  (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n",  (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n",  is_func_defined(test_string)?"true":"false");
	//1
	test_string = "func1";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n",  (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n",  (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n",  is_func_defined(test_string)?"true":"false");
	//
	test_string = "func_with_param";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n",  (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n",  (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n",  is_func_defined(test_string)?"true":"false");
	//
	test_string = "func_with_different_param";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n",  (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n",  (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n",  is_func_defined(test_string)?"true":"false");
	//
	test_string = "func_with_different_param_type";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n",  (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n",  (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n",  is_func_defined(test_string)?"true":"false");
	//
	test_string = "diff_param_count";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n",  (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n",  (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n",  is_func_defined(test_string)?"true":"false");
	//
	test_string = "diff_type";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n",  (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n",  (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n",  is_func_defined(test_string)?"true":"false");
	//
	test_string = "redefinition";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n", (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n", (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n", is_func_defined(test_string)?"true":"false");
	//
	test_string = "vars_blocks";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n", (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n", (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n", is_func_defined(test_string)?"true":"false");
	//
	test_string = "new_func";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n", (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n", (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n", is_func_defined(test_string)?"true":"false");
	//
	test_string = "func_outside_of_func";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n", (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n", (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n", is_func_defined(test_string)?"true":"false");
	//
	test_string = "same_id";
	printf("Function: %s\n", test_string);
	printf("	Start address: %u\n", (get_func(test_string)!=NULL)?get_func(test_string)->func_start:0);
	printf("	Data type: %s\n", (get_func_type(test_string)==3)?"STRING":((get_func_type(test_string)==2)?"DOUBLE":((get_func_type(test_string)==1)?"INT":"XXX")));
	printf("	Is function defined: %s\n\n", is_func_defined(test_string)?"true":"false");

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("Check if every declared function and main() is defined:\n");
	error = every_func_defined();
	if (error == OK)
	{
		printf("	- OK\n");
	}
	else
	{
		printf("	- ERROR - main() or any other declared function definition not found\n");
	}

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("PRINT GLOBAL SYMBOL TABLE\n");
	print_tree(global_sym_table->root);

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	printf("Free global symbol table including all data\n");
	free_global_symbol_table();

	printf("\n-----------------------------------------------------------------------------------------------------------------------------------------\n");

	return 0;
}
