/* See LICENSE file for copyright and license details. */

#include <errno.h>
#include <sline.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "stack.h" /* Dependency for cmd.h */
#include "cmd.h"
#include "config.h"
#include "mem.h"
#include "op.h"
#include "strlcpy.h"
#include "utils.h"

#define SCALC_EXPR_SIZE 64

static void die(const char *fmt, ...);
static void usage(void);
static void cleanup(void);
static void prompt_input(char *expr);

static void eval_cmd(const char *expr);
static int apply_op(double *dx, const OpReg *op_ptr);
static void eval_math(const char *expr);

static FILE *fp;
static int sline_mode;

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
	if (sline_mode > 0)
		sline_end();

	if (fp != stdin && fp != NULL)
		fclose(fp);
}

static int
file_input(char *expr, FILE *fp)
{
	static int line = 1;

	char trash;
	char *last_chr;

	if (fgets(expr, SCALC_EXPR_SIZE, fp) == NULL)
		return -1;

	last_chr = &expr[strlen(expr) - 1];

	/* Flushing stdin if there's more input; chomping '\n' if not. */
	if (*last_chr != '\n') {
		fprintf(stderr, "Warn: stdin: line %d truncated (too long).\n",
		        line);
		while ((trash = fgetc(stdin)) != '\n' && trash != EOF);
	} else {
		*last_chr = '\0';
	}

	++line;

	return 0;
}

static void
prompt_input(char *expr)
{
	int sline_stat;

	sline_stat = sline(expr, SCALC_EXPR_SIZE, NULL);

	if (sline_stat < 0)
		die("sline: %s", sline_errmsg());
}

static void
eval_cmd(const char *expr)
{
	char expr_cpy[SCALC_EXPR_SIZE];
	char *expr_ptr;
	const CmdReg *cmd_ptr;

	strlcpy(expr_cpy, expr, SCALC_EXPR_SIZE);
	expr_ptr = strtok(expr_cpy, " ");
	cmd_ptr = cmd(expr_ptr);
	if (strncmp(cmd_ptr->id, "", CMD_ID_SIZE) == 0)
		goto printerr;

	expr_ptr = strtok(NULL, " ");
	if ((*cmd_ptr->func)(expr_ptr) < 0)
		goto printerr;

	return;

printerr:
	fprintf(stderr, "%s: %s\n", expr, errmsg());
}

static int
apply_op(double *dx, const OpReg *op_ptr)
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
		if (stack_pop(&args[arg_i]) < 0)
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
eval_math(const char *expr)
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
		if (op_ptr->type == OP_NULL)
			goto printerr;

		if (apply_op(&dx, op_ptr) < 0)
			goto printerr;

pushnum:
		if (stack_push(dx) < 0)
			goto printerr;
		ptr = strtok(NULL, " ");
	}

	if (stack_peek(&dest, 0) < 0)
		goto printerr;

	print_num(dest);
	return;

printerr:
	fprintf(stderr, "%s: %s\n", ptr, errmsg());
}

int
main(int argc, char *argv[])
{
	char *filearg;
	char expr[SCALC_EXPR_SIZE];
	int opt;

	while ((opt = getopt(argc, argv, ":v")) != -1) {
		switch (opt) {
		case 'v':
			die("scalc %s (sline %s)", VERSION, sline_version());
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

	if (filearg == NULL)
		fp = stdin;
	else if ((fp = fopen(filearg, "r")) == NULL)
		die("Could not open %s: %s", filearg, strerror(errno));

	if ((sline_mode = isatty(fileno(fp))) > 0) {
		if (sline_setup(SCALC_EXPR_SIZE) < 0)
			die("Terminal error: %s", sline_errmsg());
	}

	stack_init();
	while (feof(fp) == 0) {
		err = NO_ERR; /* Reset err */
		if (sline_mode > 0)
			prompt_input(expr);
		else if (file_input(expr, fp) < 0)
			break;

		if (strlen(expr) == 0)
			continue;

		if (strncmp(expr, ":quit", SCALC_EXPR_SIZE) == 0)
			return 0;
		else if (expr[0] == ':')
			eval_cmd(expr);
		else
			eval_math(expr);
	}

	return 0;
}
