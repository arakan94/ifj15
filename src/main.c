/******** main.c *********
 *
 * FIT VUT, IFJ 089
 * Authors:
 *    Martin Trbola, xtrbol00
 *    David Novák, xnovak1m
 * Summary: Main
 *
 */

#include "error_codes.h"
#include "scanner.h"
#include "parser.h"
#include "semantic.h"
#include "tac.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		// argv[0] == interpret
		// argv[1] == filename
		// and nothing else!

		fprintf(stderr, "Wrong number of arguments\n");
		return ERR_INTERPRET;
	}

	// try to open file
	if (init_scanner(argv[1]) != OK) {
		fprintf(stderr, "Cannot open file\n");
		return ERR_INTERPRET;
	}

	int err;

	err = initialize_global_symbol_table();
	if (err != OK)
		return err;

	err = init_tac();
	if (err != OK)
		return err;

	err = parse();

	if (err == OK) {
		err = every_func_defined();
		if (err == OK) {
			Tglobal_sym_tab_node *m = get_func("main");
			err = run_tac(m->func_start, m->var_count);
		} else {
			fprintf(stderr, "Undefined function\n");
		}
	} else if (err == ERR_PARSER) {
		fprintf(stderr, "Syntax error: line %d, column before %d\n", row+1, col);
	} else if (err == ERR_SCANNER) { // Lexikalni vypisuje scanner sam
	//	fprintf(stderr, "Lexical error: line %d, column before %d\n", row+1, col);
	} else if (err == ERR_SEMANTIC || err == ERR_UNDEF || err == ERR_TYPE || err == ERR_AUTO) {
		fprintf(stderr, "Semantic error: line %d, column before %d\n", row+1, col);
	} else if (err == ERR_NUMBER || err == ERR_VARINIT || err == ERR_ZERODIV || err == ERR_RUN) {
		fprintf(stderr, "Runtime error: line %d, column before %d\n", row+1, col);
	} else if (err == ERR_INTERPRET) {
		fprintf(stderr, "Internal error: line %d, column before %d\n", row+1, col);
	} else {
		fprintf(stderr, "Unknown error: line %d, column before %d\n", row+1, col);
	}

	free_tac();

	free_global_symbol_table();

	clean_scanner();

	//fprintf(stderr, "%d\n", err);

	return err;
}
