/* See LICENSE file for copyright and license details. */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "op.h"
#include "stack.h"

#define SCALC_CMD_SIZE 32

enum {
	SCALC_NOP,
	SCALC_DROP,
	SCALC_DROP_ALL,
	SCALC_DUP,
	SCALC_EXIT,
	SCALC_LIST,
	SCALC_PEEK,
	SCALC_SWAP
};

struct cmd_reg {
	char id[SCALC_CMD_SIZE];
	int reply;
};

static void die(const char *fmt, ...);
static void scalc_output(double res, const char *expr, int stack_err);
static void scalc_list_ops(void);
static int scalc_cmd(const char *cmd);
static void scalc_ui(FILE *fp);

static struct cmd_reg cmd_dfs[] = {
	{ .id = "d", .reply = SCALC_DROP },
	{ .id = "D", .reply = SCALC_DROP_ALL },
	{ .id = "dup", .reply = SCALC_DUP },
	{ .id = "list", .reply = SCALC_LIST },
	{ .id = "p", .reply = SCALC_PEEK },
	{ .id = "q", .reply = SCALC_EXIT },
	{ .id = "swp", .reply = SCALC_SWAP },
	{ .id = "", .reply = SCALC_NOP } /* Dummy "terminator" */
};

static void
die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);

	va_end(ap);

	exit(1);
}

static void
scalc_output(double res, const char *expr, int stack_err)
{
	if (stack_err != STK_SUCCESS)
		fprintf(stderr, "%s: %s\n", expr, stack_strerr(stack_err));
	else
		printf("%." SCALC_PREC "f\n", res);
}

static void
scalc_list_ops(void)
{
	const OpReg *ptr;

	for (ptr = op_defs; strncmp(ptr->id, "", OP_NAME_SIZE) != 0; ++ptr)
		printf("%s ", ptr->id);
	putchar('\n');
}

static int
scalc_cmd(const char *cmd)
{
	struct cmd_reg *ptr;

	/* All scalc commands shall start with ':' */
	if (cmd[0] != ':')
		return SCALC_NOP;

	++cmd; /* Skip leading ':' */
	for (ptr = cmd_dfs; strncmp(ptr->id, "", SCALC_CMD_SIZE) != 0; ++ptr) {
		if (strncmp(cmd, ptr->id, SCALC_CMD_SIZE) == 0)
			return ptr->reply;
	}

	return SCALC_NOP;
}

static void
scalc_ui(FILE *fp)
{
	Stack stack;
	char expr[STK_EXPR_SIZE];
	int prompt_mode, output, scalc_err, stack_err;
	double res;

	prompt_mode = isatty(fileno(fp));

	stack_init(&stack);
	while (feof(fp) == 0) {
		output = 0; /* We assume output is not needed */

		if (prompt_mode > 0) {
			printf(scalc_prompt);
			fflush(stdout);
		}

		if (fgets(expr, STK_EXPR_SIZE, fp) == NULL)
			break;

		if (expr[strlen(expr) - 1] == '\n')
			expr[strlen(expr) - 1] = '\0';

		if (strlen(expr) == 0)
			continue;

		scalc_err = scalc_cmd(expr);
		switch (scalc_err) {
		case SCALC_DROP:
			stack_err = stack_drop(&stack);
			if (stack_err != STK_SUCCESS)
				output = 1;
			break;
		case SCALC_DROP_ALL:
			stack_init(&stack);
			break;
		case SCALC_DUP:
			stack_err = stack_dup(&stack);
			if (stack_err != STK_SUCCESS)
				output = 1;
			break;
		case SCALC_EXIT:
			return;
		case SCALC_LIST:
			scalc_list_ops();
			break;
		case SCALC_PEEK:
			stack_err = stack_peek(&res, stack);
			output = 1;
			break;
		case SCALC_SWAP:
			stack_err = stack_swap(&stack);
			if (stack_err != STK_SUCCESS)
				output = 1;
			break;
		default:
			stack_err = stack_calc(&res, expr, &stack);
			output = 1;
			break;
		}

		if ((prompt_mode > 0) && (output > 0))
			scalc_output(res, expr, stack_err);

		if ((prompt_mode == 0) && (stack_err != STK_SUCCESS))
			break;
	}

	if (prompt_mode == 0)
		scalc_output(res, expr, stack_err);
}

int
main(int argc, char *argv[])
{
	FILE *fp;

	if (argc < 2) {
		fp = stdin;
	} else {
		if ((fp = fopen(argv[1], "r")) == NULL)
			die("Error reading %s: %s", argv[1], strerror(errno));
	}

	scalc_ui(fp);

	if (fp != stdin)
		fclose(fp);

	return 0;
}
