/* modificato da Ale
*/

#include "loop.h"
//#include "matrix_idx.h"

#include <stdlib.h>
#include "conv_winograd_2x3.h"

//dimensione tile output
#define N 2
//dimensione filtro
#define R 3
//dimensione tile input
#define M 4

//I calcoli sono esplicitati: il procedimento di Winograd è più efficiente in questa forma
//che non come prodotto di matrici di pre- e post-addizioni

static inline void transformFilter(matrix* kernel, matrix* dest) {
	//real (*G)[3] = (real(*)[R])kernel->data;
	real **G = kernel->data;
//	real (*U)[4] = (real(*)[M])dest->data;
	real **U = dest->data;
	real mid[4][3];
	int i;
	FOR(i,3) { //i=col
		mid[0][i] = G[0][i];
		mid[1][i] = (G[0][i]+G[1][i]+G[2][i])/2;
		mid[2][i] = (G[0][i]-G[1][i]+G[2][i])/2;
		mid[3][i] = G[2][i];
	}
	FOR(i,4) { //i=row
		U[i][0] = mid[i][0];
		U[i][1] = (mid[i][0]+mid[i][1]+mid[i][2])/2;
		U[i][2] = (mid[i][0]-mid[i][1]+mid[i][2])/2;
		U[i][3] = mid[i][2];
	}
}



static inline void transformInputTile(matrix* src, matrix* dest, int off_x, int off_y, int *nsum) {

#define IDX(MATRIX_PTR,I,J) MATRIX_PTR->data[I][J]
#define IDX_SAFE(MATRIX_PTR,I,J) (((I<0) || (I>=((MATRIX_PTR)->h)) || (J<0) || (J>=((MATRIX_PTR)->w)))?0:IDX(MATRIX_PTR,I,J))
#define IDX_OFFSET_SAFE(MATRIX_PTR,OFF_H,OFF_W,I,J) IDX_SAFE(MATRIX_PTR,OFF_H+I,OFF_W+J)
	/*
	 * se indice è dentro i limiti della matrice ritorna dati[r+off_x][c+off_y]
	 * else ritorna 0
	 */
#define d(r,c) IDX_OFFSET_SAFE(src,off_x,off_y,r,c)

	real **v = dest->data;
#define v(r,c) v[r][c]
	real d[4][4] = {
			{d(0,0),d(0,1),d(0,2),d(0,3)},
			{d(1,0),d(1,1),d(1,2),d(1,3)},
			{d(2,0),d(2,1),d(2,2),d(2,3)},
			{d(3,0),d(3,1),d(3,2),d(3,3)}
	};
	v(0,0) = d[0][0]-d[2][0]-d[0][2]+d[2][2];
	v(0,1) = d[0][1]-d[2][1]+d[0][2]-d[2][2];
	v(0,2) = d[0][2]-d[2][2]-d[0][1]+d[2][1];
	v(0,3) = d[0][1]-d[2][1]-d[0][3]+d[2][3];
	v(1,0) = d[1][0]+d[2][0]-d[1][2]-d[2][2];
	v(1,1) = d[1][1]+d[2][1]+d[1][2]+d[2][2];
	v(1,2) = d[1][2]+d[2][2]-d[1][1]-d[2][1];
	v(1,3) = d[1][1]+d[2][1]-d[1][3]-d[2][3];
	v(2,0) = d[2][0]-d[1][0]-d[2][2]+d[1][2];
	v(2,1) = d[2][1]-d[1][1]+d[2][2]-d[1][2];
	v(2,2) = d[2][2]-d[1][2]-d[2][1]+d[1][1];
	v(2,3) = d[2][1]-d[1][1]-d[2][3]+d[1][3];
	v(3,0) = d[1][0]-d[3][0]-d[1][2]+d[3][2];
	v(3,1) = d[1][1]-d[3][1]+d[1][2]-d[3][2];
	v(3,2) = d[1][2]-d[3][2]-d[1][1]+d[3][1];
	v(3,3) = d[1][1]-d[3][1]-d[1][3]+d[3][3];

	(*nsum)+=3*16;
#undef d
#undef v
}


static inline void multiplyElementWise(matrix* src1, matrix* src2, matrix* dest, int *nmul) {
//	register int z;
//	FOR(z,M*M) {
//		dest->data[z] =  (src1->data[z]) * (src2->data[z]); //FIXED_MUL(src1->data[z],src2->data[z]) in origine;
//	}
	/*
	 * ogni elemento di dest è il prodotto
	 * esempio dest[0][0] = src1[0][0] * src2[0][0]
	 */
	int i;
	FOR(i,M){

		int j;
		FOR(j,M){
			dest->data[i][j] = src1->data[i][j] * src2->data[i][j];
		}
	}

	//dimpende da dimension kernel e da dimensione tile
	(*nmul)+=M*M;


}


static inline void antiTransformOutputTile(matrix* src, matrix* dest, int off_x, int off_y, int *nsum) {
//#define s(r,c) IDX_OFFSET_SAFE_WRITE(dest,off_x,off_y,r,c)
#define IDX_INSIDE(MATRIX_PTR,I,J) ((I>=0) && (I<((MATRIX_PTR)->h)) && (J>=0) && (J<((MATRIX_PTR)->w)))
#define IDX_INSIDE_OFF(MATRIX_PTR,OFF_H,OFF_W,I,J) IDX_INSIDE(MATRIX_PTR,OFF_H+I,OFF_W+J)
#define IDX_OFFSET(MATRIX_PTR,OFF_H,OFF_W,I,J) IDX(MATRIX_PTR,OFF_H+I,OFF_W+J)
#define IDX_OFFSET_SAFE_WRITE(MATRIX_PTR,OFF_H,OFF_W,I,J) if(IDX_INSIDE_OFF(MATRIX_PTR,OFF_H,OFF_W,I,J)) IDX_OFFSET(MATRIX_PTR,OFF_H,OFF_W,I,J)
#define s(r,c) IDX_OFFSET_SAFE_WRITE(dest,off_x,off_y,r,c)

real **w = src->data;
#define w(r,c) w[r][c]
	s(0,0) = w(0,0)+w(1,0)+w(2,0)
			+w(0,1)+w(1,1)+w(2,1)
			+w(0,2)+w(1,2)+w(2,2);
	s(0,1) = w(0,1)+w(1,1)+w(2,1)
			-w(0,2)-w(1,2)-w(2,2)
			-w(0,3)-w(1,3)-w(2,3);
	s(1,0) = w(1,0)-w(2,0)-w(3,0)
			+w(1,1)-w(2,1)-w(3,1)
			+w(1,2)-w(2,2)-w(3,2);
	s(1,1) = w(1,1)-w(2,1)-w(3,1)
			-w(1,2)+w(2,2)+w(3,2)
			-w(1,3)+w(2,3)+w(3,3);

	(*nsum)+=4*8;

#undef w
#undef s
}

/*
* N = 2 tile dimension
* 3 filtro 3x3
* dimensione out: stessa di in
* PRE: out di qualsiasi dimensione, preferibilemente uguali o vicine a matrice di in
* PRE: IL FILTRO DEEVE ESSERE 3x3 PER FORZA!!! dipende dai parametri definiti dalla define all inizio,
* ma non basterebbe purtroppo... transform filter sono ritagliate per questi n e r
* mi basti sapere che se inserisco un filtro 3x3 funziona tutto... unica restrizione... matrice in puo essere di dimensione
* che voglio come anche out
*/
void conv_winograd_2x3(matrix* in, matrix* filter, matrix* out, int *nsum, int *nmul) {
	register int tr,tc;
	/*
	 * tr indice riga di colonna partenza tile
	 * tc indice colonna
	 */

	/*
	 * 3 matrici temporanee che vanno deallocate
	 * U trasformata kernel
	 * V trasformata tile input
	 * W trasformata tile out che verra antitrasformata
	 */
	matrix *U=allocSquareMatrix(M);
	matrix *V=allocSquareMatrix(M);
	matrix *W=allocSquareMatrix(M);

//	ALLOC_MATRIX(U,M,M);
//	ALLOC_MATRIX(V,M,M);
//	ALLOC_MATRIX(W,M,M);
	/*
	 * M = 4 in questo caso.. dipende da N e R
	 * N dimensione tile
	 * R dimensione filtro
	 * N = 2 e R = 3... se non tocco le funziona tutto
	 */

	//per convolvere più immagini con lo stesso set di filtri,
	//questa operazione andrebbe fatta separatamente
	//per semplicità di testing, metto tutto nella stessa funzione
	transformFilter(filter,U);


	FOR_START_STEP(tr,-1,in->h,N) {//per tutti i tile di una riga
		FOR_START_STEP(tc,-1,in->w,N) {// ...colonne
			transformInputTile(in,V,tr,tc,nsum);
			multiplyElementWise(U,V,W,nmul);
			antiTransformOutputTile(W,out,tr,tc,nsum);
		}
	}


	freeMatrix(U);freeMatrix(V);freeMatrix(W);
//	FREE_MATRIX(U);
//	FREE_MATRIX(V);
//	FREE_MATRIX(W);
}


matrix* conv_winograd_2x3_bis(matrix* in, matrix* filter, int *nsum, int *nmul) {

	matrix *out = allocMatrix(in->h,in->w);

	register int tr,tc;
	/*
	 * tr indice riga di colonna partenza tile
	 * tc indice colonna
	 */

	/*
	 * 3 matrici temporanee che vanno deallocate
	 * U trasformata kernel
	 * V trasformata tile input
	 * W trasformata tile out che verra antitrasformata
	 */
	matrix *U=allocSquareMatrix(M);
	matrix *V=allocSquareMatrix(M);
	matrix *W=allocSquareMatrix(M);

//	ALLOC_MATRIX(U,M,M);
//	ALLOC_MATRIX(V,M,M);
//	ALLOC_MATRIX(W,M,M);
	/*
	 * M = 4 in questo caso.. dipende da N e R
	 * N dimensione tile
	 * R dimensione filtro
	 * N = 2 e R = 3... se non tocco le funziona tutto
	 */

	//per convolvere più immagini con lo stesso set di filtri,
	//questa operazione andrebbe fatta separatamente
	//per semplicità di testing, metto tutto nella stessa funzione
	transformFilter(filter,U);

	//debug
//	puts("****** debug trasform filter *****");
//	printMatrix(U);
//	puts("***********************************");


	FOR_START_STEP(tr,-1,out->h - 1,N) {//per tutti i tile di una riga
		FOR_START_STEP(tc,-1,out->w - 1,N) {// ...colonne
//	FOR_START_STEP(tr,-1,in->h,N) {//per tutti i tile di una riga
//			FOR_START_STEP(tc,-1,in->w,N) {// ...colonne
			transformInputTile(in,V,tr,tc,nsum);
			multiplyElementWise(U,V,W,nmul);
			antiTransformOutputTile(W,out,tr+1,tc+1,nsum);
		}
	}


	freeMatrix(U);freeMatrix(V);freeMatrix(W);
//	FREE_MATRIX(U);
//	FREE_MATRIX(V);
//	FREE_MATRIX(W);

	return out;
}
