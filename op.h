/* See LICENSE file for copyright and license details. */

#define OP_NAME_SIZE 16

typedef struct {
	char id[OP_NAME_SIZE];
	int arg_n;
	union {
		double (*n0)(void);
		double (*n1)(double);
		double (*n2)(double, double);
	} func;
} OpReg;

const OpReg *op(const char *oper);

extern const OpReg op_defs[];
