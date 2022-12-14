/* See LICENSE for copyright and license details. */

#include <stdarg.h>
#include <stddef.h> /* Dependency for strlcpy.h */
#include <stdio.h>
#include <string.h>

#include "stack.h" /* Dependency for cmd.h */
#include "cmd.h"
#include "mem.h"
#include "op.h"
#include "sline.h"
#include "strlcpy.h"
#include "utils.h"

static int get_args(const char *args, const char *fmt, ...);

static int cmd_d(const char *args);
static int cmd_dmp(const char *args);
static int cmd_dup(const char *args);
static int cmd_mclr(const char *args);
static int cmd_list(const char *args);
static int cmd_p(const char *args);
static int cmd_sav(const char *args);
static int cmd_swp(const char *args);
static int cmd_ver(const char *args);
static int cmd_whatis(const char *args);

static const CmdReg cmd_defs[] = {
	{ ":d", cmd_d, "Drop the stack." },
	{ ":dmp", cmd_dmp, "Dump session to file." },
	{ ":dup", cmd_dup, "Duplicate last element in stack." },
	{ ":mclr", cmd_mclr, "Clear all memory registers." },
	{ ":list", cmd_list, "List all available operations." },
	{ ":p", cmd_p, "Print stack." },
	{ ":sav", cmd_sav, "Save value to register." },
	{ ":swp", cmd_swp, "Swap the two last elements in stack." },
	{ ":ver", cmd_ver, "Shows scalc version information." },
	{ ":whatis", cmd_whatis, "Show info on command or operation." },
	{ "", NULL, "" }
};

static int
get_args(const char *args, const char *fmt, ...)
{
	va_list ap;
	int matches;
	
	if (args == NULL || fmt == NULL)
		return -1;

	va_start(ap, fmt);
	matches = vsscanf(args, fmt, ap);
	va_end(ap);

	if (matches == EOF)
		return -1;

	return matches;
}

static int
cmd_d(const char *args)
{
	int n;

	if (get_args(args, "%d", &n) < 0)
		n = 1;

	if (n < 0)
		return stack_init();

	if (stack_drop(n) < 0)
		return -1;

	return 0;
}

static int
cmd_dmp(const char *args)
{
	int i;
	FILE *fp;
	const char *hist_ptr;

	if (args == NULL || strlen(args) == 0) {
		err = CMD_ERR_FEW_ARGS;
		return -1;
	}

	if ((fp = fopen(args, "w")) == NULL) {
		err = CMD_ERR_FILE_IO;
		return -1;
	}

	for (i = 0; (hist_ptr = sline_history_get(i)) != NULL; ++i) {
		/* We skip commands and blank lines */
		if (strlen(hist_ptr) == 0 || hist_ptr[0] == ':')
			continue;

		fprintf(fp, "%s\n", hist_ptr);
	}

	fclose(fp);

	return 0;
}

static int
cmd_dup(const char *args)
{
	get_args(args, NULL);

	return stack_dup();
}

static int
cmd_mclr(const char *args)
{
	get_args(args, NULL);

	return mem_clr(); 
}

static int
cmd_list(const char *args)
{
	const OpReg *ptr;

	get_args(args, NULL);

	for (ptr = op_defs; strncmp(ptr->id, "", OP_NAME_SIZE) != 0; ++ptr)
		printf("%s ", ptr->id);
	putchar('\n');

	return 0;
}

static int
cmd_p(const char *args)
{
	int n;
	double buf;
	
	if (get_args(args, "%d", &n) < 0)
		n = 1;

	/* If n is neg, we want to print the whole stack at once. */
	if (n < 0)
		n = stack.sp;
	else
		--n; /* Substract one so n becomes an array index. */

	while (n >= 0) {
		buf = 0.0;
		if (stack_peek(&buf, n) < 0)
			return -1;

		print_num(buf);
		--n;
	}

	return 0;
}

static int
cmd_sav(const char *args)
{
	char var;
	double buf;

	if (get_args(args, "%c", &var) < 0) {
		err = CMD_ERR_FEW_ARGS;
		return -1;
	}

	if (stack_peek(&buf, 0) < 0)
		return -1;

	return mem_set(var, buf);
}

static int
cmd_swp(const char *args)
{
	get_args(args, NULL);

	return stack_swap();
}

static int
cmd_ver(const char *args)
{
	get_args(args, NULL);

	printf("scalc %s (sline %s)\n", VERSION, sline_version());

	return 0;
}

static int
cmd_whatis(const char *args)
{
	const OpReg *op_ptr;
	const CmdReg *cmd_ptr;
	char query[OP_DESC_SIZE];
	const char *id, *desc;

	if (get_args(args, "%s", query) < 0) {
		err = CMD_ERR_FEW_ARGS;
		return -1;
	}

	if (args[0] == ':') {
		cmd_ptr = cmd(args);
		if (cmd_valid(cmd_ptr) < 0) {
			err = CMD_ERR_WHATIS_NOT_FOUND;
			return -1;
		}

		id = cmd_ptr->id;
		desc = cmd_ptr->desc;
	} else {
		op_ptr = op(args);
		if (op_valid(op_ptr) < 0) {
			err = CMD_ERR_WHATIS_NOT_FOUND;
			return -1;
		}
		
		id = op_ptr->id;
		desc = op_ptr->desc;
	}

	printf("%s: %s\n", id, desc);

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

int
cmd_valid(const CmdReg *ptr)
{
	if (ptr->func == NULL)
		return -1;

	return 0;
}
