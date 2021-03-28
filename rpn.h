/* See LICENSE file for copyright and license details. */

#define RPN_EXPR_SIZE 32
#define RPN_STACK_SIZE 32

typedef struct {
	int sp;
	double elems[RPN_STACK_SIZE];
} RPNStack;

enum {
	RPN_SUCCESS, /* Breaking alphabetical order so that it gets = 0 */
	RPN_ERR_OP_INV,
	RPN_ERR_OP_UNDEF,
	RPN_ERR_STACK_MAX,
	RPN_ERR_STACK_MIN
};

RPNStack *rpn_stack_init(RPNStack *stack);
int rpn_stack_drop(RPNStack *stack);
int rpn_stack_dup(RPNStack *stack);
int rpn_stack_peek(double *dest, RPNStack stack);
int rpn_stack_swap(RPNStack *stack);
int rpn_calc(double *dest, const char *expr, RPNStack *stack);
const char *rpn_strerr(int rpnerr);
