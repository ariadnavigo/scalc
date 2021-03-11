/* See LICENSE file for copyright and license details. */

#define RPN_EXPR_SIZE 32

enum {
	RPN_SUCCESS, /* Breaking alphabetical order so that it gets = 0 */
	RPN_ERR_OP_UNDEF,
	RPN_ERR_STACK_MAX,
	RPN_ERR_STACK_MIN
};

int rpn_calc(float *dest, const char *expr);
const char *rpn_strerr(int rpnerr);
