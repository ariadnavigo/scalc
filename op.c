/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <string.h>

#include "op.h"

#define OP_NAME_SIZE 16

struct op_reg {
	char id[OP_NAME_SIZE];
	float (*op_ptr)(float, float);
};

static float op_add(float a, float b);
static float op_subst(float a, float b);
static float op_mult(float a, float b);
static float op_div(float a, float b);

static struct op_reg op_catalog[] = {
	{ "+", op_add },
	{ "-", op_subst },
	{ "*", op_mult },
	{ "/", op_div },
	{ "^", powf },
	{ "", NULL }
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

float 
(*op(const char *oper))(float, float)
{
	struct op_reg *ptr;

	for (ptr = op_catalog; strcmp(ptr->id, "") != 0; ++ptr) {
		if (strcmp(ptr->id, oper) == 0)
			return ptr->op_ptr;
	}

	return NULL;
}

