/* See LICENSE file for copyright and license details. */

#include <math.h>
#include <string.h>

#include "op.h"

static float op_add(float a, float b);
static float op_subst(float a, float b);
static float op_mult(float a, float b);
static float op_div(float a, float b);

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
	if (strcmp(oper, "+") == 0)
		return op_add;
	else if (strcmp(oper, "-") == 0)
		return op_subst;
	else if (strcmp(oper, "*") == 0)
		return op_mult;
	else if (strcmp(oper, "/") == 0)
		return op_div;
	else if (strcmp(oper, "^") == 0)
		return powf; /* From math.h */
	else
		return NULL;
}