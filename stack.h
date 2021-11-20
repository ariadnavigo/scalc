/* See LICENSE file for copyright and license details. */

#define STACK_SIZE 32

typedef struct {
	int sp;
	double elems[STACK_SIZE];
} Stack;

int stack_init(Stack *stack);
int stack_push(Stack *stack, double elem);
int stack_pop(double *dest, Stack *stack);
int stack_drop(Stack *stack);
int stack_dup(Stack *stack);
int stack_peek(double *dest, int i, Stack stack);
int stack_swap(Stack *stack);
