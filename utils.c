/* See LICENSE for copyright and license details. */

#include <stdio.h>

#include "config.h"

void
print_num(double num)
{
	printf("%." SCALC_PREC "f\n", num);
}
