/* See LICENSE for copyright and license details. */

enum {
	NO_ERR,
	CMD_ERR_INVALID,
	MEM_ERR_NOT_FOUND,
	MEM_ERR_REG_ARG,
	OP_ERR_INVALID,
	STACK_ERR_EMPTY,
	STACK_ERR_MAX,
	STACK_ERR_MIN
};

void print_num(double num);
const char *errmsg(void);

extern int err;
