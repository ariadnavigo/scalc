/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <string.h>

#include "rpn.h"

#define MAX_STACK_SIZE 32

typedef struct {
	int sp;
	float elems[MAX_STACK_SIZE];
} RPNStack;

static int token_is_op(const char *token);

static RPNStack *rpn_stack_init(RPNStack *stack);
static RPNStack *rpn_stack_push(RPNStack *stack, float elem);
static int rpn_stack_peek(float *dest, RPNStack stack);
static int rpn_stack_pop(float *dest, RPNStack *stack);

static float op(const char *oper, float a, float b);

static int
token_is_op(const char *token)
{
	int res;
	
	res = strcmp(token, "+") && strcmp(token, "-") && strcmp(token, "*")
	      && strcmp(token, "/");

	return (res == 0) ? 1 : 0;
}

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

static float
op(const char *oper, float a, float b)
{
	if (strcmp(oper, "+") == 0)
		return a + b;
	else if (strcmp(oper, "-") == 0)
		return a - b;
	else if (strcmp(oper, "*") == 0)
		return a * b;
	else if (strcmp(oper, "/") == 0)
		return a / b;
	else /* Dummy else until we make this extensible and robust */
		return 0;
}

int
rpn_calc(float *dest, char *expr)
{
	RPNStack stack;
	float ax, bx, dx;
	char *ptr, *endptr;
	
	rpn_stack_init(&stack);

	ptr = strtok(expr, " ");
	while (ptr != NULL) {
		if (token_is_op(ptr) > 0) {
			if ((rpn_stack_pop(&bx, &stack) < 0) 
			    || (rpn_stack_pop(&ax, &stack) < 0))
				return -1;

			dx = op(ptr, ax, bx);
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

