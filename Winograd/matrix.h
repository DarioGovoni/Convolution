#ifndef T_MATRIX
#define T_MATRIX

#include "fixed.h"

typedef fixed real;
typedef struct {
	unsigned int w;
	unsigned int h;
	real* data;
} matrix;

void printMatrix(matrix*m);
void printMatrix_Side(matrix*m1, matrix*m2);

#define ALLOC_MATRIX(PTR,H,W) (PTR)=malloc(sizeof(matrix));(PTR)->h=H;(PTR)->w=W;(PTR)->data=malloc(sizeof(real)*(H)*(W))
#define FREE_MATRIX(PTR) free(PTR->data); free(PTR)
#define REALLOC_MATRIX(PTR,H,W) PTR->data=realloc(PTR->data,sizeof(real)*(H)*(W))

#endif
