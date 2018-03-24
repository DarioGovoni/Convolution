/*
 * matrix.h
 *
 *  Created on: 24/mar/2018
 *      Author: Dario
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include "real.h"

typedef struct {
	unsigned int h;
	unsigned int w;
	real* data;
} matrix;



#endif /* MATRIX_H_ */
