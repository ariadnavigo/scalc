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
	/* Let's avoid stack overflows */
	if (++stack->sp == MAX_STACK_SIZE) {
		--stack->sp;
		return NULL;
	}

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
rpn_calc(float *dest, const char *expr)
{
	RPNStack stack;
	float ax, bx, dx;
	char expr_cpy[RPN_EXPR_SIZE];
	char *ptr, *endptr;
	float (*op_ptr)(float, float);
	
	rpn_stack_init(&stack);

	/* We need to operate on a copy, as strtok is destructive. */
	strncpy(expr_cpy, expr, RPN_EXPR_SIZE);
	ptr = strtok(expr_cpy, " ");
	while (ptr != NULL) {
		/* strtof() is ISO C99 */
		dx = strtof(ptr, &endptr);
		if (endptr[0] != '\0') {
			if ((op_ptr = op(ptr)) != NULL) {
				if ((rpn_stack_pop(&bx, &stack) < 0) 
				    || (rpn_stack_pop(&ax, &stack) < 0))
					return RPN_ERR_STACK_MIN;
			} else {
				return RPN_ERR_OP_UNDEF;
			}

			dx = (*op_ptr)(ax, bx);
		}
		
		if (rpn_stack_push(&stack, dx) == NULL)
			return RPN_ERR_STACK_MAX;
		ptr = strtok(NULL, " ");
	}

	if (rpn_stack_pop(dest, &stack) < 0)
		return RPN_ERR_STACK_MIN;

	return RPN_SUCCESS;
}

const char *
rpn_strerr(int rpnerr)
{
	switch (rpnerr) {
	case RPN_SUCCESS:
		return "success.";
	case RPN_ERR_OP_UNDEF:
		return "operation not defined.";
	case RPN_ERR_STACK_MAX:
		return "too many elements stored in stack.";
	case RPN_ERR_STACK_MIN:
		return "too few elements in stack.";
	default:
		return "unknown error.";
	}
}
