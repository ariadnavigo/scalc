/* See LICENSE file for copyright and license details. */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "mem.h"
#include "op.h"
#include "sline.h"
#include "stack.h"
#include "cmd.h" /* Depends on stack.h */
#include "strlcpy.h"
#include "utils.h"

#define SCALC_EXPR_SIZE 64

static void die(const char *fmt, ...);
static void usage(void);
static void cleanup(void);
static void prompt_input(char *expr);

static void run_cmd(Stack *stack, const char *expr);
static int apply_op(double *dx, const OpReg *op_ptr, Stack *stack);
static void eval_math(const char *expr, Stack *stack);

static FILE *fp;

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
usage(void)
{
	die("usage: scalc [-v] [file]");
}

static void
cleanup(void)
{
	sline_end();

	if (fp != stdin && fp != NULL)
		fclose(fp);
}

static void
prompt_input(char *expr)
{
	int sline_stat;

	printf(scalc_prompt);
	fflush(stdout);

	sline_stat = sline(expr, SCALC_EXPR_SIZE);

	if (sline_stat < 0)
		die("sline: %s", sline_errmsg());
}

static void
run_cmd(Stack *stack, const char *expr)
{
	int err;
	double mem_val;
	char expr_cpy[SCALC_EXPR_SIZE];
	char *expr_ptr;
	const CmdReg *cmd_ptr;

	err = 0;
	strlcpy(expr_cpy, expr, SCALC_EXPR_SIZE);
	expr_ptr = strtok(expr_cpy, " ");
	cmd_ptr = cmd(expr_ptr);

	switch (cmd_ptr->type) {
	case CMD_CMD:
		err = (*cmd_ptr->func.cmd)();
		break;
	case CMD_MEM:
		if ((expr_ptr = strtok(NULL, " ")) == NULL) {
			fprintf(stderr, "%s: register required.\n", expr);
			return;
		}

		if((err = stack_peek(&mem_val, *stack)) < 0)
			break;

		if ((*cmd_ptr->func.mem)(expr_ptr[0], mem_val) < 0) {
			fprintf(stderr, "%s: bad register.\n", expr);
			return;
		}

		break;
	case CMD_STK:
		err = (*cmd_ptr->func.stk)(stack);
		break;
	default:
		fprintf(stderr, "%s: invalid command.\n", expr);
		return;
	}

	if (err < 0)
		fprintf(stderr, "%s: %s\n", expr, stack_errmsg());

}

static int
apply_op(double *dx, const OpReg *op_ptr, Stack *stack)
{
	int arg_i, arg_n;
	double args[2];

	switch (op_ptr->type) {
	case OP_ARG2:
		arg_n = 2;
		break;
	case OP_ARG1:
		arg_n = 1;
		break;
	case OP_ARG0:
		arg_n = 0;
		break;
	default:
		return -1;
	}

	/* Traversing backwards because we're poping off the stack */
	for (arg_i = arg_n - 1; arg_i >= 0; --arg_i) {
		if (stack_pop(&args[arg_i], stack) < 0)
			return -1;
	}

	switch (op_ptr->type) {
	case OP_ARG2:
		*dx = (*op_ptr->func.n2)(args[0], args[1]);
		return 0;
	case OP_ARG1:
		*dx = (*op_ptr->func.n1)(args[0]);
		return 0;
	case OP_ARG0:
		*dx = (*op_ptr->func.n0)();
		return 0;
	default:
		return -1;
	}
}

static void
eval_math(const char *expr, Stack *stack)
{
	double dest, dx;
	char expr_cpy[SCALC_EXPR_SIZE];
	char *ptr, *endptr;
	const OpReg *op_ptr;

	/* We need to operate on a copy, as strtok is destructive. */
	strlcpy(expr_cpy, expr, SCALC_EXPR_SIZE);
	ptr = strtok(expr_cpy, " ");
	while (ptr != NULL) {
		dx = strtof(ptr, &endptr);
		if (endptr[0] == '\0')
			goto pushnum; /* If number, skip further parsing */

		if (mem_get(&dx, ptr[0]) == 0)
			goto pushnum;

		op_ptr = op(ptr);
		if (op_ptr->type == OP_NULL) {
			fprintf(stderr, "%s: undefined operation.\n", ptr);
			return;
		}

		if (apply_op(&dx, op_ptr, stack) < 0) {
			fprintf(stderr, "%s: %s\n", ptr, stack_errmsg());
			return;
		}

pushnum:
		if (stack_push(stack, dx) < 0) {
			fprintf(stderr, "%s: %s\n", expr, stack_errmsg());
			return;
		}
		ptr = strtok(NULL, " ");
	}

	if (stack_peek(&dest, *stack) < 0) {
		fprintf(stderr, "%s: %s\n", expr, stack_errmsg());
		return;
	}

	print_num(dest);
}

int
main(int argc, char *argv[])
{
	Stack stack;
	char *filearg;
	char expr[SCALC_EXPR_SIZE];
	int opt, prompt_mode;

	while ((opt = getopt(argc, argv, ":v")) != -1) {
		switch (opt) {
		case 'v':
			die("scalc %s", VERSION);
			break; /* UNREACHABLE */
		default:
			usage(); /* die()'s */
		}
	}

	atexit(cleanup);

	if (optind < argc)
		filearg = argv[optind];
	else 
		filearg = NULL;

	if (filearg != NULL) {
		if ((fp = fopen(filearg, "r")) == NULL)
			die("Could not open %s: %s", filearg, strerror(errno));
	} else {
		fp = stdin;
	}

	if ((prompt_mode = isatty(fileno(fp))) > 0) {
		if (sline_setup() < 0)
			die("Terminal error: %s", sline_errmsg());
	}

	stack_init(&stack);
	while (feof(fp) == 0) {
		if (prompt_mode > 0) {
			prompt_input(expr);
		} else {
			if (fgets(expr, SCALC_EXPR_SIZE, fp) == NULL)
				break;
		}

		if (expr[strlen(expr) - 1] == '\n')
			expr[strlen(expr) - 1] = '\0';

		if (strlen(expr) == 0)
			continue;

		if (strncmp(expr, ":quit", SCALC_EXPR_SIZE) == 0)
			return 0;
		else if (expr[0] == ':')
			run_cmd(&stack, expr);
		else
			eval_math(expr, &stack);
	}

	return 0;
}
