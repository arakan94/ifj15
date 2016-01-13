/******** tac.h *********
 *
 * FIT VUT, IFJ 089
 * Authors:
 *    Marek Majer, xmajer15
 *    Martin Trbola, xtrbol00
 * Summary: Three-address code and interpret
 *
 */

#ifndef TAC_H_INCLUDED
#define TAC_H_INCLUDED

#include "functions.h"

#define MALLOCINITIAL (64)

typedef struct T3ac {
	i_func f;
	void *op1;
	void *op2;
	void *dest;
} T3ac;

T3ac *ta_table;
unsigned maxline;
unsigned ta_size;

unsigned current_line;

typedef struct Tframe {
	unsigned return_line;
	Tl_var *return_value;

	unsigned var_count;

	Tl_var var[];
} Tframe;

Tstack *frame_stack;

Tframe *prep_frame;
Tl_var prep_var;

Tframe *new_frame();
int add_frame(Tframe *frame, unsigned return_line, Tl_var *return_value);
void destroy_frame(Tframe *frame);
void rm_frame();

int init_tac();
void free_tac();

int run_tac(unsigned start, unsigned var_count);

#endif // TAC_H_INCLUDED
