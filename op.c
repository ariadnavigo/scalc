/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <string.h>

#include "op.h"

static float op_add(float a, float b);
static float op_subst(float a, float b);
static float op_mult(float a, float b);
static float op_div(float a, float b);

static struct op_reg op_catalog[] = {
	{ "+", 2, { .op2n = op_add }},
	{ "-", 2, { .op2n = op_subst }},
	{ "*", 2, { .op2n = op_mult }},
	{ "/", 2, { .op2n = op_div }},
	{ "^", 2, { .op2n = powf }},
	{ "ln", 1, { .op1n = logf }},
	{ "", 0, { .op1n = NULL }}
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

	for (ptr = op_catalog; strcmp(ptr->id, "") != 0; ++ptr) {
		if (strcmp(ptr->id, oper) == 0)
			return ptr;
	}

	return NULL;
}

