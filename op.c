/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <string.h>

#include "op.h"

static float op_add(float a, float b);
static float op_subst(float a, float b);
static float op_mult(float a, float b);
static float op_div(float a, float b);

static struct op_reg op_defs[] = {
	{ "+", 2, { .n2 = op_add }},
	{ "-", 2, { .n2 = op_subst }},
	{ "*", 2, { .n2 = op_mult }},
	{ "/", 2, { .n2 = op_div }},
	{ "^", 2, { .n2 = powf }},
	{ "ln", 1, { .n1 = logf }},
	{ "", 0, { .n1 = NULL }}
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

