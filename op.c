/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "op.h"

#define OP_E 2.71828182845904523536
#define OP_PI 3.14159265358979323846

/*
 * EXTENDING OP.C:
 *
 * All mathematical operations take the form of a unary or binary function
 * (one double argument or two double arguments, respectively), always 
 * returning a double as a result. All functions are to be defined static, 
 * unless they're defined in an external library (e.g. log() and sqrt(), from 
 * libm).
 *
 * Mathematical constants are defined as 0-ary functions.
 *
 * Once you've written your function, in order for the rpn.c module to actually
 * know about it, you must add it into the 'op_defs' array below. Leave the
 * 'dummy' entry at the end.
 *
 * The fields must *strictly* follow this format, where 'name' will be the
 * command users will call your function from scalc's prompt or scripts:
 *
 * { "name", N of arguments (N = 0, 1 or 2), { .nN = pointer-to-func } }
 */

static double op_add(double p, double q);
static double op_subst(double p, double q);
static double op_mult(double p, double q);
static double op_div(double p, double q);
static double op_mod(double p, double q);
static double op_fact(double n);
static double op_npr(double n, double r);
static double op_ncr(double n, double r);

/* Constants */
static double op_cst_e(void);
static double op_cst_pi(void);

const OpReg op_defs[] = {
	{ "+", 2, { .n2 = op_add } },
	{ "-", 2, { .n2 = op_subst } },
	{ "*", 2, { .n2 = op_mult } },
	{ "/", 2, { .n2 = op_div } },
	{ "^", 2, { .n2 = pow } }, /* From libm, directly */
	{ "ln", 1, { .n1 = log } },
	{ "sqrt", 1, { .n1 = sqrt } },
	{ "%", 2, { .n2 = op_mod } },
	{ "!", 1, { .n1 = op_fact } },
	{ "P", 2, { .n2 = op_npr } },
	{ "C", 2, { .n2 = op_ncr } },
	{ "sin", 1, { .n1 = sin } },
	{ "cos", 1, { .n1 = cos } },
	{ "tan", 1, { .n1 = tan } },
	{ "asin", 1, { .n1 = asin } },
	{ "acos", 1, { .n1 = acos } },
	{ "atan", 1, { .n1 = atan } },
	{ "e", 0, { .n0 = op_cst_e } },
	{ "pi", 0, { .n0 = op_cst_pi } },
	{ "", 0, { .n0 = NULL } } /* Dummy "terminator" entry */
};

static double
op_add(double p, double q)
{
	return p + q;
}

static double
op_subst(double p, double q)
{
	return p - q;
}

static double
op_mult(double p, double q)
{
	return p * q;
}

static double
op_div(double p, double q)
{
	return p / q;
}

static double
op_mod(double p, double q)
{
	return (double)((int64_t)p % (int64_t)q);
}

static double
op_fact(double n)
{
	double res, i;

	res = 1;
	for (i = n; i > 1; --i)
		res *= i;

	return res;
}

static double
op_npr(double n, double r)
{
	if (r > n)
		return 0;

	return op_fact(n) / op_fact(n - r);
}

static double
op_ncr(double n, double r)
{
	return op_fact(n) / (op_fact(r) * op_fact(n - r));
}

static double
op_cst_e(void)
{
	return OP_E;
}

static double
op_cst_pi(void)
{
	return OP_PI;
}

const OpReg *
op(const char *oper)
{
	const OpReg *ptr;

	for (ptr = op_defs; strncmp(ptr->id, "", OP_NAME_SIZE) != 0; ++ptr) {
		if (strncmp(ptr->id, oper, OP_NAME_SIZE) == 0)
			return ptr;
	}

	return NULL;
}

