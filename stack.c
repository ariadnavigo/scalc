/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "utils.h"

Stack stack;

int
stack_init(void)
{
	/*
	 * We only initialize the "pointer." There is NO need to zero-out stuff
	 * on initializiation... No computer does :D As long as we know
	 * precisely where we are, it is the "pointer" which defines the
	 * current size of the stack.
	 */

	stack.sp = -1;

	return 0;
}

int
stack_push(double elem)
{
	/* Let's avoid stack overflows */
	if (++stack.sp == STACK_SIZE) {
		--stack.sp;
		err = STACK_ERR_MAX;
		return -1;
	}

	stack.elems[stack.sp] = elem;

	return 0;
}

int
stack_pop(double *dest)
{
	if (stack_peek(dest, 0) < 0)
		return -1;

	--stack.sp;

	return 0;
}

int
stack_drop(int n)
{
	if (n > stack.sp + 1 || stack.sp < 0) {
		err = STACK_ERR_MIN;
		return -1;
	}

	stack.sp -= n;
	if (stack.sp < 0)
		stack.sp = -1;

	return 0;
}

int
stack_dup(void)
{
	double dup;

	if (stack_peek(&dup, 0) < 0)
		return -1;

	if (stack_push(dup) < 0)
		return -1;

	return 0;
}

int
stack_peek(double *dest, int i)
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
stack_swap(void)
{
	double ax, bx;

	/* If less than 2 elements in stack */
	if (stack.sp < 1) {
		err = STACK_ERR_MIN;
		return -1;
	}

	/* This is totally safe after the test above */
	stack_pop(&ax);
	stack_pop(&bx);
	stack_push(ax);
	stack_push(bx);

	return 0;
}
