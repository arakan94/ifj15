/******** debug.h *********
 *
 * FIT VUT, IFJ 089
 * Author: Martin Trbola, xtrbol00
 * Summary: Debug headers
 *
 */

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

//#define DEBUG 1

#ifdef DEBUG
	#include "scanner.h"
	#include "functions.h"

	static const char *op_name[] = {
		[I_CONVERT] = "I_CONVERT",
		[I_COUT] = "I_COUT",
		[I_CIN] = "I_CIN",
		[I_ASSIGN] = "I_ASSIGN",

		[I_ADD] = "I_ADD",
		[I_SUB] = "I_SUB",
		[I_MUL] = "I_MUL",
		[I_DIV] = "I_DIV",

		[I_G] = "I_G",
		[I_GE] = "I_GE",
		[I_LE] = "I_LE",
		[I_L] = "I_L",
		[I_E] = "I_E",
		[I_NE] = "I_NE",

		[I_JUMP] = "I_JUMP",
		[I_JUMPIF] = "I_JUMPIF",
		[I_JUMPIFN] = "I_JUMPIFN",

		[I_PREPARE] = "I_PREPARE",
		[I_SET] = "I_SET",
		[I_CALL] = "I_CALL",
		[I_RETURN] = "I_RETURN",

		[I_ADDBLOCK] = "I_ADDBLOCK",
		[I_RMBLOCK] = "I_RMBLOCK",

		[I_ERR] = "I_ERR",
	};
#endif

#if defined SCANNER_DEBUG || defined DEBUG
	static const char *token_name[] = {
		[TT_ERR] = "TT_ERR",
		[TT_EOF] = "TT_EOF",
		[TT_ID] = "TT_ID",
		[TT_VALUE_INT] = "TT_VALUE_INT",
		[TT_VALUE_DOUBLE] = "TT_VALUE_DOUBLE",
		[TT_VALUE_STRING] = "TT_VALUE_STRING",
	//KEYWORDS
		[TT_AUTO] = "TT_AUTO",
		[TT_INT] = "TT_INT",
		[TT_DOUBLE] = "TT_DOUBLE",
		[TT_STRING] = "TT_STRING",
		[TT_CIN] = "TT_CIN",
		[TT_COUT] = "TT_COUT",
		[TT_FOR] = "TT_FOR",
		[TT_IF] = "TT_IF",
		[TT_ELSE] = "TT_ELSE",
		[TT_RETURN] = "TT_RETURN",
		[TT_WHILE] = "TT_WHILE",
		[TT_DO] = "TT_DO",
	//DELIMITERS
		[TT_SEMICOLON] = "TT_SEMICOLON",
		[TT_L_BRACKET] = "TT_L_BRACKET",
		[TT_R_BRACKET] = "TT_R_BRACKET",
		[TT_OPEN_BRACKET] = "TT_OPEN_BRACKET",
		[TT_CLOSE_BRACKET] = "TT_CLOSE_BRACKET",
		[TT_COMMA] = "TT_COMMA",
		//APOSTROPHE - not supported
	//OPERATORS
		[TT_MULTIPLY] = "TT_MULTIPLY",
		[TT_DIVIDE] = "TT_DIVIDE",
		[TT_ADD] = "TT_ADD",
		[TT_SUB] = "TT_SUB",
		[TT_L] = "TT_L",
		[TT_G] = "TT_G",
		[TT_ASSIGN] = "TT_ASSIGN",
		[TT_GE] = "TT_GE",
		[TT_LE] = "TT_LE",
		[TT_E] = "TT_E",
		[TT_NE] = "TT_NE",
		[TT_IN] = "TT_IN",
		[TT_OUT] = "TT_OUT"
	};
#endif

#ifdef SCANNER_DEBUG
	static char* fsm_states[] = {
		"INIT",
		"OPERATOR",
		"STRING",
		"BACKSLASH",
		"HEXA",
		"BINARY",
		"OCTA",
		"NUMBER",
		"FLOAT",
		"FLOAT_EXP",
		"ID",
		"ID_KEYWORD",
		"LINE",
		"BLOCK",
		"BLOCK_END",
		"BASE",
		"BINARY",
		"OCTA",
		"HEXA"
	};
#endif

//#define DEBUG_OUT 1

#ifdef DEBUG_OUT
	#include <stdio.h>
	#define PRINTTOKEN() printf("%s\n", token_name[token.type])
	#define PRINTNTERM(N) printf("%s\n", #N)
#else
	#define PRINTTOKEN() (void)0
	#define PRINTNTERM(N) (void)0
#endif

#endif // DEBUG_H_INCLUDED
