/*
 * matrix.c
 *
 *  Created on: 24/mar/2018
 *      Author: Dario
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>//funzioni matematiche ==> modifica su properties con -lm
#include "matrix.h"

matrix * allocMatrix(int h, int w){
	matrix *m = malloc ( sizeof(matrix) );
	m->h=h;
	m->w=w;
	m->data=(real*) malloc( (m->h)*(m->w)*sizeof(real) );
	return m;
}

void freeMatrix(matrix *m){
	//dealloco prima il livello più profondo
	free(m->data);
	//poi la struttura intera
	free(m);
}


matrix* allocSquareMatrix(int dim){
	return allocMatrix(dim,dim);
}
