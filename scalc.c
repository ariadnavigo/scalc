/* See LICENSE file for copyright and license details. */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rpn.h"

static void die(const char *fmt, ...);

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

int
main(int argc, char *argv[])
{
	FILE *fp;
	char expr[RPN_EXPR_SIZE];
	int rpnerr;
	float res;

	if (argc < 2) {
		fp = stdin;
	} else {
		if ((fp = fopen(argv[1], "r")) == NULL)
			die("Error reading %s: %s", argv[1], strerror(errno));
	}

	while (feof(fp) == 0) { 
		if (fgets(expr, RPN_EXPR_SIZE, fp) == NULL)
			break;

		if (expr[strlen(expr) - 1] == '\n')
			expr[strlen(expr) - 1] = '\0'; 

		if (strlen(expr) == 0)
			continue;

		if (fp != stdin)
			printf("%s\n", expr);

		if ((rpnerr = rpn_calc(&res, expr)) != RPN_SUCCESS)
			fprintf(stderr, "%s: %s\n", expr, rpn_strerr(rpnerr));
		else
			printf("%f\n", res);
	}

	if (fp != stdin)
		fclose(fp);

	return 0;
}
