/******** psa_stack.c *********
 *
 * FIT VUT, IFJ 089
 * Author: Martin Trbola, xtrbol00
 * Summary: Stack for precedence analysis
 *
 */

#include "psa_stack.h"
#include "error_codes.h"

#include <stdlib.h>

Tpsa_stack *ps_init() {
	Tpsa_stack *new_stack = malloc(sizeof(Tpsa_stack));

	if (new_stack == NULL)
		return NULL;

	new_stack->top = NULL;
	new_stack->active = NULL;

	return new_stack;
}

void ps_destroy(Tpsa_stack *psa_stack) {
	while (psa_stack->top != NULL) {
		Tps_item *prev = psa_stack->top->lptr;

		free(psa_stack->top);

		psa_stack->top = prev;
	}

	free(psa_stack);
}

static inline Tps_item *new_ps_item() {
	Tps_item *new_item = malloc(sizeof(Tps_item));

	return new_item;
}

int ps_push(Tpsa_stack *psa_stack, enum op op) {
	Tps_item *new_item = new_ps_item();

	if (new_item == NULL)
		return ERR_PARSER;

	new_item->op = op;

	new_item->lptr = psa_stack->top;
	new_item->rptr = NULL;

	if (psa_stack->top != NULL)
		psa_stack->top->rptr = new_item;

	psa_stack->top = new_item;

	return OK;
}

int ps_ins_before(Tps_item *ps_item, enum op op) {
	Tps_item *new_item = new_ps_item();

	if (new_item == NULL)
		return ERR_PARSER;

	new_item->op = op;

	new_item->lptr = ps_item->lptr;
	new_item->rptr = ps_item;
	ps_item->lptr = new_item;

	if (new_item->lptr != NULL)
		new_item->lptr->rptr = new_item;

	return OK;
}

int ps_ins_after(Tpsa_stack *psa_stack, Tps_item *ps_item, enum op op) {
	Tps_item *new_item = new_ps_item();

	if (new_item == NULL)
		return ERR_PARSER;

	new_item->op = op;

	new_item->rptr = ps_item->rptr;
	new_item->lptr = ps_item;
	ps_item->rptr = new_item;

	if (new_item->rptr != NULL)
		new_item->rptr->lptr = new_item;
	else
		psa_stack->top = new_item;

	return OK;
}

void ps_pop(Tpsa_stack *psa_stack) {
	if (psa_stack->top != NULL) {
		Tps_item *tmp_ptr = psa_stack->top;

		psa_stack->top = tmp_ptr->lptr;

		free(tmp_ptr);
	}
}

Tps_item *ps_top(Tpsa_stack *psa_stack) {
	psa_stack->active = psa_stack->top;
	return psa_stack->top;
}

Tps_item *ps_prev(Tpsa_stack *psa_stack) {
	if (psa_stack->active == NULL) {
		psa_stack->active = psa_stack->top;
		return psa_stack->top;
	}

	psa_stack->active = psa_stack->active->lptr;
	return psa_stack->active;
}

Tps_item *ps_next(Tpsa_stack *psa_stack) {
	if (psa_stack->active == NULL)
		return NULL;

	psa_stack->active = psa_stack->active->rptr;
	return psa_stack->active;
}

Tps_item *ps_active(Tpsa_stack *psa_stack) {
	return psa_stack->active;
}

Tps_item *ps_first_term(Tpsa_stack *psa_stack) {
	if (ps_top(psa_stack)->op <= OP_END)
		return ps_top(psa_stack);

	while (ps_prev(psa_stack)->op > OP_END)
		;

	return ps_active(psa_stack);
}
