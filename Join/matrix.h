/*
 * matrix.h
 *
 *  Created on: 24/mar/2018
 *      Author: Dario
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include "real.h"

//l'intera struttura va allocata e gestita con metodi ad hoc
typedef struct {
	unsigned int h;
	unsigned int w;
	real* data;
} matrix;



#endif /* MATRIX_H_ */
