/*
 * real.c
 *
 *  Created on: 24/mar/2018
 *      Author: Dario
 */
#include <stdio.h>
#include "real.h"

void print_real(real r) {
	double x = FIXED_2_FLOAT(r);
	printf("%f",x);
}
