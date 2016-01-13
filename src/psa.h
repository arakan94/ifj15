/******** psa.h *********
 *
 * FIT VUT, IFJ 089
 * Author: Martin Trbola, xtrbol00
 * Summary: Precedence analysis
 *
 */

#ifndef PREC_H_INCLUDED
#define PREC_H_INCLUDED

#include "psa_stack.h"
#include "semantic.h"

int tmp_var(Tdata_type type, Tlocal_sym_tab_node **var);

enum op decode_token();

int ps_push_token(Tpsa_stack *psa_stack);

char get_prec(enum op op);

int psa(Tpsa_stack *psa_stack);

#endif // PREC_H_INCLUDED
