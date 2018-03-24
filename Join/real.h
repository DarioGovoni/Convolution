/*
 * real.h
 *
 *  Created on: 24/mar/2018
 *      Author: Dario, Alessio
 */

#ifndef REAL_H_
#define REAL_H_

typedef double real;
/*
 * utilizziamo REAL_MULT per rendere omogenei i casi
 * con i fixed point
 */
#define REAL_MULT(R1,R2) ((R1)*(R2))

void print_real(real r);

#endif /* REAL_H_ */
