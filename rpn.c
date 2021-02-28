/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <string.h>

#include "op.h"
#include "rpn.h"

#define MAX_STACK_SIZE 32

typedef struct {
	int sp;
	float elems[MAX_STACK_SIZE];
} RPNStack;

static RPNStack *rpn_stack_init(RPNStack *stack);
static RPNStack *rpn_stack_push(RPNStack *stack, float elem);
static int rpn_stack_peek(float *dest, RPNStack stack);
static int rpn_stack_pop(float *dest, RPNStack *stack);

static RPNStack *
rpn_stack_init(RPNStack *stack)
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

static RPNStack *
rpn_stack_push(RPNStack *stack, float elem)
{
	++stack->sp;
	stack->elems[stack->sp] = elem;

	return stack;
}

static int
rpn_stack_peek(float *dest, RPNStack stack)
{
	if (stack.sp < 0)
		return -1;

	*dest = stack.elems[stack.sp];
	
	return 0;
}

static int
rpn_stack_pop(float *dest, RPNStack *stack)
{
	if (rpn_stack_peek(dest, *stack) < 0)
		return -1;

	--stack->sp;

	return 0;
}

int
rpn_calc(float *dest, char *expr)
{
	RPNStack stack;
	float ax, bx, dx;
	char *ptr, *endptr;
	float (*op_ptr)(float, float);
	
	rpn_stack_init(&stack);

	ptr = strtok(expr, " ");
	while (ptr != NULL) {
		if ((op_ptr = op(ptr)) != NULL) {
			if ((rpn_stack_pop(&bx, &stack) < 0) 
			    || (rpn_stack_pop(&ax, &stack) < 0))
				return -1;

			dx = (*op_ptr)(ax, bx);
		} else { 
			/* strtof() is ISO C99 */
			dx = strtof(ptr, &endptr);
			if (endptr[0] != '\0')
				return -1;
		}

		rpn_stack_push(&stack, dx);
		ptr = strtok(NULL, " ");
	}

	return rpn_stack_pop(dest, &stack);
}

