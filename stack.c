/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <string.h>

#include "stack.h"

int stack_err = STK_SUCCESS;

int
stack_init(Stack *stack)
{
	/*
	 * We only initialize the "pointer." There is NO need to zero-out stuff
	 * on initializiation... No computer does :D As long as we know
	 * precisely where we are, it is the "pointer" which defines the
	 * current size of the stack.
	 */

	stack->sp = -1;

	return 0;
}

int
stack_push(Stack *stack, double elem)
{
	/* Let's avoid stack overflows */
	if (++stack->sp == STK_STACK_SIZE) {
		--stack->sp;
		stack_err = STK_ERR_STACK_MAX;
		return -1;
	}

	stack->elems[stack->sp] = elem;

	return 0;
}

int
stack_pop(double *dest, Stack *stack)
{
	if (stack_peek(dest, *stack) < 0)
		return -1;

	--stack->sp;

	return 0;
}

int
stack_drop(Stack *stack)
{
	if (stack->sp < 0) {
		stack_err = STK_ERR_STACK_MIN;
		return -1;
	}
	
	--stack->sp;

	return 0;
}

int
stack_dup(Stack *stack)
{
	double dup;

	if (stack_peek(&dup, *stack) < 0)
		return -1;

	if (stack_push(stack, dup) < 0)
		return -1;

	return 0; 
}

int
stack_peek(double *dest, Stack stack)
{
	if (stack.sp < 0) {
		stack_err = STK_ERR_STACK_MIN;
		return -1;
	}

	*dest = stack.elems[stack.sp];

	return 0;
}

int
stack_swap(Stack *stack)
{
	double ax, bx;

	if ((stack_pop(&ax, stack) < 0) || (stack_pop(&bx, stack) < 0))
		return -1;

	if ((stack_push(stack, ax) < 0) || (stack_push(stack, bx) < 0))
		return -1;

	return 0;
}

