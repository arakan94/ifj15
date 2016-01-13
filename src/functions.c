/******** functions.c *********
 *
 * FIT VUT, IFJ 089
 * Authors:
 *    Marek Majer, xmajer15
 *    Martin Trbola, xtrbol00
 * Summary: Instruction functions
 *
 */

#include <stdio.h>
#include <ctype.h>
#include "functions.h"
#include "tac.h"
#include "debug.h"
#include "fce.h"
#include "str.h"

// Coversions
void con_i_to_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	dest->d = op1->i;
}

void con_d_to_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	dest->i = op1->d;
}


// Cout
void cout_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	(void)dest;
	printf("%d", op1->i);
}

void cout_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	(void)dest;
	printf("%g", op1->d);
}

void cout_str(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	(void)dest;
	printf("%s", op1->s);
}


// Cin
void cin_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op1;
	(void)op2;

	if (scanf("%d", &dest->i) <= 0) {
		run_error = ERR_NUMBER;
		return;
	}
}
void cin_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op1;
	(void)op2;

	if (scanf("%lf", &dest->d) <= 0) {
		run_error = ERR_NUMBER;
		return;
	}
}

void cin_str(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op1;
	(void)op2;

	string s;
	strInit(&s);

	int c;

	do {
		c = getchar();
	} while ((c != EOF) && (isspace(c)));

	do {
		strAddChar(&s, c);

		c = getchar();
	} while ((c != EOF) && (!isspace(c)));

	free(dest->s);
	dest->s = s.str;
}


// Assign
void assign_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	dest->i = op1->i;
}

void assign_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	dest->d = op1->d;
}

void assign_str(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;

	unsigned len = strlen(op1->s);
	char *tmp = malloc(len + 1);

	if (tmp == NULL) {
		run_error = ERR_INTERPRET;
		return;
	}
	memcpy(tmp, op1->s, len + 1);

	free(dest->s);
	dest->s = tmp;
}


// Arithmetic
void add_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->i + op2->i;
}

void add_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->d = op1->d + op2->d;
}

void sub_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->i - op2->i;
}

void sub_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->d = op1->d - op2->d;
}

void mul_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->i * op2->i;
}

void mul_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->d = op1->d * op2->d;
}

void div_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	if (op2->i == 0) {
		fprintf(stderr, "Division by zero\n");
		run_error = ERR_ZERODIV;
		return;
	}

	dest->i = op1->i / op2->i;
}

void div_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	if (op2->d == 0) {
		fprintf(stderr, "Division by zero\n");
		run_error = ERR_ZERODIV;
		return;
	}

	dest->d = op1->d / op2->d;
}


// Logical
void equal_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->i == op2->i;
}

void equal_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->d == op2->d;
}

void equal_str(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = strcmp(op1->s, op2->s) == 0;
}

void nequal_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->i != op2->i;
}

void nequal_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->d != op2->d;
}

void nequal_str(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = strcmp(op1->s, op2->s) != 0;
}

void greater_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->i > op2->i;
}

void greater_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->d > op2->d;
}

void greater_str(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = strcmp(op1->s, op2->s) > 0;
}

void greater_equal_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->i >= op2->i;
}

void greater_equal_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->d >= op2->d;
}

void greater_equal_str(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = strcmp(op1->s, op2->s) >= 0;
}

void lesser_equal_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->i <= op2->i;
}

void lesser_equal_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->d <= op2->d;
}

void lesser_equal_str(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = strcmp(op1->s, op2->s) <= 0;
}

void lesser_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->i < op2->i;
}

void lesser_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = op1->d < op2->d;
}

void lesser_str(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	dest->i = strcmp(op1->s, op2->s) < 0;
}


// Jumps
void go_to(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op1;
	(void)dest;
	current_line = op2->i;
}

void go_to_true(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)dest;
	if (op1->i)
		current_line = op2->i;
}

void go_to_false(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)dest;
	if (!op1->i)
		current_line = op2->i;
}

// Functions
void prepare_frame(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	(void)dest;

	prep_frame = new_frame(op1->u);
	if (prep_frame == NULL) {
		fprintf(stderr, "Allocation error\n");
		run_error = ERR_INTERPRET;
	}
}

void call_function(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;

	if (op1->u > 8) {
		run_error = add_frame(prep_frame, current_line, dest);

		current_line = op1->u;

		prep_frame = NULL;
	} else { // builtin functions
		Tl_var *var = prep_frame->var;

		switch (op1->u) {
			case BIN_LENGTH:
				dest->i = length(var[0].s);
			break;

			case BIN_SUBSTR:
				free(dest->s);
				dest->s = substr(var[0].s, var[1].i, var[2].i);

				if (dest->s == NULL)
					run_error = ERR_RUN;
			break;

			case BIN_CONCAT:
				free(dest->s);
				dest->s = concat(var[0].s, var[1].s);

				if (dest->s == NULL)
					run_error = ERR_RUN;
			break;

			case BIN_FIND:
				dest->i = find(var[0].s, var[1].s);
			break;

			case BIN_SORT:
				free(dest->s);
				dest->s = sort(var[0].s);

				if (dest->s == NULL)
					run_error = ERR_RUN;
			break;
		}

		destroy_frame(prep_frame);
		prep_frame = NULL;
	}
}

void return_function_i(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	(void)dest;

	Tframe *frame = stack_top(frame_stack);

	frame->return_value->i = op1->i;
	current_line = frame->return_line;

	rm_frame();
}

void return_function_d(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	(void)dest;

	Tframe *frame = stack_top(frame_stack);

	frame->return_value->d = op1->d;
	current_line = frame->return_line;

	rm_frame();
}

void return_function_str(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op2;
	(void)dest;

	Tframe *frame = stack_top(frame_stack);

	free(frame->return_value->s);

	unsigned len = strlen(op1->s);
	frame->return_value->s = malloc(len + 1);

	if (frame->return_value->s == NULL) {
		run_error = ERR_INTERPRET;
		return;
	}

	memcpy(frame->return_value->s, op1->s, len + 1);

	current_line = frame->return_line;

	rm_frame();
}

void nop(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op1;
	(void)op2;
	(void)dest;
}

void no_return(Tl_var *op1, Tl_var *op2, Tl_var *dest) {
	(void)op1;
	(void)op2;
	(void)dest;

	fprintf(stderr, "No return\n");
	run_error = ERR_VARINIT;
}



static unsigned line = 9;

unsigned get_line() {
	return line;
}

unsigned generate(opType operation, void *op1, void *op2, void *dest) {
	//printf("%d: %s\t(%p, %p => %p)\n", line, op_name[operation], op1, op2, dest);

	if (line == ta_size) {
		ta_size = ta_size * 2;
		ta_table = realloc(ta_table, ta_size * sizeof(T3ac));

		if (ta_table == NULL)
			return 0;
	}

	Tdata_type op1_type, op2_type, dest_type;

	if (op1 != NULL) {
		if ((operation != I_PREPARE) && (operation != I_CALL)) {
			op1_type = ((Tlocal_sym_tab_node*)op1)->data_type;
		} else {
			op1_type = TYPE_INT;
		}
	} else {
		op1_type = TYPE_NON;
	}

	if (op2 != NULL) {
		op2_type = ((Tlocal_sym_tab_node*)op2)->data_type;
	} else {
		op2_type = TYPE_NON;
	}

	if (dest != NULL) {
		dest_type = ((Tlocal_sym_tab_node*)dest)->data_type;
	} else {
		dest_type = TYPE_NON;
	}

	i_func f = find_function(operation, op1_type, op2_type, dest_type);

	ta_table[line].f = f;
	ta_table[line].op1 = op1;
	ta_table[line].op2 = op2;
	ta_table[line].dest = dest;

	maxline = ++line;

	if (f == NULL)
		return 0;

	return line;
}

i_func find_function(opType operation, Tdata_type op1, Tdata_type op2, Tdata_type dest) {
	for (int i = 0; i < FUNCTIONCOUNT; i++) {
		if ((f_list[i].operation == operation) &&
			(f_list[i].op1 == op1) &&
			(f_list[i].op2 == op2) &&
			(f_list[i].dest == dest)) {

			return f_list[i].f;
		}
	}

	return NULL;
}


static void flist_insert(opType operation, Tdata_type op1, Tdata_type op2, Tdata_type dest, i_func f) {
	static unsigned n = 0;

	f_list[n].operation = operation;
	f_list[n].op1 = op1;
	f_list[n].op2 = op2;
	f_list[n].dest = dest;
	f_list[n].f = f;

	n++;
}

void flist_fill() {
	flist_insert(I_COUT,     TYPE_INT,    TYPE_NON,    TYPE_NON,    cout_i);
	flist_insert(I_COUT,     TYPE_DOUBLE, TYPE_NON,    TYPE_NON,    cout_d);
	flist_insert(I_COUT,     TYPE_STRING, TYPE_NON,    TYPE_NON,    cout_str);
	flist_insert(I_CIN,      TYPE_NON,    TYPE_NON,    TYPE_INT,    cin_i);
	flist_insert(I_CIN,      TYPE_NON,    TYPE_NON,    TYPE_DOUBLE, cin_d);
	flist_insert(I_CIN,      TYPE_NON,    TYPE_NON,    TYPE_STRING, cin_str);

	flist_insert(I_CONVERT,  TYPE_INT,    TYPE_NON,    TYPE_DOUBLE, con_i_to_d);
	flist_insert(I_CONVERT,  TYPE_DOUBLE, TYPE_NON,    TYPE_INT,    con_d_to_i);
	flist_insert(I_ASSIGN,   TYPE_INT,    TYPE_NON,    TYPE_INT,    assign_i);
	flist_insert(I_ASSIGN,   TYPE_DOUBLE, TYPE_NON,    TYPE_DOUBLE, assign_d);
	flist_insert(I_ASSIGN,   TYPE_STRING, TYPE_NON,    TYPE_STRING, assign_str);

	flist_insert(I_ADD,      TYPE_INT,    TYPE_INT,    TYPE_INT,    add_i);
	flist_insert(I_ADD,      TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, add_d);
	flist_insert(I_SUB,      TYPE_INT,    TYPE_INT,    TYPE_INT,    sub_i);
	flist_insert(I_SUB,      TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, sub_d);
	flist_insert(I_MUL,      TYPE_INT,    TYPE_INT,    TYPE_INT,    mul_i);
	flist_insert(I_MUL,      TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, mul_d);
	flist_insert(I_DIV,      TYPE_INT,    TYPE_INT,    TYPE_INT,    div_i);
	flist_insert(I_DIV,      TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, div_d);

	flist_insert(I_E,        TYPE_INT,    TYPE_INT,    TYPE_INT,    equal_i);
	flist_insert(I_E,        TYPE_DOUBLE, TYPE_DOUBLE, TYPE_INT,    equal_d);
	flist_insert(I_E,        TYPE_STRING, TYPE_STRING, TYPE_INT,    equal_str);
	flist_insert(I_NE,       TYPE_INT,    TYPE_INT,    TYPE_INT,    nequal_i);
	flist_insert(I_NE,       TYPE_DOUBLE, TYPE_DOUBLE, TYPE_INT,    nequal_d);
	flist_insert(I_NE,       TYPE_STRING, TYPE_STRING, TYPE_INT,    nequal_str);
	flist_insert(I_GE,       TYPE_INT,    TYPE_INT,    TYPE_INT,    greater_equal_i);
	flist_insert(I_GE,       TYPE_DOUBLE, TYPE_DOUBLE, TYPE_INT,    greater_equal_d);
	flist_insert(I_GE,       TYPE_STRING, TYPE_STRING, TYPE_INT,    greater_equal_str);
	flist_insert(I_LE,       TYPE_INT,    TYPE_INT,    TYPE_INT,    lesser_equal_i);
	flist_insert(I_LE,       TYPE_DOUBLE, TYPE_DOUBLE, TYPE_INT,    lesser_equal_d);
	flist_insert(I_LE,       TYPE_STRING, TYPE_STRING, TYPE_INT,    lesser_equal_str);
	flist_insert(I_L,        TYPE_INT,    TYPE_INT,    TYPE_INT,    lesser_i);
	flist_insert(I_L,        TYPE_DOUBLE, TYPE_DOUBLE, TYPE_INT,    lesser_d);
	flist_insert(I_L,        TYPE_STRING, TYPE_STRING, TYPE_INT,    lesser_str);
	flist_insert(I_G,        TYPE_INT,    TYPE_INT,    TYPE_INT,    greater_i);
	flist_insert(I_G,        TYPE_DOUBLE, TYPE_DOUBLE, TYPE_INT,    greater_d);
	flist_insert(I_G,        TYPE_STRING, TYPE_STRING, TYPE_INT,    greater_str);

	flist_insert(I_PREPARE,  TYPE_INT,    TYPE_NON,    TYPE_NON,    prepare_frame);
	flist_insert(I_CALL,     TYPE_INT,    TYPE_NON,    TYPE_INT,    call_function);
	flist_insert(I_CALL,     TYPE_INT,    TYPE_NON,    TYPE_DOUBLE, call_function);
	flist_insert(I_CALL,     TYPE_INT,    TYPE_NON,    TYPE_STRING, call_function);

	flist_insert(I_RETURN,   TYPE_INT,    TYPE_NON,    TYPE_NON,    return_function_i);
	flist_insert(I_RETURN,   TYPE_DOUBLE, TYPE_NON,    TYPE_NON,    return_function_d);
	flist_insert(I_RETURN,   TYPE_STRING, TYPE_NON,    TYPE_NON,    return_function_str);

	flist_insert(I_JUMP,     TYPE_NON,    TYPE_INT,    TYPE_NON,    go_to);
	flist_insert(I_JUMPIF,   TYPE_INT,    TYPE_INT,    TYPE_NON,    go_to_true);
	flist_insert(I_JUMPIFN,  TYPE_INT,    TYPE_INT,    TYPE_NON,    go_to_false);

	flist_insert(I_ERR,      TYPE_NON,    TYPE_NON,    TYPE_NON,    no_return);
}
