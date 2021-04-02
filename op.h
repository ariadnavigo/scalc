/* See LICENSE file for copyright and license details. */

#define OP_NAME_SIZE 16

typedef struct {
	char id[OP_NAME_SIZE];
	int argn;
	union {
		double (*n1)(double);
		double (*n2)(double, double);
	} func;
} OpReg;

const OpReg *op(const char *oper);
