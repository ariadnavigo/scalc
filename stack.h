/* See LICENSE file for copyright and license details. */

#define STK_EXPR_SIZE 32
#define STK_STACK_SIZE 32

typedef struct {
	int sp;
	double elems[STK_STACK_SIZE];
} Stack;

enum {
	STK_SUCCESS,
	STK_ERR_OP_INV,
	STK_ERR_OP_UNDEF,
	STK_ERR_STACK_MAX,
	STK_ERR_STACK_MIN
};

Stack *stack_init(Stack *stack);
Stack *stack_push(Stack *stack, double elem);
int stack_pop(double *dest, Stack *stack);
int stack_drop(Stack *stack);
int stack_dup(Stack *stack);
int stack_peek(double *dest, Stack stack);
int stack_swap(Stack *stack);
