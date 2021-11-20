/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "utils.h"

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
	if (++stack->sp == STACK_SIZE) {
		--stack->sp;
		err = STACK_ERR_MAX;
		return -1;
	}

	stack->elems[stack->sp] = elem;

	return 0;
}

int
stack_pop(double *dest, Stack *stack)
{
	if (stack_peek(dest, 0, *stack) < 0)
		return -1;

	--stack->sp;

	return 0;
}

int
stack_drop(Stack *stack)
{
	if (stack->sp < 0) {
		err = STACK_ERR_MIN;
		return -1;
	}

	--stack->sp;

	return 0;
}

int
stack_dup(Stack *stack)
{
	double dup;

	if (stack_peek(&dup, 0, *stack) < 0)
		return -1;

	if (stack_push(stack, dup) < 0)
		return -1;

	return 0;
}

int
stack_peek(double *dest, int i, Stack stack)
{
	int index;

	if ((index = stack.sp - i) < 0) {
		err = STACK_ERR_MIN;
		return -1;
	}

	*dest = stack.elems[index];

	return 0;
}

int
stack_swap(Stack *stack)
{
	double ax, bx;

	/* If less than 2 elements in stack */
	if (stack->sp < 1) {
		err = STACK_ERR_MIN;
		return -1;
	}

	/* This is totally safe after the test above */
	stack_pop(&ax, stack);
	stack_pop(&bx, stack);
	stack_push(stack, ax);
	stack_push(stack, bx);

	return 0;
}
