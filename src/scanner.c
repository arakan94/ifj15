/******** scanner.c *********
 *
 * FIT VUT, IFJ 089
 * Author: David Novák, xnovak1m
 * Summary: Reads input file and sends tokens to parser
 * Notes: Working nicely, support for BASE. ID length is unlimited.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "error_codes.h"
#include "str.h"
#include "scanner.h"

// macros for shorter code
#define strAddChar(s, c) if(!strAddChar((s), (c))) return ERR_INTERPRET
#define strCopyString(s1, s2) if(!strCopyString((s1), (s2))) return ERR_INTERPRET

#define lex_error(...) do {                             \
	fprintf(stderr, "Line %d, column %d: ", row+1, col);  \
	fprintf(stderr, ##__VA_ARGS__);                       \
	token.type = TT_ERR;                                  \
	return ERR_SCANNER;                                   \
	} while (0)

#define lex_warning(...) do {                           \
	fprintf(stderr, "Line %d, column %d - Warning: ", row+1, col);  \
	fprintf(stderr, ##__VA_ARGS__);                       \
	} while (0)

static char *keywords[KEYWORDS_COUNT] = {
	"auto",
	"int",
	"double",
	"string",
	"cin",
	"cout",
	"for",
	"if",
	"else",
	"return",
	"while",
	"do"
};

static char *operators[OPERATORS_COUNT] = {
	"*",
	"/",
	"+",
	"-",
	"<",
	">",
	"=",
	">=",
	"<=",
	"==",
	"!=",
	">>",
	"<<"
};

#ifdef SCANNER_DEBUG
	#include "debug.h"
#endif

static char operator_chars[OPERATOR_CHARS_COUNT] = {'*', '/', '+', '-', '<', '>', '=', '!'};

static char delimiters[DELIMITERS_COUNT] = {';', '(', ')', '{', '}', ',', '"'}; // '[', ']',

static FILE *in;

static int is_keyword(const char *text)
{
	for (int i = 0; i < KEYWORDS_COUNT; i++)
	{
		if (strcmp(text, keywords[i]) == 0)
			return i+1;
	}
	return 0;
}

static int determine_operator(const char *text)
{
	for (int i = 0; i < OPERATORS_COUNT; i++)
	{
		if (strcmp(text, operators[i]) == 0)
			return i+1;
	}
	return 0;
}

static int is_operator(const char c)
{
	for (int i = 0; i < OPERATOR_CHARS_COUNT; i++)
	{
		if (operator_chars[i] == c)
			return i+1;
	}
	return 0;
}

// basically anything, that can follow id or operator
static int is_delimiter(const char c)
{
	for (int i = 0; i < DELIMITERS_COUNT; i++)
	{
		if (delimiters[i] == c)
			return i+1;
	}
	return 0;
}

int get_token()
{
	typedef enum {
		INIT,
		OPERATOR,
		SLASH,
		STRING,
		STRING_BACKSLASH,
		STRING_HEXA,
		STRING_BINARY,
		STRING_OCTA,
		NUMBER,
		FLOAT,
		FLOAT_EXP,
		ID,
		ID_KEYWORD,
		LINE_COMMENT,
		BLOCK_COMMENT,
		BLOCK_COMMENT_END,
		BASE_EXT,
		BINARY,
		OCTA,
		HEXA
	} Tstate;

	Tstate state = INIT;

	int c;
	int j = 0;

	int ret_val = 0;
	int escape_seq = 0;

	//char *check;

	strClear(buffer);

	token.type = TT_ERR;

	while ((c = fgetc(in)))
	{
		if (c == '\n')
		{
			row++;
			col = 0;
		}
		else
			col++;

		if (c == EOF)
		{
			token.type = TT_EOF;
			return EOF;
		}

	#ifdef SCANNER_DEBUG
		fprintf(stderr, "%s (%s)", fsm_states[state], strGetStr(buffer));
		if (strFirst(buffer) == '\0')
			fprintf(stderr, "\n");
		else
			fprintf(stderr, " -> ");
	#endif // DEBUG

		switch(state)
		{
			case INIT:
				if (c == '/')  // comment or operator
				{
					state = SLASH;
					strAddChar(buffer, c);
				}
				else if (is_operator(c))
				{
					state = OPERATOR;
					strAddChar(buffer, c);
				}
				else if (c == '"') // string literal
				{
					state = STRING;
				}
				else if (c == '\\') // x, b, 0 literals supported - BASE
				{
					state = BASE_EXT;
				}
				else if (isdigit(c))  // number -> integer or double literal
				{
					state = NUMBER;
					strAddChar(buffer, c);
				}
				else if (c == '_')   // id
				{
					state = ID;
					strAddChar(buffer, c);
				}
				else if (isalpha(c)) // alphabetic char -> id or keyword
				{
					state = ID_KEYWORD;
					strAddChar(buffer, c);
				}
				else if ((ret_val = is_delimiter(c)))
				{
					token.type = TYPE_DELIMITER + ret_val - 1;
					#ifdef SCANNER_DEBUG
						fprintf(stderr, "%s\n", token_name[token.type]);
					#endif
					return OK;
				}
				else if (!isspace(c))    // non valid character
				{
					lex_error("Unknown character: '%c'.\n", c);
				}
			break;

			case BASE_EXT:
				if (c == 'b')
				{
					state = BINARY;
				}
				else if (c == '0')
				{
					state = OCTA;
				}
				else if (c == 'x')
				{
					state = HEXA;
				}
				else
					lex_error("Unknown character in literal '\\%c'.\n", c);
			break;

			case HEXA:
				if (isxdigit(c))
				{
					if (j < 8)  // 8 hexadecimal digits are max int value
					{
						literal[j] = c;
						j++;
					}
					else
						lex_error("Hexadecimal literal too long -> int overflow!\n");
				}
				else
				{
					ungetc(c, in);
					token.type = TT_VALUE_INT;
					literal[j] = '\0';
					token.value_int = (int) strtol(literal, NULL, 16);  // cannot fail
					if (token.value_int < 0)
						lex_warning("Hexadecimal literal '\\x%s' overflow to negative number %d\n", literal, token.value_int);
					return OK;
				}

			break;

			case OCTA:
				if (c >= '0' && c <= '7')
				{
					if (j < 12)  // max int = \0 7777 7777 7777
					{
						literal[j] = c;
						j++;
					}
					else
						lex_error("Octal literal too long -> int overflow!\n");
				}
				else
				{
					ungetc(c, in);
					token.type = TT_VALUE_INT;
					literal[j] = '\0';
					token.value_int = (int) strtol(literal, NULL, 8);
					if (token.value_int < 0)
						lex_warning("Octal literal '\\0%s' overflow to negative number %d\n", literal, token.value_int);
					return OK;
				}

			break;

			case BINARY:
				if ((c == '0' || c == '1'))
				{
					if (j < 32)
					{
						literal[j] = c;
						j++;
					}
					else
						lex_error("Binary literal too long -> int overflow!\n");
				}
				else
				{
					ungetc(c, in);
					token.type = TT_VALUE_INT;
					literal[j] = '\0';
					token.value_int = (int) strtol(literal, NULL, 2);
					if (token.value_int < 0)
						lex_warning("Binary literal '\\b%s' overflow to negative number %d\n", literal, token.value_int);
					return OK;
				}

			break;

			case ID_KEYWORD:
				if (isalpha(c))  // add another char into buffer
				{
					strAddChar(buffer, c);
				}
				else if (c == '_' || isdigit(c))  // id - these chars are not in any keyword
				{
					state = ID;
					strAddChar(buffer, c);
				}
				else // end of id or keyword
				{

					ungetc(c, in);			 // return last read char to buffer

					ret_val = is_keyword(strGetStr(buffer));
					if (ret_val)
					{
						token.type = TYPE_KEYWORD + ret_val - 1;  // magic
						#ifdef SCANNER_DEBUG
							fprintf(stderr, "%s\n", token_name[token.type]);
						#endif
						return OK;
					}
					else
					{
						token.type = TT_ID;
						token.p_string = strGetStr(buffer);
						#ifdef SCANNER_DEBUG
							fprintf(stderr, "%s\n", token_name[token.type]);
						#endif
						return OK;
					}
				}
			break;

			case ID:
				if (isalnum(c) || c == '_')
				{
					strAddChar(buffer, c);
				}
				else
				{
					ungetc(c, in);
					token.type = TT_ID;
					token.p_string = strGetStr(buffer);
					#ifdef SCANNER_DEBUG
						fprintf(stderr, "%s\n", token_name[token.type]);
					#endif
					return OK;
				}
			break;

			case SLASH:
				if (c == '/')
				{
					state = LINE_COMMENT;
				}
				else if (c == '*')
				{
					state = BLOCK_COMMENT;
				}
				else  // it was division
				{
					ungetc(c, in);
					token.type = TT_DIVIDE;
					#ifdef SCANNER_DEBUG
						fprintf(stderr, "%s\n", token_name[token.type]);
					#endif
					return OK;
				}
			break;

			case OPERATOR:  // not precisely "normal" fsm, but easily extensible (just add operator to operators[] and Ttoken_type)
        if (is_operator(c))  // c is one of valid chars, that can be in operator
        {
					strAddChar(buffer, c);
					ret_val = determine_operator(strGetStr(buffer));   // check if we still have valid operator in buffer

					if (!ret_val)         // if it's not valid operator
					{
						ungetc(c, in);       // return last char, it was not part of operator
						strDelChar(buffer);  // delete wrong char from buffer

						ret_val = determine_operator(strGetStr(buffer));  // determine which operator we have
						token.type = TYPE_OPERATOR + ret_val - 1;         // return token
						#ifdef SCANNER_DEBUG
							fprintf(stderr, "%s\n", token_name[token.type]);
						#endif
						return OK;
					}
					// continue with loading chars if it's valid
				}
				else  // another char is not operator -> end
				{
					ungetc(c, in);
					ret_val = determine_operator(strGetStr(buffer));
					if (ret_val)
					{
						token.type = TYPE_OPERATOR + ret_val - 1;
						#ifdef SCANNER_DEBUG
							fprintf(stderr, "%s\n", token_name[token.type]);
						#endif
						return OK;
					}
					else // shouldn't occur, just to be sure..
					{
						lex_error("Unknown operator: '%s'.\n", strGetStr(buffer));
					}
				}
			break;

			case LINE_COMMENT:
				if (c == '\n')   // end of line comment
				{
					state = INIT;
					strClear(buffer);
				}
			break;

			case BLOCK_COMMENT:
				if (c == '*')    // possible end of comment
					state = BLOCK_COMMENT_END;
			break;

			case BLOCK_COMMENT_END:
				if (c == '/')    // comment ended
				{
					state = INIT;
					strClear(buffer);
				}
				else            // false alarm - comment continues
					state = BLOCK_COMMENT;
			break;

			case NUMBER:
				if (isdigit(c))
				{
					strAddChar(buffer, c);
				}
				else if (c == '.')
				{
					strAddChar(buffer, c);
					state = FLOAT;
				}
				else if (tolower(c) == 'e')
				{
					strAddChar(buffer, c);
					state = FLOAT_EXP;
				}
				else
				{
					ungetc(c, in);
					token.type = TT_VALUE_INT;
					token.value_int = (int) strtol(strGetStr(buffer), NULL, 10);

					#ifdef SCANNER_DEBUG
						fprintf(stderr, "%s\n", token_name[token.type]);
					#endif
					return OK;
				}

			break;

			case FLOAT:  // aspoň jedna číslice!
        if (isdigit(c))
				{
					strAddChar(buffer, c);
				}
				else if (tolower(c) == 'e')
				{
					strAddChar(buffer, c);
					state = FLOAT_EXP;
				}
				else
				{
					ungetc(c, in);

					token.type = TT_VALUE_DOUBLE;
					token.value_double = strtod(strGetStr(buffer), NULL); //&check);

					#ifdef SCANNER_DEBUG
						fprintf(stderr, "%s\n", token_name[token.type]);
					#endif
					return OK;
				}
			break;

			case FLOAT_EXP:
				if (isdigit(c))
				{
					strAddChar(buffer, c);
				}
				else if (tolower(strLast(buffer)) == 'e' && (c == '+' || c == '-')) // optional +/- after e/E
				{
					strAddChar(buffer, c);
				}
				else
				{
					ungetc(c, in);
					token.type = TT_VALUE_DOUBLE;
					token.value_double = strtod(strGetStr(buffer), NULL); //&check);

					#ifdef SCANNER_DEBUG
						fprintf(stderr, "%s\n", token_name[token.type]);
					#endif
					return OK;
				}
			break;

			case STRING:
				if (c == '"')   // end of string literal
				{
					token.type = TT_VALUE_STRING;
					token.p_string = strGetStr(buffer);
					#ifdef SCANNER_DEBUG
						fprintf(stderr, "%s\n", token_name[token.type]);
					#endif
					return OK;
				}
				else if (c == '\\')   // string literal continues on another line or character constant
					state = STRING_BACKSLASH;
				else if (c != '\n')
				{
					strAddChar(buffer, c);
				}
				else
				{
					lex_error("String literal not closed.\n");
				}
			break;

			case STRING_BACKSLASH:
				state = STRING;

				if (c == '\\')
				{
					strAddChar(buffer, '\\');
				}
				else if (c == 'n')
				{
					strAddChar(buffer, '\n');
				}
				else if (c == 't')
				{
					strAddChar(buffer, '\t');
				}
				else if (c == '"')
				{
					strAddChar(buffer, '"');
				}
				else if (c == 'x')
				{
					state = STRING_HEXA;
				}
				else if (c == 'b')
				{
					state = STRING_BINARY;
				}
				else if (c == '0')
				{
					state = STRING_OCTA;
				}
				else if (c == '\n')
				{
					// do nothing, string continues on next line - TODO: zdokumentovat upravu
				}
				else
				{
					lex_error("Escape sequence '\\%c' unknown.\n", c);
				}
			break;

			case STRING_HEXA:
				if (j < 2 && isxdigit(c)) // 2 is max hexadecimal escape length
				{
					literal[j] = c;
					j++;
				}
				else if (j == 0) // no valid hexadecimal digit after \x -> error
				{
					lex_error("'\\x%c' is not valid hexadecimal escape sequence.\n", c);
				}
				else  // end of hexadecimal escape
				{
					literal[j] = '\0';
					escape_seq = strtol(literal, NULL, 16);  // will always be successful

					if (escape_seq == 0)
					{
						lex_error("\\x00 is not allowed hexadecimal escape sequence.\n");
					}

					strAddChar(buffer, escape_seq);

					ungetc(c, in);   // return currently read char
					j = 0;

					state = STRING;
				}
			break;

			case STRING_BINARY:
				if (j < 8 && (c == '0' || c == '1')) // 8 is max binary escape length
				{
					literal[j] = c;
					j++;
				}
				else if (j == 0) // no valid binary digit after \b -> error
				{
					lex_error("'\\b%c' is not valid binary escape sequence.\n", c);
				}
				else  // end of binary escape
				{
					literal[j] = '\0';
					escape_seq = strtol(literal, NULL, 2);  // will always be successful

					if (escape_seq == 0)
					{
						lex_error("\\b00000000 is not allowed binary escape sequence.\n");
					}

					strAddChar(buffer, escape_seq);

					ungetc(c, in);   // return currently read char
					j = 0;

					state = STRING;
				}
			break;

			case STRING_OCTA:
				if (j < 3 && c >= '0' && c <= '7') // 3 is max octal escape length
				{
					literal[j] = c;
					j++;
				}
				else if (j == 0) // no valid octal digit after \0 -> error
				{
					lex_error("'\\0%c' is not valid octal escape sequence.\n", c);
				}
				else  // end of octal escape
				{
					literal[j] = '\0';
					escape_seq = strtol(literal, NULL, 8);  // will always be successful

					if (escape_seq == 0)
					{
						lex_error("\\000 is not allowed octal escape sequence.\n");
					}
					else if (escape_seq > 255)
					{
						lex_error("Octal escape '\\0%s' bigger than 255.\n", literal);
					}

					strAddChar(buffer, escape_seq);

					ungetc(c, in);   // return currently read char
					j = 0;

					state = STRING;
				}
			break;

			default:
				lex_error("Scanner panic!!!\n");
			break;
		} // end_switch
	} // end_while
	return 0;
}

int init_scanner(char *filename)
{
	if (filename != NULL)
	{
		if ((in = fopen(filename, "r")) == NULL)
		{
			fprintf(stderr, "Input file '%s' cannot be open!\n", filename);
			return EOF;
		}
	}
	else
	{
		in = stdin;
	}

	buffer = &buff_struct;

	if (!strInit(buffer))
	{
		fprintf(stderr, "Malloc error.\n");
		return ERR_INTERPRET;
	}
	else
	{
		return OK;
	}
}

int clean_scanner()
{
	strFree(buffer);

	if (fclose(in) != EOF)
	{
		return OK;
	}
	else
	{
		fprintf(stderr, "Input file couldn't be closed!\n");
		return EOF;
	}
}

#ifdef SCANNER_SELFTEST
int main(int argc, char **argv)
{
	if (argc == 2)
	{
		if (init_scanner(argv[1]))
			return EOF;
	}
	else
	{
		if (init_scanner(NULL))
			return EOF;
	}

	int ret;
	do
	{
		ret = get_token();

		#ifdef SCANNER_DEBUG
			if (token.type == TT_VALUE_INT)
				printf("\n%d Typ: %s %d\n", ret, token_name[token.type], token.value_int);
			else if (token.type == TT_VALUE_DOUBLE)
				printf("\n%d Typ: %s %lf\n", ret, token_name[token.type], token.value_double);
			else
				printf("\n%d Typ: %s %s\n", ret, token_name[token.type], strGetStr(buffer));
		#endif // DEBUG

	} while (ret != ERR_SCANNER && ret != EOF);

	if(clean_scanner())
		return EOF;

	return 0;
}
#endif // SCANNER_SELFTEST

