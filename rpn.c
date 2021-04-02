/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <string.h>

#include "op.h"
#include "rpn.h"

static RPNStack *rpn_stack_push(RPNStack *stack, double elem);
static int rpn_stack_pop(double *dest, RPNStack *stack);

static RPNStack *
rpn_stack_push(RPNStack *stack, double elem)
{
	/* Let's avoid stack overflows */
	if (++stack->sp == RPN_STACK_SIZE) {
		--stack->sp;
		return NULL;
	}

	stack->elems[stack->sp] = elem;

	return stack;
}

static int
rpn_stack_pop(double *dest, RPNStack *stack)
{
	int rpn_err;

	rpn_err = rpn_stack_peek(dest, *stack);
	if (rpn_err != RPN_SUCCESS)
		return rpn_err;

	--stack->sp;

	return RPN_SUCCESS;
}

RPNStack *
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

int
rpn_stack_drop(RPNStack *stack)
{
	if (stack->sp < 0)
		return RPN_ERR_STACK_MIN;
	
	--stack->sp;

	return RPN_SUCCESS;
}

int
rpn_stack_dup(RPNStack *stack)
{
	double dup;
	int rpn_err;

	rpn_err = rpn_stack_peek(&dup, *stack);
	if (rpn_err != RPN_SUCCESS)
		return rpn_err;

	if (rpn_stack_push(stack, dup) == NULL)
		return RPN_ERR_STACK_MAX;

	return RPN_SUCCESS; 
}

int
rpn_stack_peek(double *dest, RPNStack stack)
{
	if (stack.sp < 0)
		return RPN_ERR_STACK_MIN;

	*dest = stack.elems[stack.sp];

	return RPN_SUCCESS;
}

int
rpn_stack_swap(RPNStack *stack)
{
	int rpn_err;
	double ax, bx;

	if (((rpn_err = rpn_stack_pop(&ax, stack)) != RPN_SUCCESS)
	    || ((rpn_err = rpn_stack_pop(&bx, stack)) != RPN_SUCCESS))
		return rpn_err;

	if ((rpn_stack_push(stack, ax) == NULL)
	    || (rpn_stack_push(stack, bx) == NULL))
		return RPN_ERR_STACK_MAX;

	return RPN_SUCCESS;
}

int
rpn_calc(double *dest, const char *expr, RPNStack *stack)
{
	int rpn_err;
	double ax, bx, dx;
	char expr_cpy[RPN_EXPR_SIZE];
	char *ptr, *endptr;
	const struct op_reg *op_ptr;

	/* We need to operate on a copy, as strtok is destructive. */
	strncpy(expr_cpy, expr, RPN_EXPR_SIZE);
	ptr = strtok(expr_cpy, " ");
	while (ptr != NULL) {
		/* strtof() is ISO C99 */
		dx = strtof(ptr, &endptr);
		if (endptr[0] != '\0') {
			if ((op_ptr = op(ptr)) == NULL)
				return RPN_ERR_OP_UNDEF;

			if (op_ptr->argn > 1) {
				rpn_err = rpn_stack_pop(&bx, stack);
				if (rpn_err != RPN_SUCCESS)
					return rpn_err;
			}

			rpn_err = rpn_stack_pop(&ax, stack);
			if (rpn_err != RPN_SUCCESS)
				return rpn_err;

			if (op_ptr->argn == 2)
				dx = (*op_ptr->func.n2)(ax, bx);
			else if (op_ptr->argn == 1)
				dx = (*op_ptr->func.n1)(ax);
			else
				return RPN_ERR_OP_INV;
		}

		if (rpn_stack_push(stack, dx) == NULL)
			return RPN_ERR_STACK_MAX;
		ptr = strtok(NULL, " ");
	}

	return rpn_stack_peek(dest, *stack);
}

const char *
rpn_strerr(int rpnerr)
{
	switch (rpnerr) {
	case RPN_SUCCESS:
		return "success.";
	case RPN_ERR_OP_INV:
		return "operation invalidly defined.";
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
