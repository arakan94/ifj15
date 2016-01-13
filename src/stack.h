/******** stack.h *********
*
* FIT VUT, IFJ 089
* Author: Šimon Urban, xurban63
* Summary: Stack implementation
*
*/

#ifndef STACK_INCLUDED
#define STACK_INCLUDED

	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>

	#define STACK_MINIMUM_SIZE 64

	typedef enum stack_errors{
		OK_STACK,
		ERR_STACK_ALLOC,
		ERR_STACK_NULL,
	} Tstack_error;

	typedef struct
	{
		void **top;
		void **base;
		void **max_size;
	} Tstack;

	Tstack *stack_init();
	Tstack_error stack_push(Tstack *s, void *address);
	void *stack_pop(Tstack *s);
	void *stack_top(Tstack *s);
	void stack_dispose(Tstack *s);
	bool stack_empty(Tstack *s);

#endif	//STACK_INCLUDED
