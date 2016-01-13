/******** error_codes.h *********
 *
 * FIT VUT, IFJ 089
 * Author: David Novák, xnovak1m
 * Summary: Defines symbolic constants for error codes.
 *
 */

#ifndef ERR_INCLUDED
#define ERR_INCLUDED

	#define OK           0

	#define ERR_SCANNER  1
	#define ERR_PARSER   2

	// semantic analysis errors
	#define ERR_UNDEF     3    // undefined function/variable, multiple definitions
	#define ERR_TYPE      4    // type incompability (int + string, ...), wrong count or type of parameters in function call
	#define ERR_AUTO      5    // error determining type of auto variable
	#define ERR_SEMANTIC  6    // other semantic errors

	// runtime errors
	#define ERR_NUMBER    7    // error parsing number from stdin
	#define ERR_VARINIT   8    // working with uninicialized variable
	#define ERR_ZERODIV   9    // division by zero
	#define ERR_RUN      10    // other runtime errors

	#define ERR_INTERPRET  99  // internal error (memory allocation, opening file, wrong args)

#endif // ERR_INCLUDED
