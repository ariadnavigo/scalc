/* See LICENSE file for copyright and license details. */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "rpn.h"

#define SCALC_CMD_SIZE 32

enum {
	SCALC_NOP,
	SCALC_DROP,
	SCALC_DROP_ALL,
	SCALC_EXIT,
	SCALC_PEEK
};

struct cmd_reg {
	char id[SCALC_CMD_SIZE];
	int reply;
};

static void die(const char *fmt, ...);
static void scalc_output(float res, const char *expr, int rpnerr);
static int scalc_cmd(const char *cmd);

static struct cmd_reg cmd_defs[] = {
	{ .id = "d", .reply = SCALC_DROP },
	{ .id = "D", .reply = SCALC_DROP_ALL },
	{ .id = "p", .reply = SCALC_PEEK },
	{ .id = "q", .reply = SCALC_EXIT },
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
scalc_output(float res, const char *expr, int rpnerr)
{
	if (rpnerr != RPN_SUCCESS)
		fprintf(stderr, "%s: %s\n", expr, rpn_strerr(rpnerr));
	else
		printf("%f\n", res);
}

static int
scalc_cmd(const char *cmd)
{
	struct cmd_reg *ptr;

	/* All scalc commands shall start with ':' */
	if (cmd[0] != ':')
		return SCALC_NOP;

	++cmd; /* Skip leading ':' */
	for (ptr = cmd_defs; strcmp(ptr->id, "") != 0; ++ptr) {
		if (strcmp(cmd, ptr->id) == 0)
			return ptr->reply;
	}

	return SCALC_NOP;
}

void
ui_start(FILE *fp)
{
	RPNStack stack;
	char expr[RPN_EXPR_SIZE];
	int prompt_mode, output, scalc_err, rpn_err;
	float res;

	prompt_mode = isatty(fileno(fp));

	rpn_stack_init(&stack);
	while (feof(fp) == 0) {
		output = 0; /* We assume output is not needed */

		if (prompt_mode > 0)
			printf(scalc_prompt);
		if (fgets(expr, RPN_EXPR_SIZE, fp) == NULL)
			break;

		if (expr[strlen(expr) - 1] == '\n')
			expr[strlen(expr) - 1] = '\0';

		if (strlen(expr) == 0)
			continue;

		scalc_err = scalc_cmd(expr);
		switch (scalc_err) {
		case SCALC_DROP:
			if (rpn_stack_drop(&stack) < 0) {
				rpn_err = RPN_ERR_STACK_MIN;
				output = 1;
			}
			break;
		case SCALC_DROP_ALL:
			rpn_stack_init(&stack);
			break;
		case SCALC_EXIT:
			return;
		case SCALC_PEEK:
			if (rpn_stack_peek(&res, stack) < 0)
				rpn_err = RPN_ERR_STACK_MIN;
			output = 1;
			break;
		default:
			rpn_err = rpn_calc(&res, expr, &stack);
			output = 1;
			break;
		}

		if ((prompt_mode > 0) && (output > 0))
			scalc_output(res, expr, rpn_err);

		if ((prompt_mode == 0) && (rpn_err != RPN_SUCCESS))
			break;
	}

	if (prompt_mode == 0)
		scalc_output(res, expr, rpn_err);
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

	ui_start(fp);

	if (fp != stdin)
		fclose(fp);

	return 0;
}
