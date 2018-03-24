/*
 * real.h
 *
 *  Created on: 24/mar/2018
 *      Author: Dario
 */

#ifndef REAL_H_
#define REAL_H_

#include <stdint.h>
typedef int64_t fixed;
#define FRACT_BITS 32
#define HALF_FRACT_BITS 16
#define FIXED_ONE (1<<FRACT_BITS)
#define FIXED_HALF (1<<(FRACT_BITS-1))
#define INT_2_FIXED(x) (fixed)((x)<<FRACT_BITS)
#define FLOAT_2_FIXED(x) ((int)((x)*FIXED_ONE))
#define FIXED_2_INT(x) ((x)>>FRACT_BITS)
#define FIXED_2_FLOAT(x) (((double)(x)) / FIXED_ONE)
#define FIXED_MUL(x,y) (((x)>>HALF_FRACT_BITS)*((y)>>HALF_FRACT_BITS))

typedef fixed real;
#define REAL_MULT(R1,R2) FIXED_MUL(R1,R2)
void print_real(real r);

#endif /* REAL_H_ */
