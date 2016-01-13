/******** tac.c *********
 *
 * FIT VUT, IFJ 089
 * Authors:
 *    Marek Majer, xmajer15
 *    Martin Trbola, xtrbol00
 * Summary: Three-address code and interpret
 *
 */

#include "tac.h"
#include "stack.h"

Tframe *new_frame(unsigned var_count) {
	Tframe *frame = calloc(sizeof(Tframe) + var_count * sizeof(Tl_var), 1);

	if (frame == NULL)
		return NULL;

	frame->var_count = var_count;

	return frame;
}

int add_frame(Tframe *frame, unsigned return_line, Tl_var *return_value) {
	if (frame == NULL)
		return ERR_INTERPRET;

	if (stack_push(frame_stack, frame) != OK) {
		free(frame);

		return ERR_INTERPRET;
	}

	frame->return_line = return_line;
	frame->return_value = return_value;

	return OK;
}

void destroy_frame(Tframe *frame) {
	if (frame == NULL)
		return;

	for (unsigned i = 0; i < frame->var_count; i++) {
		if (frame->var[i].type == TYPE_STRING)
			free(frame->var[i].s);
	}

	free(frame);
}

void rm_frame() {
	destroy_frame(stack_pop(frame_stack));
}

int init_tac() {
	f_list = malloc(FUNCTIONCOUNT * sizeof(Tlist_function));

	if (f_list == NULL)
		return ERR_INTERPRET;

	flist_fill();

	ta_size = MALLOCINITIAL;
	ta_table = malloc(ta_size * sizeof(T3ac));

	maxline = 0;

	if (ta_table == NULL)
		return ERR_INTERPRET;

	frame_stack = stack_init();

	if (frame_stack == NULL)
		return ERR_INTERPRET;

	prep_frame = NULL;
	prep_var.defined = 1;
	prep_var.type = TYPE_INT;

	run_error = OK;

	return OK;
}

void free_tac() {
	free(f_list);
	free(ta_table);

	while (!stack_empty(frame_stack)) {
		rm_frame();
	}
	stack_dispose(frame_stack);

	destroy_frame(prep_frame);
}



static Tl_var *decode_var(Tlocal_sym_tab_node *ptr, Tframe *frame) {
	if (ptr == NULL)
		return NULL;

	Tl_var *var = frame->var + ptr->offset;

	if (!var->defined) {
		var->type = ptr->data_type;

		if (ptr->defined) {
			var->defined = 1;

			if (ptr->data_type == TYPE_INT)
				var->i = ptr->int_val;
			else if (ptr->data_type == TYPE_DOUBLE)
				var->d = ptr->double_val;
			else if (ptr->data_type == TYPE_STRING) {
				unsigned len = strlen(ptr->string_val);
				var->s = malloc(len + 1);

				if (var->s == NULL) {
					run_error = ERR_INTERPRET;
					return var;
				}

				memcpy(var->s, ptr->string_val, len + 1);
			}
		}
	}

	//if (var != NULL)
	//	printf("%p:%p (%d, %p)\n", frame, var, var->defined, var->s);

	return var;
}

static inline Tl_var *decode_op1(void *ptr) {
	i_func f = ta_table[current_line].f;
	if ((f != call_function) && (f != prepare_frame))
		return decode_var(ptr, stack_top(frame_stack));

	if (f == call_function)
		prep_var.u = ((Tglobal_sym_tab_node*)ptr)->func_start;
	else if (f == prepare_frame)
		prep_var.u = ((Tglobal_sym_tab_node*)ptr)->var_count;

	return &prep_var;
}

static inline Tl_var *decode_op2(void *ptr) {
	return decode_var(ptr, stack_top(frame_stack));
}

static inline Tl_var *decode_dest(void *ptr) {
	if ((prep_frame == NULL) ||
		((ta_table[current_line].f != assign_i) &&
		(ta_table[current_line].f != assign_d) &&
		(ta_table[current_line].f != assign_str)))
		return decode_var(ptr, stack_top(frame_stack));

	return decode_var(ptr, prep_frame);
}

int run_tac(unsigned start, unsigned var_count) {
	Tl_var result;
	int err = add_frame(new_frame(var_count), 0, &result);
	if (err != OK)
		return err;

	current_line = start;

	//printf("Interpreting from %d:\n", start);

	while ((current_line < maxline) && (current_line > 8)) {
		//printf(":%d:\n", current_line);

		Tl_var *op1 = decode_op1(ta_table[current_line].op1);
		Tl_var *op2 = decode_op2(ta_table[current_line].op2);
		Tl_var *dest = decode_dest(ta_table[current_line].dest);

		if (run_error != OK)
			return run_error;

		if ((op1 != NULL) && (!op1->defined)) {
			fprintf(stderr, "Uninitialized variable '%s'\n",
				((Tlocal_sym_tab_node*)ta_table[current_line].op1)->name);
			return ERR_VARINIT;
		}

		if ((op2 != NULL) && (!op2->defined)) {
			fprintf(stderr, "Uninitialized variable '%s'\n",
				((Tlocal_sym_tab_node*)ta_table[current_line].op1)->name);
			return ERR_VARINIT;
		}

		if (dest != NULL)
			dest->defined = 1;

		i_func f = ta_table[current_line].f;

		if (f == NULL) {
			fprintf(stderr, "Unknown instruction\n");
			return ERR_RUN;
		}

		current_line++;

		f(op1, op2, dest);

		if (run_error != OK)
			return run_error;
	}

	return OK;
}
