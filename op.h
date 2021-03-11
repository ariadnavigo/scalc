/* See LICENSE file for copyright and license details. */

#define OP_NAME_SIZE 16

struct op_reg {
	char id[OP_NAME_SIZE];
	int argn;
	union {
		float (*n1)(float);
		float (*n2)(float, float);
	} func;
};

struct op_reg *op(const char *oper);
