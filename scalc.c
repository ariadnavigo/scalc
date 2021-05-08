/* See LICENSE file for copyright and license details. */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *argv0; /* Required here by arg.h */
#include "arg.h"
#include "config.h"
#include "op.h"
#include "stack.h"
#include "strlcpy.h"

static void die(const char *fmt, ...);
static void usage(void);

static const char *errmsg(int err);
static void list_ops(void);
static int eval(double *dest, const char *expr, Stack *stack);

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

static const char *
errmsg(int err)
{
	switch (err) {
	case STK_ERR_OP_INV:
		return "operation invalidly defined.";
	case STK_ERR_OP_UNDEF:
		return "operation not defined.";
	case STK_ERR_STACK_MAX:
		return "too many elements stored in stack.";
	case STK_ERR_STACK_MIN:
		return "too few elements in stack.";
	default:
		return "unknown error.";
	}
}

static void
list_ops(void)
{
	const OpReg *ptr;

	for (ptr = op_defs; strncmp(ptr->id, "", OP_NAME_SIZE) != 0; ++ptr)
		printf("%s ", ptr->id);
	putchar('\n');
}

static int
eval(double *dest, const char *expr, Stack *stack)
{
	int arg_i, err;
	double args[2];
	double dx;
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

		if ((op_ptr = op(ptr)) == NULL)
			return STK_ERR_OP_UNDEF;

		/* Traversing backwards because we're poping off the stack */
		for (arg_i = op_ptr->argn - 1; arg_i >= 0; --arg_i) {
			err = stack_pop(&args[arg_i], stack);
			if (err != STK_SUCCESS)
				return err;
		}

		if (op_ptr->argn == 2)
			dx = (*op_ptr->func.n2)(args[0], args[1]);
		else if (op_ptr->argn == 1)
			dx = (*op_ptr->func.n1)(args[0]);
		else if (op_ptr->argn == 0)
			dx = (*op_ptr->func.n0)();
		else
			return STK_ERR_OP_INV;

pushnum:
		if ((err = stack_push(stack, dx)) != STK_SUCCESS)
			return err;
		ptr = strtok(NULL, " ");
	}

	return stack_peek(dest, *stack);
}

int
main(int argc, char *argv[])
{
	Stack stack;
	char expr[STK_EXPR_SIZE];
	int prompt_mode, output, err;
	double res;

	ARGBEGIN {
	case 'v':
		die("scalc %s", VERSION);
		break; /* UNREACHABLE */
	default:
		usage(); /* die()'s */
	} ARGEND;

	prompt_mode = isatty(fileno(stdin));

	stack_init(&stack);
	while (feof(stdin) == 0) {
		err = STK_SUCCESS;
		output = 0; /* We assume no output is wanted */

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

		if (strncmp(expr, ":d", STK_EXPR_SIZE) == 0) {
			err = stack_drop(&stack);
		} else if (strncmp(expr, ":dup", STK_EXPR_SIZE) == 0) {
			err = stack_dup(&stack);
		} else if (strncmp(expr, ":D", STK_EXPR_SIZE) == 0) {
			stack_init(&stack);
		} else if (strncmp(expr, ":list", STK_EXPR_SIZE) == 0) {
			list_ops();
		} else if (strncmp(expr, ":p", STK_EXPR_SIZE) == 0) {
			err = stack_peek(&res, stack);
			output = 1;
		} else if (strncmp(expr, ":swp", STK_EXPR_SIZE) == 0) {
			err = stack_swap(&stack);
		} else if (strncmp(expr, ":q", STK_EXPR_SIZE) == 0) {
			return 0;
		} else {
			err = eval(&res, expr, &stack);
			output = 1;
		}

		if (err != STK_SUCCESS) {
			fprintf(stderr, "%s: %s\n", expr, errmsg(err));
			continue;
		}

		if (output > 0)
			printf("%." SCALC_PREC "f\n", res);
	}

	return 0;
}
