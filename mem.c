/* See LICENSE file for copyright and license details. */

#include "mem.h"

static int mem_var_to_i(char var);

static double mem[MEM_SIZE];

static int
mem_var_to_i(char var)
{
	int i;

	i = var - 'A';
	if (i < 0 || i >= MEM_SIZE)
		return -1;

	return i;
}

int
mem_get(double *val, char var)
{
	int i;

	if ((i = mem_var_to_i(var)) < 0)
		return -1;

	*val = mem[i];

	return 0;
}

int
mem_set(char var, double val)
{
	int i;

	if ((i = mem_var_to_i(var)) < 0)
		return -1;

	mem[i] = val;

	return 0;
}
