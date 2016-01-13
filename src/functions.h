/******** functions.h *********
 *
 * FIT VUT, IFJ 089
 * Authors:
 *    Marek Majer, xmajer15
 *    Martin Trbola, xtrbol00
 * Summary: Instruction functions
 *
 */

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include "sym_tab.h"

#define FUNCTIONCOUNT (48)

typedef enum opTypes {
	I_CONVERT,
	I_COUT,
	I_CIN,
	I_ASSIGN,

	I_ADD,
	I_SUB, // 5
	I_MUL,
	I_DIV,

	I_G,
	I_GE,
	I_LE, // 10
	I_L,
	I_E,
	I_NE,

	I_JUMP,
	I_JUMPIF, // 15
	I_JUMPIFN,

	I_PREPARE,
	I_PRECALL,
	I_SET,
	I_CALL, // 20
	I_RETURN,

	I_ADDBLOCK,
	I_RMBLOCK,

	I_ERR,
} opType;

enum builtin {
	BIN_NONE,
	BIN_LENGTH,
	BIN_SUBSTR,
	BIN_CONCAT,
	BIN_FIND,
	BIN_SORT, // 5
};

typedef struct Tl_var {
	union {
		unsigned u;
		int i;
		double d;
		char *s;
	};
	Tdata_type type;
	int defined;
} Tl_var;

typedef void (*i_func)(Tl_var*, Tl_var*, Tl_var*);

typedef struct	{
	opType operation;
	Tdata_type op1;
	Tdata_type op2;
	Tdata_type dest;
	i_func f;
} Tlist_function;

Tlist_function *f_list; //List of functions used in ta_table

void flist_fill();

i_func find_function(opType operation, Tdata_type op1, Tdata_type op2, Tdata_type dest);

int run_error;


unsigned get_line();

unsigned generate(opType operation, void *op1, void *op2, void *dest);

void prepare_frame(Tl_var *op1, Tl_var *op2, Tl_var *dest);
void call_function(Tl_var *op1, Tl_var *op2, Tl_var *dest);
void assign_i(Tl_var *op1, Tl_var *op2, Tl_var *dest);
void assign_d(Tl_var *op1, Tl_var *op2, Tl_var *dest);
void assign_str(Tl_var *op1, Tl_var *op2, Tl_var *dest);

#endif // FUNCTIONS_H_INCLUDED
