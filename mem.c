/* See LICENSE file for copyright and license details. */

#include <string.h>

#include "mem.h"
#include "utils.h"

static int mem_var_to_i(char var);

static double mem[MEM_SIZE];

static int
mem_var_to_i(char var)
{
	int i;

	i = var - 'A';
	if (i < 0 || i >= MEM_SIZE) {
		err = MEM_ERR_NOT_FOUND;
		return -1;
	}

	return i;
}

void
mem_clr(void)
{
	memset(mem, 0, sizeof(mem));
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
