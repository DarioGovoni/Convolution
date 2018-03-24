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

matrix * allocMatrix(int h, int w) {
	matrix *m = malloc ( sizeof(matrix) );
	m->h=h;
	m->w=w;
	m->data=(real*) malloc( (m->h)*(m->w)*sizeof(real) );
	return m;
}

matrix* allocSquareMatrix(int dim){
	return allocMatrix(dim,dim);
}

void freeMatrix(matrix *m){
	//dealloco prima il livello più profondo
	free(m->data);
	//poi la struttura intera
	free(m);
}

void initMatrix(matrix *m){
	int size = m->w * m->h;
	memset(m->data,0,size*sizeof(real));
}



void printMatrix(matrix *m){
	int r,c;
	for(r=0;r<(m->h);r++){ //rows
		printf("[ ");
		for(c=0;c<(m->w);c++){ //columns
			//printf("%d | ",  );
			//TODO
		}
		printf("]\n");
	}
	printf("\n");
}
