/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "op.h"
#include "utils.h"

#define OP_E 2.71828182845904523536
#define OP_PI 3.14159265358979323846

static double op_add(double p, double q);
static double op_subst(double p, double q);
static double op_mult(double p, double q);
static double op_div(double p, double q);
static double op_prcnt(double n);
static double op_mod(double p, double q);
static double op_fact(double n);
static double op_npr(double n, double r);
static double op_ncr(double n, double r);
static double op_tan(double n);
static double op_cot(double n);
static double op_sec(double n);
static double op_csc(double n);
static double op_acot(double n);
static double op_asec(double n);
static double op_acsc(double n);
static double op_todeg(double n);
static double op_torad(double n);

/* Constants */
static double op_cst_e(void);
static double op_cst_pi(void);

const OpReg op_defs[] = {
	{ "+", 2, { .n2 = op_add }, "Addition" },
	{ "-", 2, { .n2 = op_subst }, "Substraction" },
	{ "*", 2, { .n2 = op_mult }, "Multiplication" },
	{ "/", 2, { .n2 = op_div }, "Division" },
	{ "^", 2, { .n2 = pow }, "Exponent" },
	{ "%", 1, { .n1 = op_prcnt }, "Percentage" },
	{ "abs", 1, { .n1 = fabs }, "Absolute value" },
	{ "ln", 1, { .n1 = log }, "Natural logarithm" },
	{ "sqrt", 1, { .n1 = sqrt }, "Square root" },
	{ "mod", 2, { .n2 = op_mod }, "Modulo" },
	{ "!", 1, { .n1 = op_fact }, "Factorial" },
	{ "nPr", 2, { .n2 = op_npr }, "Permutation operation" },
	{ "nCr", 2, { .n2 = op_ncr }, "Binomial coefficient" },
	{ "sin", 1, { .n1 = sin }, "Sine (in radians)" },
	{ "cos", 1, { .n1 = cos }, "Cosine (in radians)" },
	{ "tan", 1, { .n1 = op_tan }, "Tangent (in radians)" },
	{ "cot", 1, { .n1 = op_cot }, "Cotangent (in radians)" },
	{ "sec", 1, { .n1 = op_sec }, "Secant (in radians)" },
	{ "csc", 1, { .n1 = op_csc }, "Cosecant (in radians)" },
	{ "asin", 1, { .n1 = asin }, "Arcsine (returns radians)" },
	{ "acos", 1, { .n1 = acos }, "Arccosine (returns radians)" },
	{ "atan", 1, { .n1 = atan }, "Arctangent (returns radians)" },
	{ "acot", 1, { .n1 = op_acot }, "Arccotagent (returns radians)" },
	{ "asec", 1, { .n1 = op_asec }, "Arcsecant (returns radians)" },
	{ "acsc", 1, { .n1 = op_acsc }, "Arccosecant (returns radians)" },
	{ "todeg", 1, { .n1 = op_todeg }, "Convert radians to degrees" },
	{ "torad", 1, { .n1 = op_torad }, "Convert degrees to radians" },
	{ "e", 0, { .n0 = op_cst_e }, "The e constant" },
	{ "pi", 0, { .n0 = op_cst_pi }, "The pi constant" },
	{ "", -1, { .n0 = NULL }, "" } /* Dummy "terminator" entry */
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
		return NAN;

	return op_fact(n) / op_fact(n - r);
}

static double
op_ncr(double n, double r)
{
	if (r > n)
		return NAN;

	return op_fact(n) / (op_fact(r) * op_fact(n - r));
}

static double
op_tan(double n)
{
	if (fmod(n, OP_PI / 2) == 0)
		return NAN;

	return tan(n);
}

static double
op_cot(double n)
{
	return 1 / tan(n);
}

static double
op_sec(double n)
{
	return 1 / cos(n);
}

static double
op_csc(double n)
{
	return 1 / sin(n);
}

static double
op_acot(double n)
{
	return OP_PI / 2 - atan(n);
}

static double
op_asec(double n)
{
	return acos(1 / n);
}

static double
op_acsc(double n)
{
	return asin(1 / n);
}

static double
op_todeg(double n)
{
	return n * 180 / OP_PI;
}

static double
op_torad(double n)
{
	return n * OP_PI / 180;
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

	for (ptr = op_defs; op_valid(ptr) == 0; ++ptr) {
		if (strncmp(ptr->id, oper, OP_NAME_SIZE) == 0)
			return ptr;
	}

	/* If no match is found, we return the "Null" pointer */
	err = OP_ERR_INVALID;
	return ptr;
}

int
op_valid(const OpReg *ptr)
{
	switch (ptr->arg_n) {
	case 2:
		if (ptr->func.n0 == NULL)
			return -1;
		break;
	case 1:
		if (ptr->func.n1 == NULL)
			return -1;
		break;
	case 0:
		if (ptr->func.n0 == NULL)
			return -1;
		break;
	default:
		return -1;
	}

	return 0;
}
