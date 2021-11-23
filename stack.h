/* See LICENSE file for copyright and license details. */

#define STACK_SIZE 32

typedef struct {
	int sp;
	double elems[STACK_SIZE];
} Stack;

int stack_init(void);
int stack_push(double elem);
int stack_pop(double *dest);
int stack_drop(void);
int stack_dup(void);
int stack_peek(double *dest, int i);
int stack_swap(void);

extern Stack stack;
