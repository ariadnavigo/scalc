/* See LICENSE for copyright and license details. */

#include <errno.h>
#include <stdio.h>
#include <string.h>

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
	case CMD_ERR_FEW_ARGS:
		return "too few arguments passed.";
	case CMD_ERR_FILE_IO:
		return strerror(errno);
	case CMD_ERR_INVALID:
		return "invalid command.";
	case MEM_ERR_NOT_FOUND:
		return "bad register.";
	case MEM_ERR_REG_ARG:
		return "register required.";
	case OP_ERR_INVALID:
		return "undefined operation.";
	case STACK_ERR_MAX:
		return "too many elements stored in stack.";
	case STACK_ERR_MIN:
		return "too few elements in stack.";
	default:
		return "success.";
	}
}
