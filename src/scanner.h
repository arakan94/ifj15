/******** scanner.h *********
 *
 * FIT VUT, IFJ 089
 * Author: David Novák, xnovak1m
 * Summary: All defines, global variables and types for scanner.
 * Notes: For selftest (enables main) define SCANNER_SELFTEST.
 *        For debug information define SCANNER_DEBUG.
 */

#ifndef SCANNER_INCLUDED
#define SCANNER_INCLUDED

	#include "str.h"

	//#define SCANNER_DEBUG
	//#define SCANNER_SELFTEST

	#define KEYWORDS_COUNT 12
	#define OPERATOR_CHARS_COUNT 8
	#define OPERATORS_COUNT 13
	#define DELIMITERS_COUNT 7

	#define TYPE_KEYWORD     10
	#define TYPE_DELIMITER   30
	#define TYPE_OPERATOR    50

	// Public functions
	int init_scanner(char *filename);
	int get_token();
	int clean_scanner();

	typedef enum {
		TT_ERR,
		TT_EOF,
		TT_ID,
		TT_VALUE_INT,
		TT_VALUE_DOUBLE,
		TT_VALUE_STRING,
	//KEYWORDS
		TT_AUTO = TYPE_KEYWORD,
		TT_INT,
		TT_DOUBLE,
		TT_STRING,
		TT_CIN,
		TT_COUT,
		TT_FOR,
		TT_IF,
		TT_ELSE,
		TT_RETURN,
		TT_WHILE,
		TT_DO,
	//DELIMITERS
		TT_SEMICOLON = TYPE_DELIMITER,
		TT_L_BRACKET,
		TT_R_BRACKET,
		TT_OPEN_BRACKET,
		TT_CLOSE_BRACKET,
		TT_COMMA,
		//APOSTROPHE - not supported
	//OPERATORS
		TT_MULTIPLY = TYPE_OPERATOR,
		TT_DIVIDE,
		TT_ADD,
		TT_SUB,
		TT_L,
		TT_G,
		TT_ASSIGN,
		TT_GE,
		TT_LE,
		TT_E,
		TT_NE,
		TT_IN,
		TT_OUT
	} Ttoken_type;

	struct {
		Ttoken_type type;
		union {
			int value_int;
			double value_double;
			char* p_string;
		};
	} token;

	string buff_struct;
	string *buffer;

	char literal[32];  // buffer for /x, /b, /0 literals

	// current position in source code
	int row;
	int col;

#endif // SCANNER_INCLUDED
