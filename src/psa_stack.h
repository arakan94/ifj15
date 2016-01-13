/******** psa_stack.h *********
 *
 * FIT VUT, IFJ 089
 * Author: Martin Trbola, xtrbol00
 * Summary: Stack for precedence analysis
 *
 */

#ifndef PSA_STACK_H_INCLUDED
#define PSA_STACK_H_INCLUDED

#include "semantic.h"

enum op {
	OP_LT,     // <
	OP_GT,     // >
	OP_LE,     // <=
	OP_GE,     // >=
	OP_EQ,     // ==
	OP_NEQ,    // !=
	OP_ADD,    // +
	OP_SUB,    // -
	OP_MUL,    // *
	OP_DIV,    // /
	OP_LB,     // (
	OP_RB,     // )
	OP_IMM,    // imm
	OP_ID,     // id
	OP_IDF,    // idf
	OP_COMMA,  // ,
	OP_ASSIGN, // =
	OP_END,    // $

	OP_NTERM,  // E

	OP_EXPR,   // <
};

typedef struct Tps_item {
	enum op op;
	Tdata_type type;
	union {
		int i;
		double d;
		char *s;
		void *ptr;
	} value;
	struct Tps_item *lptr;
	struct Tps_item *rptr;
} Tps_item;

typedef struct {
	Tps_item *top;
	Tps_item *active;
} Tpsa_stack;

Tpsa_stack *ps_init();
void ps_destroy(Tpsa_stack *psa_stack);

int ps_push(Tpsa_stack *psa_stack, enum op op);
int ps_ins_before(Tps_item *ps_item, enum op op);
int ps_ins_after(Tpsa_stack *psa_stack, Tps_item *ps_item, enum op op);

void ps_pop(Tpsa_stack *psa_stack);

Tps_item *ps_top(Tpsa_stack *psa_stack);
Tps_item *ps_prev(Tpsa_stack *psa_stack);
Tps_item *ps_next(Tpsa_stack *psa_stack);
Tps_item *ps_active(Tpsa_stack *psa_stack);

Tps_item *ps_first_term(Tpsa_stack *psa_stack);

#endif // PSA_STACK_H_INCLUDED
