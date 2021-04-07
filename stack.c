/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <string.h>

#include "op.h"
#include "stack.h"

Stack *
stack_init(Stack *stack)
{
	/*
	 * We only initialize the "pointer." There is NO need to zero-out stuff
	 * on initializiation... No computer does :D As long as we know
	 * precisely where we are, it is the "pointer" which defines the
	 * current size of the stack.
	 */

	stack->sp = -1;

	return stack;
}

Stack *
stack_push(Stack *stack, double elem)
{
	/* Let's avoid stack overflows */
	if (++stack->sp == STK_STACK_SIZE) {
		--stack->sp;
		return NULL;
	}

	stack->elems[stack->sp] = elem;

	return stack;
}

int
stack_pop(double *dest, Stack *stack)
{
	int err;

	err = stack_peek(dest, *stack);
	if (err != STK_SUCCESS)
		return err;

	--stack->sp;

	return STK_SUCCESS;
}

int
stack_drop(Stack *stack)
{
	if (stack->sp < 0)
		return STK_ERR_STACK_MIN;
	
	--stack->sp;

	return STK_SUCCESS;
}

int
stack_dup(Stack *stack)
{
	double dup;
	int err;

	err = stack_peek(&dup, *stack);
	if (err != STK_SUCCESS)
		return err;

	if (stack_push(stack, dup) == NULL)
		return STK_ERR_STACK_MAX;

	return STK_SUCCESS; 
}

int
stack_peek(double *dest, Stack stack)
{
	if (stack.sp < 0)
		return STK_ERR_STACK_MIN;

	*dest = stack.elems[stack.sp];

	return STK_SUCCESS;
}

int
stack_swap(Stack *stack)
{
	int err;
	double ax, bx;

	if (((err = stack_pop(&ax, stack)) != STK_SUCCESS)
	    || ((err = stack_pop(&bx, stack)) != STK_SUCCESS))
		return err;

	if ((stack_push(stack, ax) == NULL)
	    || (stack_push(stack, bx) == NULL))
		return STK_ERR_STACK_MAX;

	return STK_SUCCESS;
}

