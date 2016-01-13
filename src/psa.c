/******** psa.c *********
 *
 * FIT VUT, IFJ 089
 * Author: Martin Trbola, xtrbol00
 * Summary: Precedence analysis
 *
 */

#include "psa.h"
#include "scanner.h"
#include "error_codes.h"
#include "functions.h"

#define TABLE_SIZE (18)

static const char prec_table[TABLE_SIZE][TABLE_SIZE] = { /*
	 <    >    <=   >=   ==   !=   +    -    *    /    (    )    imm  id   idf  ,    =    $    */
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '>', '<', '<', '<', '>', ' ', '>'}, // <
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '>', '<', '<', '<', '>', ' ', '>'}, // >
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '>', '<', '<', '<', '>', ' ', '>'}, // <=
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '>', '<', '<', '<', '>', ' ', '>'}, // >=
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '>', '<', '<', '<', '>', ' ', '>'}, // ==
	{'>', '>', '>', '>', '>', '>', '<', '<', '<', '<', '<', '>', '<', '<', '<', '>', ' ', '>'}, // !=
	{'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '<', '<', '<', '>', ' ', '>'}, // +
	{'>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '<', '>', '<', '<', '<', '>', ' ', '>'}, // -
	{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '>', ' ', '>'}, // *
	{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '>', ' ', '>'}, // /
	{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '=', ' ', ' '}, // (
	{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', ' ', ' ', '>', ' ', '>'}, // )
	{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', ' ', ' ', '>', ' ', '>'}, // imm
	{'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', ' ', ' ', '>', '=', '>'}, // id
	{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '=', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // idf
	{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '=', ' ', ' '}, // ,
	{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '<', '<', '<', ' ', ' ', '>'}, // =
	{'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', '<', '<', ' ', ' ', ' '}, // $
};




static opType op2instr(enum op op) {
	static opType decode_table[] = {
		[OP_LT] = I_L,
		[OP_GT] = I_G,
		[OP_LE] = I_LE,
		[OP_GE] = I_GE,
		[OP_EQ] = I_E,
		[OP_NEQ] = I_NE,
		[OP_ADD] = I_ADD,
		[OP_SUB] = I_SUB,
		[OP_MUL] = I_MUL,
		[OP_DIV] = I_DIV,
	};

	return decode_table[op];
}

enum op decode_token() {
	static enum op decode_table[] = {
		[TT_L] = OP_LT,
		[TT_G] = OP_GT,
		[TT_LE] = OP_LE,
		[TT_GE] = OP_GE,
		[TT_E] = OP_EQ,
		[TT_NE] = OP_NEQ,
		[TT_ADD] = OP_ADD,
		[TT_SUB] = OP_SUB,
		[TT_MULTIPLY] = OP_MUL,
		[TT_DIVIDE] = OP_DIV,
		[TT_L_BRACKET] = OP_LB,
		[TT_R_BRACKET] = OP_RB,
		[TT_VALUE_INT] = OP_IMM,
		[TT_VALUE_DOUBLE] = OP_IMM,
		[TT_VALUE_STRING] = OP_IMM,
		[TT_ID] = OP_ID,
		[TT_COMMA] = OP_COMMA,
		[TT_ASSIGN] = OP_ASSIGN,
	};

	if ((token.type == TT_L) || (decode_table[token.type] > 0))
		return decode_table[token.type];
	else
		return OP_END;
}

int tmp_var(Tdata_type type, Tlocal_sym_tab_node **var) {
	static unsigned tmp_id = 0;

	char tmp[32];

	sprintf(tmp, "@T%d", tmp_id);

	int err = add_var(type, tmp);
	if (err != OK)
		return err;

	*var = get_var(tmp);

	tmp_id++;

	return OK;
}

int ps_push_token(Tpsa_stack *psa_stack) {
	int err = ps_push(psa_stack, decode_token());
	if (err != OK)
		return err;

	Tps_item *item = ps_top(psa_stack);
	if (token.type == TT_ID) {
		if (var_exists(token.p_string)) {
			Tlocal_sym_tab_node *var = get_var(token.p_string);

			item->type = var->data_type;
			item->value.ptr = var;
		} else if (func_exists(token.p_string)) {
			Tglobal_sym_tab_node *fce = get_func(token.p_string);

			item->op = OP_IDF;
			item->type = fce->data_type;
			item->value.ptr = fce;
		} else {
			return ERR_UNDEF;
		}
	} else if (token.type == TT_VALUE_INT) {
		item->type = TYPE_INT;
		item->value.i = token.value_int;
	} else if (token.type == TT_VALUE_DOUBLE) {
		item->type = TYPE_DOUBLE;
		item->value.d = token.value_double;
	} else if (token.type == TT_VALUE_STRING) {
		item->type = TYPE_STRING;

		int slen = strlen(token.p_string);

		item->value.s = malloc(slen + 1);
		if (item->value.s == NULL)
			return ERR_INTERPRET;

		if (err != OK)
			return err;

		memcpy(item->value.s, token.p_string, slen + 1);
	}

	return OK;
}

char get_prec(enum op op) {
	return prec_table[op][decode_token()];
}

int psa(Tpsa_stack *psa_stack) {
	psa_stack->active = NULL;

	while (ps_prev(psa_stack)->op != OP_EXPR)
		;

	Tps_item tmp_item;
	int err;
	enum op op;
	Tps_item new_item;
	Tlocal_sym_tab_node *var, *e1, *e2;

	Tps_item *item = ps_next(psa_stack);
	switch (item->op) {
		case OP_NTERM: // E op E
			tmp_item.type = item->type;
			tmp_item.value = item->value;

			item = ps_next(psa_stack);
			if (item == NULL)
				return ERR_PARSER;
			if (item->op > OP_DIV)
				return ERR_PARSER;

			op = item->op;

			item = ps_next(psa_stack);
			if (item == NULL)
				return ERR_PARSER;
			if (item->op != OP_NTERM)
				return ERR_PARSER;

			err = type_compare(tmp_item.type, item->type);
			if (err != OK)
				return err;

			if (op <= OP_NEQ) {
				new_item.type = TYPE_INT;
			} else {
				if (tmp_item.type == TYPE_DOUBLE || item->type == TYPE_DOUBLE)
					new_item.type = TYPE_DOUBLE;
				else
					new_item.type = tmp_item.type;
			}

			if (tmp_item.type != item->type) {
				if (tmp_item.type == TYPE_DOUBLE) {
					e1 = tmp_item.value.ptr;

					err = tmp_var(TYPE_DOUBLE, &e2);
					if (err != OK)
						return err;

					generate(I_CONVERT, item->value.ptr, NULL, e2);
				} else if (item->type == TYPE_DOUBLE) {
					err = tmp_var(TYPE_DOUBLE, &e1);
					if (err != OK)
						return err;

					generate(I_CONVERT, tmp_item.value.ptr, NULL, e1);

					e2 = item->value.ptr;
				} else {
					return ERR_PARSER;
				}
			} else {
				e1 = tmp_item.value.ptr;
				e2 = item->value.ptr;
			}

			err = tmp_var(new_item.type, &var);
			if (err != OK)
				return err;

			new_item.value.ptr = var;

			if (generate(op2instr(op), e1, e2, var) == 0) {
				fprintf(stderr, "Invalid operation and operands\n");
				return ERR_TYPE;
			}

			if (ps_next(psa_stack) != NULL)
				return ERR_PARSER;
		break;

		case OP_LB: // (E)
			item = ps_next(psa_stack);
			if (item == NULL)
				return ERR_PARSER;
			if (item->op != OP_NTERM)
				return ERR_PARSER;

			new_item.type = item->type;
			new_item.value = item->value;

			item = ps_next(psa_stack);
			if (item == NULL)
				return ERR_PARSER;
			if (item->op != OP_RB)
				return ERR_PARSER;

			if (ps_next(psa_stack) != NULL)
				return ERR_PARSER;
		break;

		case OP_IMM: // imm
			new_item.type = item->type;

			err = tmp_var(new_item.type, &var);
			if (err != OK)
				return err;

			new_item.value.ptr = var;

			switch (item->type) {
				case TYPE_INT:
					err = set_var_value_int(var->name, item->value.i);
					if (err != OK)
						return err;
				break;

				case TYPE_DOUBLE:
					err = set_var_value_double(var->name, item->value.d);
					if (err != OK)
						return err;
				break;

				case TYPE_STRING:
					err = set_var_value_string(var->name, item->value.s);
					if (err != OK)
						return err;
				break;

				default:
					return ERR_PARSER;
				break;
			}

			if (ps_next(psa_stack) != NULL)
				return ERR_PARSER;
		break;

		case OP_ID: // id or id = E
			new_item.type = item->type;
			new_item.value.ptr = item->value.ptr;

			item = ps_next(psa_stack);
			if (item != NULL) {
				if (item->op == OP_ASSIGN) { // id = E
					item = ps_next(psa_stack);
					if (item == NULL)
						return ERR_PARSER;
					if (item->op != OP_NTERM)
						return ERR_PARSER;

					err = type_compare(new_item.type, item->type);
					if (err != OK)
						return err;

					if (item->type != new_item.type) {
						err = tmp_var(new_item.type, &var);
						if (err != OK)
							return err;

						generate(I_CONVERT, item->value.ptr, NULL, var);
					} else {
						var = item->value.ptr;
					}

					generate(I_ASSIGN, var, NULL, new_item.value.ptr);

					item = ps_next(psa_stack);
				}
			}

			if (item != NULL)
				return ERR_PARSER;
		break;

		case OP_IDF: // id(E, ..., E)
			generate(I_PREPARE, item->value.ptr, NULL, NULL);

			new_item.type = item->type;
			new_item.value.ptr = item->value.ptr;

			item = ps_next(psa_stack);
			if (item == NULL)
				return ERR_PARSER;
			if (item->op != OP_LB)
				return ERR_PARSER;

			item = ps_next(psa_stack);
			if (item == NULL)
				return ERR_PARSER;

			int n = 1;

			if (item->op != OP_RB) {
				if (item->op != OP_NTERM)
					return ERR_PARSER;

				var = get_nth_param(new_item.value.ptr, n);
				if (var == NULL)
					return ERR_TYPE;
				err = type_compare(var->data_type, item->type);
				if (err != OK)
					return err;
				n++;

				if (item->type != var->data_type) {
					err = tmp_var(var->data_type, &e1);
					if (err != OK)
						return err;

					generate(I_CONVERT, item->value.ptr, NULL, e1);
				} else {
					e1 = item->value.ptr;
				}

				generate(I_ASSIGN, e1, NULL, var);

				item = ps_next(psa_stack);
				if (item == NULL)
					return ERR_PARSER;

				while (item->op == OP_COMMA) {
					item = ps_next(psa_stack);
					if (item == NULL)
						return ERR_PARSER;
					if (item->op != OP_NTERM)
						return ERR_PARSER;

					var = get_nth_param(new_item.value.ptr, n);
					if (var == NULL)
						return ERR_TYPE;
					err = type_compare(var->data_type, item->type);
					if (err != OK)
						return err;
					n++;

					if (item->type != var->data_type) {
						err = tmp_var(var->data_type, &e1);
						if (err != OK)
							return err;

						generate(I_CONVERT, item->value.ptr, NULL, e1);
					} else {
						e1 = item->value.ptr;
					}

					generate(I_ASSIGN, e1, NULL, var);

					item = ps_next(psa_stack);
					if (item == NULL)
						return ERR_PARSER;
				}
				if (item->op != OP_RB)
					return ERR_PARSER;
			}

			var = get_nth_param(new_item.value.ptr, n);
			if (var != NULL)
				return ERR_TYPE;

			err = tmp_var(new_item.type, &var);
			if (err != OK)
				return err;

			generate(I_CALL, new_item.value.ptr, NULL, var);

			new_item.value.ptr = var;

			if (ps_next(psa_stack) != NULL)
				return ERR_PARSER;
		break;

		default:
			return ERR_PARSER;
	}

	ps_top(psa_stack);
	while (ps_active(psa_stack)->op != OP_EXPR) {
		ps_pop(psa_stack);
		ps_top(psa_stack);
	}
	ps_pop(psa_stack);

	err = ps_push(psa_stack, OP_NTERM);
	if (err != OK)
		return err;

	item = ps_top(psa_stack);
	item->type = new_item.type;
	item->value = new_item.value;

	return OK;
}
