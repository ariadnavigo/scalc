/* See LICENSE file for copyright and license details. */

#define OP_NAME_SIZE 16

struct op_reg {
	char id[OP_NAME_SIZE];
	int argn;
	union {
		double (*n1)(double);
		double (*n2)(double, double);
	} func;
};

const struct op_reg *op(const char *oper);
