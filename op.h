/* See LICENSE file for copyright and license details. */

#define OP_NAME_SIZE 16

struct op_reg {
	char id[OP_NAME_SIZE];
	int argn;
	union {
		float (*op1n)(float);
		float (*op2n)(float, float);
	} op_func;
};

struct op_reg *op(const char *oper);
