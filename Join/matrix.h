/*
 * matrix.h
 *
 *  Created on: 24/mar/2018
 *      Author: Dario, Alessio
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include "real.h"

//l'intera struttura va allocata e gestita con metodi ad hoc
typedef struct {
	unsigned int h; //height (number of rows)
	unsigned int w; //width (number of columns)
	real* data;
} matrix;



/*
 * fondamentale: la struttura contiene un puntatore che va gestito a malloc e free,
 * non tentare mai di cambiare valori della struttura senza passare per una
 * funzione dedicata
 */
matrix* allocMatrix(int h, int w);

matrix* allocSquareMatrix(int dim);

void initMatrix(matrix *m);

void freeMatrix(matrix *m);

void printMatrix(matrix *m);


#endif /* MATRIX_H_ */
