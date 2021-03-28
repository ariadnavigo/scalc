/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <string.h>

#include "op.h"

/*
 * EXTENDING OP.C:
 *
 * All mathematical operations take the form of a unary or binary function
 * (one double argument or two double arguments, respectively), always 
 * returning a double as a result. All functions are to be defined static, 
 * unless they're defined in an external library (e.g. log() and sqrt(), from 
 * libm).
 *
 * Once you've written your function, in order for the rpn.c module to actually
 * know about it, you must add it into the 'op_defs' array below. Leave the
 * 'dummy' entry at the end.
 *
 * The fields must *strictly* follow this format, where 'name' will be the
 * command users will call your function from scalc's prompt or scripts:
 *
 * { "name", N of arguments (N = 1 or 2), { .nN = pointer-to-func } }
 */

static double op_add(double a, double b);
static double op_subst(double a, double b);
static double op_mult(double a, double b);
static double op_div(double a, double b);

static struct op_reg op_defs[] = {
	{ "+", 2, { .n2 = op_add } },
	{ "-", 2, { .n2 = op_subst } },
	{ "*", 2, { .n2 = op_mult } },
	{ "/", 2, { .n2 = op_div } },
	{ "^", 2, { .n2 = pow } }, /* From libm, directly */
	{ "ln", 1, { .n1 = log } },
	{ "sqrt", 1, { .n1 = sqrt } },
	{ "", 0, { .n1 = NULL } } /* Dummy "terminator" entry */
};

static double
op_add(double a, double b)
{
	return a + b;
}

static double
op_subst(double a, double b)
{
	return op_add(a, -b);
}

static double
op_mult(double a, double b)
{
	return a * b;
}

static double
op_div(double a, double b)
{
	return op_mult(a, 1 / b);
}

struct op_reg *
op(const char *oper)
{
	struct op_reg *ptr;

	for (ptr = op_defs; strcmp(ptr->id, "") != 0; ++ptr) {
		if (strcmp(ptr->id, oper) == 0)
			return ptr;
	}

	return NULL;
}

