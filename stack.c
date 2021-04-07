/* See LICENSE file for copyright and license details. */

#include <stdlib.h>
#include <string.h>

#include "op.h"
#include "stack.h"
#include "strlcpy.h"

static Stack *stack_push(Stack *stack, double elem);
static int stack_pop(double *dest, Stack *stack);

static Stack *
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

static int
stack_pop(double *dest, Stack *stack)
{
	int err;

	err = stack_peek(dest, *stack);
	if (err != STK_SUCCESS)
		return err;

	--stack->sp;

	return STK_SUCCESS;
}

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

int
stack_calc(double *dest, const char *expr, Stack *stack)
{
	int arg_i, err;
	double args[2];
	double dx;
	char expr_cpy[STK_EXPR_SIZE];
	char *ptr, *endptr;
	const OpReg *op_ptr;

	/* We need to operate on a copy, as strtok is destructive. */
	strlcpy(expr_cpy, expr, STK_EXPR_SIZE);
	ptr = strtok(expr_cpy, " ");
	while (ptr != NULL) {
		dx = strtof(ptr, &endptr);
		if (endptr[0] == '\0')
			goto pushnum; /* If number, skip further parsing */

		if ((op_ptr = op(ptr)) == NULL)
			return STK_ERR_OP_UNDEF;

		/* Traversing backwards because we're poping off the stack */
		for (arg_i = op_ptr->argn - 1; arg_i >= 0; --arg_i) {
			err = stack_pop(&args[arg_i], stack);
			if (err != STK_SUCCESS)
				return err;
		}

		if (op_ptr->argn == 2)
			dx = (*op_ptr->func.n2)(args[0], args[1]);
		else if (op_ptr->argn == 1)
			dx = (*op_ptr->func.n1)(args[0]);
		else if (op_ptr->argn == 0)
			dx = (*op_ptr->func.n0)();
		else
			return STK_ERR_OP_INV;

pushnum:
		if (stack_push(stack, dx) == NULL)
			return STK_ERR_STACK_MAX;
		ptr = strtok(NULL, " ");
	}

	return stack_peek(dest, *stack);
}

const char *
stack_strerr(int err)
{
	switch (err) {
	case STK_SUCCESS:
		return "success.";
	case STK_ERR_OP_INV:
		return "operation invalidly defined.";
	case STK_ERR_OP_UNDEF:
		return "operation not defined.";
	case STK_ERR_STACK_MAX:
		return "too many elements stored in stack.";
	case STK_ERR_STACK_MIN:
		return "too few elements in stack.";
	default:
		return "unknown error.";
	}
}
