/* See LICENSE for copyright and license details. */

enum {
	NO_ERR,
	CMD_ERR_INVALID,
	MEM_ERR_NOT_FOUND,
	MEM_ERR_REG_ARG,
	OP_ERR_INVALID,
	STK_ERR_STACK_MAX,
	STK_ERR_STACK_MIN
};

extern int err;

void print_num(double num);
const char *errmsg(void);
