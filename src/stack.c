/******** stack.c *********
*
* FIT VUT, IFJ 089
* Author: Šimon Urban, xurban63
* Summary: Stack implementation
*
*/

#include "stack.h"

/*
	Check if stack s is full
	Used in stack_push
*/
static bool stack_full(Tstack *s)
{
	if (s != NULL)
	{
		//stack is full when s->top == s->max_size
		return (s->top == s->max_size);
	}
	else
	{
		//stack given by parameter is NULL
        return false;
	}
}

/*
	Initialize stack s
*/
Tstack *stack_init()
{
	Tstack *s = malloc(sizeof(Tstack));	//allocate memory for stack
	if (s == NULL)
	{
		//unable to allocate memory for stack
		return NULL;
	}
	s->base = malloc(sizeof(void *) * STACK_MINIMUM_SIZE);	//allocate memory for elements
	if (s->base == NULL)
	{
		//unable to allocate memory for elements
		free(s);	//free memory allocated for stack s
		return NULL;
	}
	s->top = s->base;	//stack is empty
	s->max_size = s->base + STACK_MINIMUM_SIZE; //get pointer to end of memory allocated for stack
	return s;   //return pointer to allocated stack s
}

/*
	Add new element on top of stack s
*/
Tstack_error stack_push(Tstack *s, void *address)
{
	if (s != NULL)
	{
		if (stack_full(s))
		{
			//stack is full - double stack size
			int elem_count = s->top - s->base;  //get element count (stack size)
			s->base = realloc(s->base, sizeof(void *) * elem_count * 2);	//allocate more memory for stack elements
			if (s->base == NULL)
			{
				//unable to reallocate memory for elements
				return ERR_STACK_ALLOC;
			}
			else
			{
				//realloc ok
				s->top = s->base + elem_count;  //actualize s->top
				s->max_size = s->base + 2 * elem_count; //actualize s->max_size
			}
		}
		*(s->top) = address;	//push address on top of stack
		(s->top)++;	//stack top increment
		return OK_STACK;
	}
	else
	{
		//stack given by parameter is NULL
		return ERR_STACK_NULL;
	}
}

/*
	Remove element on top of stack s
*/
void *stack_pop(Tstack *s)
{
	if (s != NULL)
	{
		if (stack_empty(s))
		{
			//pop on empty stack
			return NULL;
		}
		else
		{
			//stack is not empty - return address of element on it's top and remove it
			(s->top)--; //remove element
			return *(s->top);   //return removed element address
		}
	}
	else
	{
		//stack given by parameter is NULL
		return NULL;
	}
}

/*
	Return address of element on top of stack s
*/
void *stack_top(Tstack *s)
{
	if (s != NULL)
	{
		if (stack_empty(s))
		{
			//top on empty stack
			return NULL;
		}
		else
		{
			//stack is not empty - return address of element on it's top
			return *(s->top - 1);	//copy address of element on top of stack
		}
	}
	else
	{
		//stack given by parameter is NULL
		return NULL;
	}
}

/*
	Free allocated memory for stack s
*/
void stack_dispose(Tstack *s)
{
	if (s != NULL)
	{
		free(s->base);  //free memory allocated for elements
		free(s);		//free memory allocated for stack
	}
}

/*
	Check if stack s is empty
	Used in stack_top and stack_pop
*/
bool stack_empty(Tstack *s)
{
	if (s != NULL)
	{
		return (s->top == s->base); //stack has no elements when s->top == s->base
	}
	else
	{
		//stack given by parameter is NULL
		return false;
	}
}


//-----------TEST-----------//
#ifdef STACK_SELFTEST
int main()
{

	Tstack *s = stack_init();
	if (s == NULL)
	{
		printf("Init Error\n");
		return 1;
	}
	void *data = NULL;


	printf("TEST\n----\n\n");
	printf("Stack element count: %d\n", s->top - s->base);
	printf("\nCALL stack_top and stack_pop on empty stack -> error\n");
	data = stack_top(s);
	stack_pop(s);


	printf("\nPUSH values on stack\n");
	for(int i = 0; i < 17; i++)
	{
		stack_push(s,data);
		printf("Element count/Stack size: %d/%d\n", s->top - s->base, s->max_size - s->base);
	}

	printf("\n\nPOP values from stack\n");
	for(int i = 0; i < 8; i++)
	{
		stack_pop(s);
		printf("Element count/Stack size: %d/%d\n", s->top - s->base, s->max_size - s->base);
	}


	printf("\n---------------------\n");


	printf("push x (value 20) on top of stack\n");
	int *x = malloc(sizeof(int));
	*x = 20;
	printf("\nx address=%d\n", x);
	stack_push(s, x);
	printf("\nstore top of stack in y\n");
	int *y = (int *)stack_top(s);
	printf("\ny address=%d\n", y);
	printf("\ny value=%d\n", *y);
	*x = 50;
	printf("\nx value changed to 50\n");
	printf("\ny value=%d\n", *y);


	printf("\n---------------------\n");

	free(x);
	printf("Destroy stack/Init new stack\n");
	stack_dispose(s);
	s = stack_init();

	printf("Data for test:\n");
	int *value[30];
	for(int i = 0; i < 30; i++)
	{
		value[i] = malloc(sizeof(int));
		*(value[i]) = i;
	}
	for(int i = 0; i < 30; i++)
	{
		printf("value[%d] = %d (on address %d)\n", i, *(value[i]), value[i]);
	}

	printf("\nPUSH 20elements on stack\n");
	for(int i = 0; i < 20; i++)
	{
		stack_push(s,value[i]);
		printf("Element count/Stack size: %d/%d\n", s->top - s->base, s->max_size - s->base);
		printf("Address on top of stack = %d (value on this address: %d)\n\n", (int *)stack_top(s), *((int *)stack_top(s)));
	}

	printf("POP 20elements from stack\n");
	int *pop_add;
	for(int i = 0; i < 20; i++)
	{
		pop_add = stack_pop(s);
		printf("Element count/Stack size: %d/%d\n", s->top - s->base, s->max_size - s->base);
		printf("POPed address from top of stack = %d (value on this address: %d)\n\n", (int *)pop_add, *((int *)pop_add));
	}

	printf("PUSH another 10elements on stack\n");
	for(int i = 20; i < 30; i++)
	{
		stack_push(s,value[i]);
		printf("Element count/Stack size: %d/%d\n", s->top - s->base, s->max_size - s->base);
		printf("Address on top of stack = %d (value on this address: %d)\n\n", (int *)stack_top(s), *((int *)stack_top(s)));
	}

	printf("POP 10elements from stack again\n");
	for(int i = 0; i < 10; i++)
	{
		pop_add = stack_pop(s);
		printf("Element count/Stack size: %d/%d\n", s->top - s->base, s->max_size - s->base);
		printf("POPed address from top of stack = %d (value on this address: %d)\n\n", (int *)pop_add, *((int *)pop_add));
	}

	if (stack_empty(s))
	{
		printf("Stack is empty\n");
		printf("Next pop cause error\n");
		pop_add = stack_pop(s);
	}

	for(int i = 0; i < 30; i++)
	{
		free(value[i]);
	}
	stack_dispose(s);

	return 0;
}
#endif
