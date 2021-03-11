/* See LICENSE file for copyright and license details. */

enum {
	RPN_SUCCESS,
	RPN_ERR_STACK_MAX,
	RPN_ERR_STACK_MIN,
	RPN_ERR_NAN
};

int rpn_calc(float *dest, char *expr);
const char *rpn_strerr(int rpnerr);
