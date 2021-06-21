/* See LICENSE file for copyright and license details. */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "mem.h"
#include "op.h"
#include "stack.h"
#include "strlcpy.h"

#define CMD_ID_SIZE 32

enum cmd_type {
	CMD_NULL,
	CMD_CMD,
	CMD_MEM,
	CMD_STK
};

typedef struct {
	char id[CMD_ID_SIZE];
	enum cmd_type type;
	union {
		int (*cmd)(void);
		int (*mem)(char var, double val);
		int (*stk)(Stack *stk);
	} func;
} CmdReg;

static void die(const char *fmt, ...);
static void usage(void);

static void print_num(double num);
static void run_cmd(Stack *stack, const char *expr);
static int apply_op(double *dx, const OpReg *op_ptr, Stack *stack);
static void eval_math(const char *expr, Stack *stack);

static int cmd_print(Stack *stack);
static int cmd_list(void);
static int cmd_quit(void);

static const CmdReg cmd_defs[] = {
	{ ":d", CMD_STK, { .stk = stack_drop } },
	{ ":D", CMD_STK, { .stk = stack_init } },
	{ ":dup", CMD_STK, { .stk = stack_dup } },
	{ ":list", CMD_CMD, { .cmd = cmd_list } },
	{ ":p", CMD_STK, { .stk = cmd_print } },
	{ ":sav", CMD_MEM, { .mem = mem_set } },
	{ ":swp", CMD_STK, { .stk = stack_swap } },
	{ ":q", CMD_CMD, { .cmd = cmd_quit } },
	{ "", CMD_NULL, { .cmd = NULL } }
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
	die("usage: scalc [-v] [file]");
}

static void
print_num(double num)
{
	printf("%." SCALC_PREC "f\n", num);
}

static void
run_cmd(Stack *stack, const char *expr)
{
	int err;
	double mem_val;
	char expr_cpy[STK_EXPR_SIZE];
	char *expr_ptr;
	const CmdReg *cmd_ptr;

	err = 0;

	strlcpy(expr_cpy, expr, STK_EXPR_SIZE);
	expr_ptr = strtok(expr_cpy, " ");
	for (cmd_ptr = cmd_defs; cmd_ptr->type != CMD_NULL; ++cmd_ptr) {
		if (strncmp(cmd_ptr->id, expr_ptr, CMD_ID_SIZE) == 0)
			break;
	}

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

		if (mem_get(&dx, ptr[0]) == 0)
			goto pushnum;

		if ((op_ptr = op(ptr)) == NULL) {
			fprintf(stderr, "%s: undefined operation.\n", ptr);
			return;
		}

		if (apply_op(&dx, op_ptr, stack) < 0) {
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
	FILE *fp;
	char *filearg;
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

	if (optind < argc)
		filearg = argv[optind];
	else 
		filearg = NULL;

	if (filearg != NULL) {
		if ((fp = fopen(filearg, "r")) == NULL)
			die("fileerr");
	} else {
		fp = stdin;
	}

	prompt_mode = isatty(fileno(fp));

	stack_init(&stack);
	while (feof(fp) == 0) {
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

		if (expr[0] == ':')
			run_cmd(&stack, expr);
		else
			eval_math(expr, &stack);
	}

	return 0;
}
