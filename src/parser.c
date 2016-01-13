/******** parser.c *********
 *
 * FIT VUT, IFJ 089
 * Author: Martin Trbola, xtrbol00
 * Summary: Parser
 *
 */

#include <string.h>
#include <stdio.h>
#include "parser.h"
#include "scanner.h"
#include "error_codes.h"
#include "semantic.h"
#include "psa.h"
#include "functions.h"
#include "str.h"

#include "debug.h"

static const char *token_[] = {
	[TT_EOF] = "end of file",
	[TT_ID] = "identifier",
	[TT_VALUE_INT] = "integer value",
	[TT_VALUE_DOUBLE] = "double value",
	[TT_VALUE_STRING] = "string value",
//KEYWORDS
	[TT_AUTO] = "auto",
	[TT_INT] = "int",
	[TT_DOUBLE] = "double",
	[TT_STRING] = "string",
	[TT_CIN] = "cin",
	[TT_COUT] = "cout",
	[TT_FOR] = "for",
	[TT_IF] = "if",
	[TT_ELSE] = "else",
	[TT_RETURN] = "return",
//DELIMITERS
	[TT_SEMICOLON] = ";",
	[TT_L_BRACKET] = "(",
	[TT_R_BRACKET] = ")",
	[TT_OPEN_BRACKET] = "{",
	[TT_CLOSE_BRACKET] = "}",
	[TT_COMMA] = ",",
	//APOSTROPHE - not supported
//OPERATORS
	[TT_MULTIPLY] = "*",
	[TT_DIVIDE] = "/",
	[TT_ADD] = "+",
	[TT_SUB] = "-",
	[TT_L] = "<",
	[TT_G] = ">",
	[TT_ASSIGN] = "=",
	[TT_GE] = ">=",
	[TT_LE] = "<=",
	[TT_E] = "==",
	[TT_NE] = "!=",
	[TT_IN] = ">>",
	[TT_OUT] = "<<"
};

#define next() do { \
	if (get_token() == ERR_SCANNER) \
		return ERR_SCANNER; \
	PRINTTOKEN(); \
} while (0)

#define check(T) do { \
	if (token.type != T) { \
		fprintf(stderr, "Syntax error: '%s' expected, but '%s' given.\n", token_[T], token_[token.type]); \
		return ERR_PARSER; \
	} \
} while (0)

#define term(T) do { \
	check(T); \
	next(); \
} while (0)

#define call(R) do { \
	PRINTNTERM(R); \
	int err = R(); \
	if (err != OK) \
		return err; \
} while (0)

#define try(C) do { \
	int err = C; \
	if (err != OK) \
		return err; \
} while (0)

#define return(R) do { \
	PRINTNTERM(R); \
	return R(); \
} while (0)

string tmp_buffer;
Tdata_type tmp_type;
Tps_item tmp_psitem;
Tdata_type tmp_ftype;

static inline int tmpstr(char *str) {
	string tmp;
	tmp.str = str;
	tmp.length = strlen(str);

	if (!strCopyString(&tmp_buffer, &tmp))
		return ERR_INTERPRET;
	return OK;
}

static Tdata_type get_type() {
	switch (token.type) {
		case TT_AUTO:
			return TYPE_AUTO;
		case TT_INT:
			return TYPE_INT;
		case TT_DOUBLE:
			return TYPE_DOUBLE;
		case TT_STRING:
			return TYPE_STRING;
		default:
			return TYPE_NON;
	}
}

static int P(); // program
static int F(); // function declaration
static int AL(); // first function argument
static int AL2(); // remaining function arguments
static int AD(); // variable declaration
static int FD(); // function definition
static int STL(); // stat list
static int S(); // stat
static int D(); // variable declaration
static int I(); // variable definition
static int ELSE(); // else
static int IN(); // input variables
static int OUT(); // output expressions
static int E(); // expression



static int P() { // program
	switch (token.type) {
		case TT_INT:
		case TT_DOUBLE:
		case TT_STRING: // rule: P -> F P
			call(F);
			return(P);

		case TT_EOF: // rule: P -> EOF
			return OK;

		default:
			return ERR_PARSER;
	}
}

static int F() { // function declaration
	Tdata_type ftype;

	switch (token.type) {
		case TT_INT:
		case TT_DOUBLE:
		case TT_STRING: // rule: type idf ( AL ) FD
			ftype = get_type();
			tmp_ftype = ftype;
			next();

			check(TT_ID);
			try(add_func(ftype, token.p_string));
			next();

			term(TT_L_BRACKET);
			call(AL);
			term(TT_R_BRACKET);
			return(FD);

		default:
			return ERR_PARSER;
	}
}

static int AL() { // first function argument
	switch (token.type) {
		case TT_INT:
		case TT_DOUBLE:
		case TT_STRING: // rule: AL -> AD AL2
			call(AD);
			return(AL2);

		default: // rule: AL -> eps
			return OK;
	}
}

static int AL2() { // remaining function arguments
	switch (token.type) {
		case TT_COMMA: // rule: AL2 -> , AD AL2
			next();
			call(AD);
			return(AL2);

		default: // rule: AL2 -> eps
			return OK;
	}
}

static int AD() { // argument
	Tdata_type atype;

	switch (token.type) {
		case TT_INT:
		case TT_DOUBLE:
		case TT_STRING: // rule: AD -> type id
			atype = get_type();
			next();

			check(TT_ID);
			try(add_param(atype, token.p_string));
			next();

			return OK;

		default:
			return ERR_PARSER;
	}
}

static int FD() { // function definition
	unsigned i;

	switch (token.type) {
		case TT_SEMICOLON: // rule: FD -> ;
			try(func_declaration());

			next();
			return OK;

		case TT_OPEN_BRACKET: // rule: FD -> { STL }
			i = get_line();
			try(func_definition(i)); // instruction pointer

			next();

			call(STL);

			term(TT_CLOSE_BRACKET);

			remove_block();
			generate(I_ERR, NULL, NULL, NULL); // no return => runtime error

			return OK;

		default:
			return ERR_PARSER;
	}
}

static int STL() { // stat list
	switch (token.type) {
		case TT_CLOSE_BRACKET: // rule: STL -> eps
			return OK;

		default: // rule: STL -> S STL
			call(S);
			return(STL);
	}
}

static int S() { // stat
	unsigned i;
	Tlocal_sym_tab_node *conv;

	switch (token.type) {
		case TT_AUTO:
		case TT_INT:
		case TT_DOUBLE:
		case TT_STRING: // rule: S -> D ;
			call(D);
			term(TT_SEMICOLON);
			return OK;

		case TT_OPEN_BRACKET: // rule: S -> { STL }
			next();

			try(add_block());

			call(STL);

			term(TT_CLOSE_BRACKET);

			remove_block();

			return OK;

		case TT_IF: // rule: S -> if ( E ) S else S
			next();
			term(TT_L_BRACKET);

			call(E);
			try(type_compare(tmp_psitem.type, TYPE_INT));

			if (tmp_psitem.type != TYPE_INT) {
				try(tmp_var(TYPE_INT, &conv));

				generate(I_CONVERT, tmp_psitem.value.ptr, NULL, conv);
			} else {
				conv = tmp_psitem.value.ptr;
			}

			Tlocal_sym_tab_node *label;
			try(tmp_var(TYPE_INT, &label));
			generate(I_JUMPIFN, conv, label, NULL);

			term(TT_R_BRACKET);
			call(S);

			Tlocal_sym_tab_node *endif;
			try(tmp_var(TYPE_INT, &endif));
			generate(I_JUMP, NULL, endif, NULL);

			i = get_line();
			set_var_value_int(label->name, i);

			call(ELSE);

			i = get_line();
			set_var_value_int(endif->name, i);

			return OK;

		case TT_FOR: // rule: S -> for ( D ; E ; E ) S
			try(add_block());

			next();
			term(TT_L_BRACKET);
			call(D);
			term(TT_SEMICOLON);

			Tlocal_sym_tab_node *cond;
			try(tmp_var(TYPE_INT, &cond));
			i = get_line();
			set_var_value_int(cond->name, i);

			call(E);
			try(type_compare(tmp_psitem.type, TYPE_INT));

			if (tmp_psitem.type != TYPE_INT) {
				try(tmp_var(TYPE_INT, &conv));

				generate(I_CONVERT, tmp_psitem.value.ptr, NULL, conv);
			} else {
				conv = tmp_psitem.value.ptr;
			}

			Tlocal_sym_tab_node *endfor;
			try(tmp_var(TYPE_INT, &endfor));
			generate(I_JUMPIFN, conv, endfor, NULL);

			Tlocal_sym_tab_node *statlist;
			try(tmp_var(TYPE_INT, &statlist));
			generate(I_JUMP, NULL, statlist, NULL);

			Tlocal_sym_tab_node *assign;
			try(tmp_var(TYPE_INT, &assign));
			i = get_line();
			set_var_value_int(assign->name, i);

			term(TT_SEMICOLON);
			call(E);

			generate(I_JUMP, NULL, cond, NULL);

			i = get_line();
			set_var_value_int(statlist->name, i);

			term(TT_R_BRACKET);
			call(S);

			generate(I_JUMP, NULL, assign, NULL);

			i = get_line();
			set_var_value_int(endfor->name, i);

			remove_block();

			return OK;

		case TT_WHILE: // rule: S -> while ( E ) S
			next();

			term(TT_L_BRACKET);

			Tlocal_sym_tab_node *wcond;
			try(tmp_var(TYPE_INT, &wcond));
			i = get_line();
			set_var_value_int(wcond->name, i);

			call(E);
			try(type_compare(tmp_psitem.type, TYPE_INT));

			if (tmp_psitem.type != TYPE_INT) {
				try(tmp_var(TYPE_INT, &conv));

				generate(I_CONVERT, tmp_psitem.value.ptr, NULL, conv);
			} else {
				conv = tmp_psitem.value.ptr;
			}

			Tlocal_sym_tab_node *endwhile;
			try(tmp_var(TYPE_INT, &endwhile));
			generate(I_JUMPIFN, conv, endwhile, NULL);

			term (TT_R_BRACKET);

			call(S);

			generate(I_JUMP, NULL, wcond, NULL);

			i = get_line();
			set_var_value_int(endwhile->name, i);

			return OK;

		case TT_DO: // rule: S -> do S while ( E ) ;
			next();

			Tlocal_sym_tab_node *dwbody;
			try(tmp_var(TYPE_INT, &dwbody));
			i = get_line();
			set_var_value_int(dwbody->name, i);

			call(S);

			term(TT_WHILE);

			term(TT_L_BRACKET);

			call(E);
			try(type_compare(tmp_psitem.type, TYPE_INT));

			if (tmp_psitem.type != TYPE_INT) {
				try(tmp_var(TYPE_INT, &conv));

				generate(I_CONVERT, tmp_psitem.value.ptr, NULL, conv);
			} else {
				conv = tmp_psitem.value.ptr;
			}

			generate(I_JUMPIF, conv, dwbody, NULL);

			term(TT_R_BRACKET);

			term(TT_SEMICOLON);

			return OK;

		case TT_RETURN: // rule: S -> return E ;
			next();

			call(E);
			try(type_compare(tmp_ftype, tmp_psitem.type));

			if (tmp_psitem.type != tmp_ftype) {
				try(tmp_var(tmp_ftype, &conv));

				generate(I_CONVERT, tmp_psitem.value.ptr, NULL, conv);
			} else {
				conv = tmp_psitem.value.ptr;
			}

			generate(I_RETURN, conv, NULL, NULL);

			term(TT_SEMICOLON);
			return OK;

		case TT_CIN: // rule: S -> cin >> id IN ;
			next();
			term(TT_IN);

			check(TT_ID);
			if (!var_exists(token.p_string))
				return ERR_UNDEF;
			Tlocal_sym_tab_node *var = get_var(token.p_string);

			generate(I_CIN, NULL, NULL, var);

			next();

			call(IN);
			term(TT_SEMICOLON);
			return OK;

		case TT_COUT: // rule: S -> cout << E OUT ;
			next();
			term(TT_OUT);

			call(E);

			generate(I_COUT, tmp_psitem.value.ptr, NULL, NULL);

			call(OUT);
			term(TT_SEMICOLON);
			return OK;

		default: // rule: S -> E ;
			call(E);
			term(TT_SEMICOLON);
			return OK;
	}
}

static int D() { // variable declaration
	Tdata_type vtype;

	switch (token.type) {
		case TT_AUTO:
		case TT_INT:
		case TT_DOUBLE:
		case TT_STRING: // rule: D -> type id I
			vtype = get_type();
			tmp_type = vtype;
			next();

			check(TT_ID);
			try(add_var(vtype, token.p_string));
			try(tmpstr(token.p_string));
			next();

			return(I);

		default:
			return ERR_PARSER;
	}
}

static int I() { // variable definition
	switch (token.type) {
		case TT_ASSIGN: // rule: I -> = E
			next();
			call(E);

			try(assign_type_compare(tmp_buffer.str, tmp_psitem.type));

			Tlocal_sym_tab_node *var = get_var(tmp_buffer.str);
			Tlocal_sym_tab_node *conv;

			if (tmp_psitem.type != var->data_type) {
				try(tmp_var(var->data_type, &conv));

				generate(I_CONVERT, tmp_psitem.value.ptr, NULL, conv);
			} else {
				conv = tmp_psitem.value.ptr;
			}

			generate(I_ASSIGN, conv, NULL, var);

			return OK;

		default: // rule: I -> eps
			if (tmp_type == TYPE_AUTO)
				return ERR_AUTO;
			return OK;
	}
}

static int ELSE() { // else
	switch (token.type) {
		case TT_ELSE: // rule: ELSE -> else S
			next();

			return(S);

		default: // rule: ELSE -> eps
			return OK;
	}
}

static int IN() { // input variables
	switch (token.type) {
		case TT_IN: // rule: IN -> >> id IN
			next();

			check(TT_ID);
			if (!var_exists(token.p_string))
				return ERR_UNDEF;
			Tlocal_sym_tab_node *var = get_var(token.p_string);

			generate(I_CIN, NULL, NULL, var);

			next();

			return(IN);

		default: // rule: IN -> eps
			return OK;
	}
}

static int OUT() { // output expressions
	switch (token.type) {
		case TT_OUT: // rule: OUT -> << E OUT
			next();
			call(E);

			generate(I_COUT, tmp_psitem.value.ptr, NULL, NULL);

			return(OUT);

		default: // rule: OUT -> eps
			return OK;
	}
}

static int E() { // expression
	Tpsa_stack *psa_stack = ps_init();

	ps_push(psa_stack, OP_END);

	if (get_prec(OP_END) == ' ') {
		fprintf(stderr, "Syntax error: 'expression' expected, but '%s' given.\n", token_[token.type]);

		return ERR_PARSER;
	}

	int err;

	do {
		Tps_item *ps_item = ps_first_term(psa_stack);
		switch (get_prec(ps_item->op)) {
			case '=':
				err = ps_push_token(psa_stack);
				if (err != OK) {
					ps_destroy(psa_stack);
					return err;
				}
				next();
			break;

			case '<':
				err = ps_ins_after(psa_stack, ps_item, OP_EXPR);
				if (err != OK) {
					ps_destroy(psa_stack);
					return err;
				}

				err = ps_push_token(psa_stack);
				if (err != OK) {
					ps_destroy(psa_stack);
					return err;
				}

				next();
			break;

			case '>':
				err = psa(psa_stack);
				if (err != OK) {
					ps_destroy(psa_stack);
					return err;
				}
			break;
		}
	} while (get_prec(ps_first_term(psa_stack)->op) != ' ');

	Tps_item *top = ps_top(psa_stack);

	if (top->op != OP_NTERM || ps_prev(psa_stack)->op != OP_END) {
		fprintf(stderr, "Syntax error: unexpected '%s'\n", token_[token.type]);
		return ERR_PARSER;
	}

	tmp_psitem.type = top->type;
	tmp_psitem.value = top->value;

	ps_destroy(psa_stack);

	return OK;
}

int parse() {
	next();

	strInit(&tmp_buffer);

	int err = P();

	strFree(&tmp_buffer);

	return err;
}
