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
static double op_prcnt(double n);
static double op_mod(double p, double q);
static double op_fact(double n);
static double op_npr(double n, double r);
static double op_ncr(double n, double r);

/* Constants */
static double op_cst_e(void);
static double op_cst_pi(void);

const OpReg op_defs[] = {
	{ "+", OP_ARG2, { .n2 = op_add } },
	{ "-", OP_ARG2, { .n2 = op_subst } },
	{ "*", OP_ARG2, { .n2 = op_mult } },
	{ "/", OP_ARG2, { .n2 = op_div } },
	{ "^", OP_ARG2, { .n2 = pow } }, /* From libm, directly */
	{ "%", OP_ARG1, { .n1 = op_prcnt } },
	{ "abs", OP_ARG1, { .n1 = fabs } },
	{ "ln", OP_ARG1, { .n1 = log } },
	{ "sqrt", OP_ARG1, { .n1 = sqrt } },
	{ "mod", OP_ARG2, { .n2 = op_mod } },
	{ "!", OP_ARG1, { .n1 = op_fact } },
	{ "P", OP_ARG2, { .n2 = op_npr } },
	{ "C", OP_ARG2, { .n2 = op_ncr } },
	{ "sin", OP_ARG1, { .n1 = sin } },
	{ "cos", OP_ARG1, { .n1 = cos } },
	{ "tan", OP_ARG1, { .n1 = tan } },
	{ "asin", OP_ARG1, { .n1 = asin } },
	{ "acos", OP_ARG1, { .n1 = acos } },
	{ "atan", OP_ARG1, { .n1 = atan } },
	{ "e", OP_ARG0, { .n0 = op_cst_e } },
	{ "pi", OP_ARG0, { .n0 = op_cst_pi } },
	{ "", OP_NULL, { .n0 = NULL } } /* Dummy "terminator" entry */
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
op_prcnt(double n)
{
	return n / 100;
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

	for (ptr = op_defs; ptr->type != OP_NULL; ++ptr) {
		if (strncmp(ptr->id, oper, OP_NAME_SIZE) == 0)
			return ptr;
	}

	return NULL;
}

