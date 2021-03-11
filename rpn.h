/* See LICENSE file for copyright and license details. */

#define RPN_EXPR_SIZE 32

enum {
	RPN_SUCCESS,
	RPN_ERR_STACK_MAX,
	RPN_ERR_STACK_MIN,
	RPN_ERR_NAN
};

int rpn_calc(float *dest, const char *expr);
const char *rpn_strerr(int rpnerr);
