/* See LICENSE file for copyright and license details. */

#define STK_STACK_SIZE 32

typedef struct {
	int sp;
	double elems[STK_STACK_SIZE];
} Stack;

int stack_init(Stack *stack);
int stack_push(Stack *stack, double elem);
int stack_pop(double *dest, Stack *stack);
int stack_drop(Stack *stack);
int stack_dup(Stack *stack);
int stack_peek(double *dest, Stack stack);
int stack_swap(Stack *stack);
const char *stack_errmsg(void);
