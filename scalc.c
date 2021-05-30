/* See LICENSE file for copyright and license details. */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "op.h"
#include "stack.h"
#include "strlcpy.h"

#define CMD_ID_SIZE 32

enum cmd_type {
	CMD_NULL,
	CMD_CMD0,
	CMD_STK0
};

typedef struct {
	char id[CMD_ID_SIZE];
	enum cmd_type type;
	union {
		int (*cmd0)(void);
		int (*stk0)(Stack *stk);
	} func;
} CmdReg;

static void die(const char *fmt, ...);
static void usage(void);

static void print_num(double num);
static int expr_is_cmd(const char *expr);
static void run_cmd(Stack *stack, const char *expr);
static int apply(double *dx, const OpReg *op_ptr, Stack *stack);
static void eval(const char *expr, Stack *stack);

static int cmd_print(Stack *stack);
static int cmd_list(void);
static int cmd_quit(void);

static const CmdReg cmd_defs[] = {
	{ ":d", CMD_STK0, { .stk0 = stack_drop } },
	{ ":D", CMD_STK0, { .stk0 = stack_init } },
	{ ":dup", CMD_STK0, { .stk0 = stack_dup } },
	{ ":list", CMD_CMD0, { .cmd0 = cmd_list } },
	{ ":p", CMD_STK0, { .stk0 = cmd_print } },
	{ ":swp", CMD_STK0, { .stk0 = stack_swap } },
	{ ":q", CMD_CMD0, { .cmd0 = cmd_quit } },
	{ "", CMD_NULL, { .cmd0 = NULL } }
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
usage(void)
{
	die("usage: scalc [-v]");
}

static void
print_num(double num)
{
	printf("%." SCALC_PREC "f\n", num);
}

static int
expr_is_cmd(const char *expr)
{
	return expr[0] == ':';
}

static void
run_cmd(Stack *stack, const char *expr)
{
	int err;
	const CmdReg *cmd_ptr;

	err = 0;

	for (cmd_ptr = cmd_defs;
	     strncmp(cmd_ptr->id, "", CMD_ID_SIZE) != 0;
	     ++cmd_ptr) {
		if (strncmp(cmd_ptr->id, expr, CMD_ID_SIZE) == 0)
			break;
	}

	switch (cmd_ptr->type) {
	case CMD_CMD0:
		err = (*cmd_ptr->func.cmd0)();
		break;
	case CMD_STK0:
		err = (*cmd_ptr->func.stk0)(stack);
		break;
	default:
		fprintf(stderr, "%s: invalid command.\n", expr);
		return;
	}

	if (err < 0)
		fprintf(stderr, "%s: %s\n", expr, stack_errmsg());

}

static int
apply(double *dx, const OpReg *op_ptr, Stack *stack)
{
	int arg_i;
	double args[2];

	/* Traversing backwards because we're poping off the stack */
	for (arg_i = op_ptr->argn - 1; arg_i >= 0; --arg_i) {
		if (stack_pop(&args[arg_i], stack) < 0)
			return -1;
	}

	switch (op_ptr->argn) {
	case 2:
		*dx = (*op_ptr->func.n2)(args[0], args[1]);
		return 0;
	case 1:
		*dx = (*op_ptr->func.n1)(args[0]);
		return 0;
	case 0:
		*dx = (*op_ptr->func.n0)();
		return 0;
	default:
		return -1;
	}
}

static void
eval(const char *expr, Stack *stack)
{
	double dest, dx;
	char expr_cpy[STK_EXPR_SIZE];
	char *ptr, *endptr;
	const OpReg *op_ptr;

	/* We need to operate on a copy, as strtok is destructive. */
	strlcpy(expr_cpy, expr, STK_EXPR_SIZE);
	ptr = strtok(expr_cpy, " ");
	while (ptr != NULL) {
		dx = strtof(ptr, &endptr);
		if (endptr[0] == '\0')
			goto pushnum; /* If number, skip further parsing */

		if ((op_ptr = op(ptr)) == NULL) {
			fprintf(stderr, "%s: undefined operation.\n", ptr);
			return;
		}
		
		if (apply(&dx, op_ptr, stack) < 0) {
			fprintf(stderr, "%s: %s\n", expr, stack_errmsg());
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
cmd_quit(void)
{
	exit(0);

	return 0; /* UNREACHABLE */
}

int
main(int argc, char *argv[])
{
	Stack stack;
	char expr[STK_EXPR_SIZE];
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

	prompt_mode = isatty(fileno(stdin));

	stack_init(&stack);
	while (feof(stdin) == 0) {
		if (prompt_mode > 0) {
			printf(scalc_prompt);
			fflush(stdout);
		}

		if (fgets(expr, STK_EXPR_SIZE, stdin) == NULL)
			break;

		if (expr[strlen(expr) - 1] == '\n')
			expr[strlen(expr) - 1] = '\0';

		if (strlen(expr) == 0)
			continue;

		if (expr_is_cmd(expr) > 0)
			run_cmd(&stack, expr); 
		else
			eval(expr, &stack);
	}

	return 0;
}
