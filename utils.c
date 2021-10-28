/* See LICENSE for copyright and license details. */

#include <stdio.h>

#include "config.h"
#include "utils.h"

int err = NO_ERR;

void
print_num(double num)
{
	printf("%." SCALC_PREC "f\n", num);
}

const char *
errmsg(void)
{
	switch (err) {
	case CMD_ERR_INVALID:
		return "invalid command.";
	case MEM_ERR_NOT_FOUND:
		return "bad register.";
	case MEM_ERR_REG_ARG:
		return "register required.";
	case OP_ERR_INVALID:
		return "undefined operation.";
	case STK_ERR_STACK_MAX:
		return "too many elements stored in stack.";
	case STK_ERR_STACK_MIN:
		return "too few elements in stack.";
	default:
		return "success.";
	}
}
