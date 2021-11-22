/* See LICENSE for copyright and license details. */

#include <stdio.h>
#include <string.h>

#include "stack.h" /* Dependency for cmd.h */
#include "cmd.h"
#include "mem.h"
#include "op.h"
#include "strlcpy.h"
#include "utils.h"

static int cmd_d(const char *args);
static int cmd_D(const char *args);
static int cmd_dup(const char *args);
static int cmd_mclr(const char *args);
static int cmd_list(const char *args);
static int cmd_p(const char *args);
static int cmd_sav(const char *args);
static int cmd_swp(const char *args);
static int cmd_ver(const char *args);

static const CmdReg cmd_defs[] = {
	{ ":d", cmd_d },
	{ ":D", cmd_D },
	{ ":dup", cmd_dup },
	{ ":mclr", cmd_mclr },
	{ ":list", cmd_list },
	{ ":p", cmd_p },
	{ ":sav", cmd_sav },
	{ ":swp", cmd_swp },
	{ ":ver", cmd_ver },
	{ "", NULL }
};

extern Stack stack;

static int
cmd_d(const char *args)
{
	int i, n, res;

	if (args == NULL || sscanf(args, "%d", &n) == EOF)
		n = 1;

	for (i = 0; i < n; ++i) {
		if ((res = stack_drop(&stack)) < 0)
			return -1;
	}

	return 0;
}

static int
cmd_D(const char *args)
{
	return stack_init(&stack);
}

static int
cmd_dup(const char *args)
{
	return stack_dup(&stack);
}

static int
cmd_mclr(const char *args)
{
	return mem_clr(); 
}

static int
cmd_list(const char *args)
{
	const OpReg *ptr;

	for (ptr = op_defs; strncmp(ptr->id, "", OP_NAME_SIZE) != 0; ++ptr)
		printf("%s ", ptr->id);
	putchar('\n');

	return 0;
}

static int
cmd_p(const char *args)
{
	int i, n;
	double buf;
	
	if (args == NULL || sscanf(args, "%d", &n) == EOF)
		n = 1;

	/* If n is neg, we want to print the whole stack at once. */
	if (n < 0)
		n = stack.sp + 1;

	for (i = 0; i < n; ++i) {
		buf = 0.0;
		if (stack_peek(&buf, i, stack) < 0)
			break;

		print_num(buf);
	}

	return 0;
}

static int
cmd_sav(const char *args)
{
	char var;
	double buf;

	if (args == NULL || sscanf(args, "%c", &var) == EOF)
		return -1;

	if (stack_peek(&buf, 0, stack) < 0)
		return -1;

	return mem_set(var, buf);
}

static int
cmd_swp(const char *args)
{
	return stack_swap(&stack);
}

static int
cmd_ver(const char *args)
{
	printf("scalc %s\n", VERSION);

	return 0;
}

const CmdReg *
cmd(const char *name)
{
	const CmdReg *cmd_ptr;

	for (cmd_ptr = cmd_defs; cmd_ptr->func != NULL; ++cmd_ptr) {
		if (strncmp(cmd_ptr->id, name, CMD_ID_SIZE) == 0)
			return cmd_ptr;
	}

	err = CMD_ERR_INVALID;
	return cmd_ptr;
}
