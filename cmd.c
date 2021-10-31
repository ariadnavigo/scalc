/* See LICENSE for copyright and license details. */

#include <stdio.h>
#include <string.h>

#include "stack.h" /* Dependency for cmd.h */
#include "cmd.h"
#include "mem.h"
#include "op.h"
#include "strlcpy.h"
#include "utils.h"

static int cmd_list(void);
static int cmd_print(Stack *stack);
static int cmd_show_stk(Stack *stack);
static int cmd_ver(void);

static const CmdReg cmd_defs[] = {
	{ ":d", CMD_STACK, { .stack = stack_drop } },
	{ ":D", CMD_STACK, { .stack = stack_init } },
	{ ":dup", CMD_STACK, { .stack = stack_dup } },
	{ ":mclr", CMD_NOARGS, { .noargs = mem_clr } },
	{ ":list", CMD_NOARGS, { .noargs = cmd_list } },
	{ ":p", CMD_STACK, { .stack = cmd_print } },
	{ ":P", CMD_STACK, { .stack = cmd_show_stk } },
	{ ":sav", CMD_SETTER, { .setter = mem_set } },
	{ ":swp", CMD_STACK, { .stack = stack_swap } },
	{ ":ver", CMD_NOARGS, { .noargs = cmd_ver } },
	{ "", CMD_NULL, { .noargs = NULL } }
};

static int
cmd_list(void)
{
	const OpReg *ptr;

	for (ptr = op_defs; strncmp(ptr->id, "", OP_NAME_SIZE) != 0; ++ptr)
		printf("%s ", ptr->id);
	putchar('\n');

	return 0;
}

static int
cmd_print(Stack *stack)
{
	int err;
	double buf;

	buf = 0.0;
	err = stack_peek(&buf, *stack);
	if (err < 0)
		return -1;

	print_num(buf);
	return 0;
}

static int
cmd_show_stk(Stack *stack)
{
	int i;

	if (stack->sp < 0) {
		printf("Stack is empty.\n");
		return 0;
	}

	for (i = 0; i < stack->sp + 1; ++i)
		print_num(stack->elems[i]);

	return 0;
}

static int
cmd_ver(void)
{
	printf("scalc %s\n", VERSION);

	return 0;
}

const CmdReg *
cmd(const char *name)
{
	const CmdReg *cmd_ptr;

	for (cmd_ptr = cmd_defs; cmd_ptr->type != CMD_NULL; ++cmd_ptr) {
		if (strncmp(cmd_ptr->id, name, CMD_ID_SIZE) == 0)
			return cmd_ptr;
	}

	err = CMD_ERR_INVALID;
	return cmd_ptr;
}
