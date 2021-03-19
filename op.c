/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <string.h>

#include "op.h"

static float op_add(float a, float b);
static float op_subst(float a, float b);
static float op_mult(float a, float b);
static float op_div(float a, float b);

/*
 * EXTENDING OP.C:
 *
 * All mathematical operations take the form of a unary or binary function
 * (one float argument or two float arguments, respectively), always returning
 * a float as a result. All functions are to be defined static, unless they're
 * defined in an external library (e.g. logf() and sqrt(), from libm).
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

static struct op_reg op_defs[] = {
	{ "+", 2, { .n2 = op_add } },
	{ "-", 2, { .n2 = op_subst } },
	{ "*", 2, { .n2 = op_mult } },
	{ "/", 2, { .n2 = op_div } },
	{ "^", 2, { .n2 = powf } }, /* From libm, directly */
	{ "ln", 1, { .n1 = logf } },
	{ "sqrt", 1, { .n1 = sqrtf } },
	{ "", 0, { .n1 = NULL } } /* Dummy "terminator" entry */
};

static float
op_add(float a, float b)
{
	return a + b;
}

static float
op_subst(float a, float b)
{
	return op_add(a, -b);
}

static float
op_mult(float a, float b)
{
	return a * b;
}

static float
op_div(float a, float b)
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

