#include "matrix.h"
#include "loop.h"
#include "matrix_idx.h"
#include <stdlib.h>
#include <stdio.h>

void printMatrix(matrix*m) {
	register int r,c;
	FOR(r,m->h) {
		printf("[");
		printf("%f",FIXED_2_FLOAT(IDX(m,r,0)));
		FOR_1(c,m->w) {
			printf(", %f",FIXED_2_FLOAT(IDX(m,r,c)));
		}
		printf("]\n");
	}
}

void printMatrix_Side(matrix*m1, matrix*m2) {
	register int r,c;
	FOR(r,m1->h) {
		printf("[");
		printf("%f",FIXED_2_FLOAT(IDX(m1,r,0)));
		FOR_1(c,m1->w) {
			printf(", %f",FIXED_2_FLOAT(IDX(m1,r,c)));
		}
		printf("] [");
		printf("%f",FIXED_2_FLOAT(IDX(m2,r,0)));
		FOR_1(c,m2->w) {
			printf(", %f",FIXED_2_FLOAT(IDX(m2,r,c)));
		}
		printf("]\n");
	}
}
