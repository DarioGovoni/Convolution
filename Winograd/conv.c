#include "conv.h"
#include "loop.h"
#include "matrix_idx.h"
#include "tensor_select.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

//clock_t begin, end;

//decidiamoci sulla nomenclatura:
//DERIVATI DAL BLAHUT
//d = (tile) input; g = filtro; s = (tile) output
//DERIVATI (anche) DAL PAPER LAVIN/GRAY
//U = filtro trasformato; V = tile input trasformato;
//NUOVO (per completare la triade)
//W = tile output trasformato

static inline void transformFilter(matrix* kernel, matrix* dest) {
	if (kernel->h!=R || kernel->w!=R) {
		printf("ERROR: Only %dx%d kernels supported./n",R,R);
		exit(3);
	}
	//REALLOC_MATRIX(dest,M,M);
	{
		fixed (*G)[R] = (fixed(*)[R])kernel->data;
		fixed (*U)[M] = (fixed(*)[M])dest->data;
//#define g(r,c) IDX(kernel,r,c)
//#define u(r,c) IDX(dest,r,c)
#define g(r,c) G[r][c]
#define u(r,c) U[r][c]
#include FILENAME_TF_KERN
#undef g
#undef u
	}
}

static inline void transformFilters(macroMatrix* kernels, macroMatrix* dest) {
	int bank,chan;

	if (kernels->h!=K || kernels->w!=C) {
		printf("ERROR: Only %dx%d filterbanks supported./n",K,C);
		exit(4);
	}
	//REALLOC_MMATRIX(dest,K,C);
	FOR(bank,K) {
		FOR(chan,C) {
			transformFilter(IDX(kernels,bank,chan),IDX(dest,bank,chan));
		}
	}
}

static inline void transformInputTile(matrix* src, matrix* dest, int off_x, int off_y) {
#define d(r,c) IDX_OFFSET_SAFE(src,off_x,off_y,r,c)
	fixed (*v)[M] = (fixed(*)[M])dest->data;
//#define v(r,c) IDX(dest,r,c)
#define v(r,c) v[r][c]
#include FILENAME_TF_INPUT
#undef d
#undef v
}

static inline void antiTransformOutputTile(matrix* src, matrix* dest, int off_x, int off_y) {
#define s(r,c) IDX_OFFSET_SAFE_WRITE(dest,off_x,off_y,r,c)
	fixed (*w)[M] = (fixed(*)[M])src->data;
#define w(r,c) w[r][c]
#include FILENAME_ATF_OUT
#undef w
#undef s
}

static inline void multiplyElementWise(matrix* src1, matrix* src2, matrix* dest) {
	register int z;
	FOR(z,M*M) {
		dest->data[z] = FIXED_MUL(src1->data[z],src2->data[z]);
	}
}
static inline void zeroMatrix(matrix* m) {
	register int i;
	FOR(i,M*M) {
		m->data[i] = 0;
	}
}
static inline void addElementWise(matrix* src1, matrix* src2, matrix* dest) {
	register int i;
	FOR(i,M*M) {
		dest->data[i] = src1->data[i]+src2->data[i];
	}
}
static inline void accElementWise(matrix* src, matrix* dest) {
	register int i;
	FOR(i,M*M) {
		dest->data[i] += src->data[i];
	}
}
static inline void subElementWise(matrix* src1, matrix* src2, matrix* dest) {
	register int i;
	FOR(i,M*M) {
		dest->data[i] = src1->data[i]-src2->data[i];
	}
}
static inline void naccElementWise(matrix* src, matrix* dest) {
	register int i;
	FOR(i,M*M) {
		dest->data[i] -= src->data[i];
	}
}
static inline void maccElementWise(matrix* src1, matrix* src2, matrix* dest) {
	register int z;
	FOR(z,M*M) {
		(dest->data[z]) += FIXED_MUL(src1->data[z],src2->data[z]);
	}
}
static inline void nmaccElementWise(matrix* src1, matrix* src2, matrix* dest) {
	register int z;
	FOR(z,M*M) {
		(dest->data[z]) -= FIXED_MUL(src1->data[z],src2->data[z]);
	}
}
static inline void copyElementWise(matrix* src, matrix* dst) {
	//memcpy(dst->data, src->data, M*M*sizeof(real));
	register int z;
	FOR(z,M*M) {
		(dst->data[z]) = (src->data[z]);
	}
}
static inline void multiplyMacroMatrix_naive(macroMatrix* src1, macroMatrix* src2, macroMatrix* dest) {
	register int l,m,n;
	FOR(l,K) {
		FOR(m,T) {
			//zeroMatrix(IDX(dest,i,j));
			multiplyElementWise(IDX(src1,l,0),IDX(src2,0,m),IDX(dest,l,m));
			FOR_1(n,C)
			//FOR(n,C)
			{
				maccElementWise(IDX(src1,l,n),IDX(src2,n,m),IDX(dest,l,m));
			}
		}
	}
}
static inline void selectQuadrant(int i, int j, macroMatrix*src, macroMatrix*dst) {
	IDX(dst,0,0) = IDX(src,0+i,0+j);
	IDX(dst,0,1) = IDX(src,0+i,1+j);
	IDX(dst,1,0) = IDX(src,1+i,0+j);
	IDX(dst,1,1) = IDX(src,1+i,1+j);
}
static inline void copyQuadrant(macroMatrix*src, macroMatrix*dest) {
	copyElementWise(IDX(src,0,0), IDX(dest,0,0));
	copyElementWise(IDX(src,0,1), IDX(dest,0,1));
	copyElementWise(IDX(src,1,0), IDX(dest,1,0));
	copyElementWise(IDX(src,1,1), IDX(dest,1,1));
}
/*static inline void putQuadrant(int i, int j, macroMatrix*src, macroMatrix*dst) {
	IDX(dst,0+i,0+j) = IDX(src,0,0);
	IDX(dst,0+i,1+j) = IDX(src,0,1);
	IDX(dst,1+i,0+j) = IDX(src,1,0);
	IDX(dst,1+i,1+j) = IDX(src,1,1);
}*/
static inline void addQuadrant(macroMatrix*src1, macroMatrix*src2, macroMatrix*dest) {
	addElementWise(IDX(src1,0,0),IDX(src2,0,0),IDX(dest,0,0));
	addElementWise(IDX(src1,0,1),IDX(src2,0,1),IDX(dest,0,1));
	addElementWise(IDX(src1,1,0),IDX(src2,1,0),IDX(dest,1,0));
	addElementWise(IDX(src1,1,1),IDX(src2,1,1),IDX(dest,1,1));
}
static inline void subQuadrant(macroMatrix*src1, macroMatrix*src2, macroMatrix*dest) {
	subElementWise(IDX(src1,0,0),IDX(src2,0,0),IDX(dest,0,0));
	subElementWise(IDX(src1,0,1),IDX(src2,0,1),IDX(dest,0,1));
	subElementWise(IDX(src1,1,0),IDX(src2,1,0),IDX(dest,1,0));
	subElementWise(IDX(src1,1,1),IDX(src2,1,1),IDX(dest,1,1));
}
static inline void mulQuadrant(macroMatrix*src1, macroMatrix*src2, macroMatrix*dest) {
	matrix *s, *t, *u;
	ALLOC_MATRIX(s,M,M);
	ALLOC_MATRIX(t,M,M);
	ALLOC_MATRIX(u,M,M);

	addElementWise(IDX(src1,1,0),IDX(src1,1,1),s);//S=A10+A11
	subElementWise(IDX(src2,0,1),IDX(src2,0,0),t);//T=B01-B00
	multiplyElementWise(s,t,u);//U = S*T
	copyElementWise(u, IDX(dest,1,1)); //C11=U
	copyElementWise(u, IDX(dest,0,1)); //C01=U
	multiplyElementWise(IDX(src1,0,0),IDX(src2,0,0),u); //U = A00*B00
	multiplyElementWise(IDX(src1,0,1),IDX(src2,1,0),IDX(dest,0,0));//C00 = A01*B10
	//addElementWise(u,IDX(dest,0,0),IDX(dest,0,0));//C00 = C00+U
	accElementWise(u,IDX(dest,0,0)); //C00 += U
	//subElementWise(s,IDX(src1,0,0),s);//S = S - A00
	naccElementWise(IDX(src1,0,0),s);//S -= A00
	//subElementWise(t,IDX(src2,1,1),t);//T = T - B11
	naccElementWise(IDX(src2,1,1),t);//T -= B11
	nmaccElementWise(s,t,u); //U -= S*T
	//addElementWise(u,IDX(dest,0,1),IDX(dest,0,1));//C01 = C01+U
	accElementWise(u,IDX(dest,0,1));//C01 += U
	//subElementWise(s,IDX(src1,0,1),s);//S = S-A01
	naccElementWise(IDX(src1,0,1),s);//S -= A01
	nmaccElementWise(s,IDX(src2,1,1),IDX(dest,0,1));//C01 -= S*B11
	//addElementWise(t,IDX(src2,1,0),t);//T = T+B10
	accElementWise(IDX(src2,1,0),t);//T += B10
	multiplyElementWise(IDX(src1,1,1),t,IDX(dest,1,0));//C10=A11*T
	subElementWise(IDX(src1,0,0),IDX(src1,1,0),s);//S = A00-A10
	subElementWise(IDX(src2,1,1),IDX(src2,0,1),t);//T = B11-B01
	maccElementWise(s,t,u); //U += S*T
	addElementWise(u,IDX(dest,1,1),IDX(dest,1,1));//C11 = U+C11
	addElementWise(u,IDX(dest,1,0),IDX(dest,1,0));//C10 = U+C10
}
static inline void multiplyMacroMatrix_strassen(macroMatrix* src1, macroMatrix* src2, macroMatrix* dest) {
	register int i,j;
	macroMatrix *s, *t, *u;
	macroMatrix *op1, *op2, *op3;
	macroMatrix *c00,*c01,*c10,*c11;
	ALLOC_MMATRIX(s,2,2);
	ALLOC_MMATRIX(t,2,2);
	ALLOC_MMATRIX(u,2,2);
	ALLOC_MMATRIX(op1,2,2);
	ALLOC_MMATRIX(op2,2,2);
	ALLOC_MMATRIX(op3,2,2);
	ALLOC_MMATRIX(c00,2,2);
	ALLOC_MMATRIX(c01,2,2);
	ALLOC_MMATRIX(c10,2,2);
	ALLOC_MMATRIX(c11,2,2);
	FOR(i,2) {
		FOR(j,2) {
			ALLOC_MATRIX(IDX(s,i,j),M,M);
			ALLOC_MATRIX(IDX(t,i,j),M,M);
			ALLOC_MATRIX(IDX(u,i,j),M,M);
			ALLOC_MATRIX(IDX(op3,i,j),M,M);
		}
	}
	selectQuadrant(0,0,dest,c00);
	selectQuadrant(0,2,dest,c01);
	selectQuadrant(2,0,dest,c10);
	selectQuadrant(2,2,dest,c11);

	selectQuadrant(2,0,src1,op1); //A[1][0]
	selectQuadrant(2,2,src1,op2); //A[1][1]
	addQuadrant(op1,op2,s);		//S=A10+A11
	selectQuadrant(0,2,src2,op1); //B[0][1]
	selectQuadrant(0,0,src2,op2); //B[0][0]
	subQuadrant(op1,op2,t);		//T=B01-B00
	mulQuadrant(s,t,u);			//U=S*T
	copyQuadrant(u,c11);	//C11=U
	copyQuadrant(u,c01);	//C01=U
	selectQuadrant(0,0,src1,op1); //A[0][0] //B[0][0]
	mulQuadrant(op1,op2,u);			//U=A00*B00
	selectQuadrant(0,2,src1,op1); //A[0][1]
	selectQuadrant(2,0,src2,op2); //B[1][0]
	mulQuadrant(op1,op2,c00);//C00=A01*B10
	addQuadrant(u,c00,c00);//C00 = C00+U
	selectQuadrant(0,0,src1,op1); //A[0][0]
	subQuadrant(s,op1,s);//S = S - A00
	selectQuadrant(2,2,src2,op2); //B[1][1]
	subQuadrant(t,op2,t);//T = T - B11
	mulQuadrant(s,t,op3);//U -= S*T
	subQuadrant(u,op3,u);
	addQuadrant(c01,u,c01);//C01 = C01+U
	selectQuadrant(0,2,src1,op1); //A[0][1]
	subQuadrant(s,op1,s);//S = S-A01 //B[1][1]
	mulQuadrant(s,op2,op3);//C01 -= S*B11
	subQuadrant(c01,op3,c01);
	selectQuadrant(2,0,src2,op2); //B[1][0]
	addQuadrant(t,op2,t);//T = T+B10
	selectQuadrant(2,2,src1,op1); //A[1][1]
	mulQuadrant(op1,t,c10);//C10=A11*T
	selectQuadrant(0,0,src1,op1); //A[0][0]
	selectQuadrant(2,0,src1,op2); //A[1][0]
	subQuadrant(op1,op2,s);		//S=A00-A10
	selectQuadrant(2,2,src2,op1); //B[1][1]
	selectQuadrant(0,2,src2,op2); //B[0][1]
	subQuadrant(op1,op2,t);		//T=B11-B01
	mulQuadrant(s,t,op3);		//U += S*T
	addQuadrant(u,op3,u);
	addQuadrant(c11,u,c11);//C11 = U+C11
	addQuadrant(c10,u,c10);//C10 = U+C10
}

void conv_single(matrix* in, matrix* filter, matrix* out) {
	register int tr,tc;
	matrix *U,*V,*W;
	ALLOC_MATRIX(U,M,M);
	ALLOC_MATRIX(V,M,M);
	ALLOC_MATRIX(W,M,M);

	transformFilter(filter,U);

	begin = clock();
	FOR_STEP(tr,out->h-N+1,N) {
		FOR_STEP(tc,out->w-N+1,N) {
			transformInputTile(in,V,tr,tc);
			multiplyElementWise(U,V,W);
			antiTransformOutputTile(W,out,tr,tc);
		}
	}
	end = clock();
	conv_elapsed = end - begin;

	FREE_MATRIX(U);
	FREE_MATRIX(V);
	FREE_MATRIX(W);
}
void conv_single_naive(matrix* in, matrix* filter, matrix* out) {
	register int r,c;
	register int x,y;

	begin = clock();
	FOR(r,out->h) {
		FOR(c,out->w) {
			IDX(out,r,c) = 0;
			FOR(y,R) {
				FOR(x,R) {
					IDX(out,r,c) += FIXED_MUL(IDX_OFFSET(in,r,c,y,x),IDX(filter,y,x));
//					IDX(out,r,c) += FIXED_MUL(IDX_OFFSET_SAFE(in,r,c,y,x),IDX(filter,y,x));

				}
			}
		}
	}
	end = clock();
	conv_elapsed = end - begin;
}

void conv_full(tensor* in, macroMatrix* filterbank, tensor* out) {
	register int tr,tc,c,k,t;
	macroMatrix *U;
	macroMatrix *V;
	macroMatrix *W;
	matrix in_chan = {
			in->w,
			in->h,
			(void*)0
	};
	matrix out_bank = {
			out->w,
			out->h,
			(void*)0
	};

	//printf("Alloc mmatrix start\n");
	//kernels [KxC] * in [CxT] = out [KxT];
	ALLOC_MMATRIX(U,K,C);
	ALLOC_MMATRIX(V,C,T);
	ALLOC_MMATRIX(W,K,T);
	FOR(tr,K) {
		FOR(tc,C) {
			ALLOC_MATRIX(IDX(U,tr,tc),M,M);
		}
	}
	FOR(tr,C) {
		FOR(tc,T) {
			ALLOC_MATRIX(IDX(V,tr,tc),M,M);
		}
	}
	FOR(tr,K) {
		FOR(tc,T) {
			ALLOC_MATRIX(IDX(W,tr,tc),M,M);
			//printf("%d %d done (address: %p)\n",tr,tc,IDX(W,tr,tc));
		}
	}
	//printf("full w\n");

	//printf("Start transforming filters\n");
	transformFilters(filterbank, U);
	//printf("Filters transformed\n");

	begin = clock();
	FOR_STEP(tr,out->h-N+1,N) {
		FOR_STEP(tc,out->w-N+1,(N*T)) {
			//printf("%d %d\n",tr,tc);

			//transformInputTile(in,V,tr,tc);
			FOR(t,T) {
				int tc_current = tc+(t*N);
				FOR(c,C) {
					/*register int i,j;
					matrix in_tile = {
						M,M,malloc(sizeof(real)*16)
					};*/
					matrix* V_select = IDX(V,c,t);
					matrix* wat; //selezione dell'elemento di macroMatrix *V da riempire
					in_chan.data = TENSOR_CHANNEL_POINTER(in,c);
					transformInputTile(&in_chan,V_select,tr,tc_current);

					/*if (tr == 100 && tc_current>=72) {
						printf("%d %d(%d), ch=%d\n",tr,tc,tc_current,c);
						FOR(i,M) {
							FOR(j,M) {
								in_tile.data[i*4+j] = in_chan.data[(tr+i)*in_chan.w+tc_current+j];
							}
						}
						printMatrix_Side(&in_tile,V_select);
						if (tc_current == 80)
							exit(0);
					}*/
				}
			}
			//multiplyElementWise(U,V,W);
			//multiplyMacroMatrix_naive(U,V,W);
			multiplyMacroMatrix_strassen(U,V,W);

			//antiTransformOutputTile(W,out,tr,tc);
			FOR(t,T) {
				int tc_current = tc+(t*N);
				FOR(k,K) {
					matrix* W_select = IDX(W,k,t);
					out_bank.data = TENSOR_CHANNEL_POINTER(out,k);
					antiTransformOutputTile(W_select,&out_bank,tr,tc_current);
				}
			}
		}
	}
	end = clock();
	conv_elapsed = end - begin;
}

//TODO: allocare una macroMatrice CxT, riempirla con una riga di 4 tile input (una riga per semplicità), ciascuno con 4 canali
//i suoi elementi puntano a matrici singole 4x4: fare una trasformata di winograd in-place

//hai una macromatrice di filtri trasf. e una di tile trasf.
//le macromatrici contengono 4x4 elementi ciascuna, un elemento è un "tile" 4x4. NON SONO SOTTOMATRICI, sono elementi astratti
//devi fare il prodotto tra queste macromatrici, e l'operazione "prodotto" tra tile è il prodotto element-wise (NON PRODOTTO MATRICE)
//forse, per fare meno copie ed esplicitare il processo, conviene applicare strassen alla macromatrice

//strassen: 7 prodotti invece di 8
//strassen su macromatrice: 7x16 prodotti invece di 8x16
//strassen su elementi singoli: 16x7 invece di 16x8
//equivalenti

//fatto ciò, hai una macromatrice di tile trasf. in output
//fai una antitrasformata di Winograd, prendi e metti i 2x2 risultati nel posto giusto, come un DEMUX :D  -_-
//ricorda che hai preso i tile in riga, quindi mettili in rigaH in output.
//(la macromatrice ha dimensioni KxT; alla fine di tutto saltano fuori K immagni in output, a partire da input a C canali)

//CONSIDERAZIONE DA TESI:
//si è scelto di usare il procedimento che da un tile 4x4 (MxM) ricava un tile 2x2 (NxN), invece che viceversa (NxN input->MxM ouput).
//questo perché, essendo lo stride sempre 2(N), i tile 4x4(MxM) si sovrappongono.
//sovrapposizione in input -> leggi/usi un dato più volte, non problematico
//sovrapposizione in output -> bisogna sommare dei valori; evitiamo, per risparmiare operazioni.
//CONSIDERAZIONE DA TESI:
//un Winograd hardcoded (non calcolato dinamicamente) diminuisce il tempo di computazione già a lato software
//tuttavia, questo codice sarebbe più vantaggioso su un hardware dove l'indicizzazione è a costo zero (mentre qui: 1 somma + 1 prodotto al minimo)
//possibili ottimizzazioni: indicizzazione più intelligente a lato software

//ambiti di ricerca futuri: se effettivamente Winograd(N=2,R=3) è il più ottimale
//TODO: ho già un codice che fa winograd fino a 6x6, aggiungigi il cronometraggio e vedi se equivale.
//in realtà converrebbe riscriverlo nel contesto di questa versione
//CONSIDERAZIONE DA TESI: con dimensioni maggiori di N=2,R=3 servono più somme -> più tempo, sopratt. a lato sw

//TODO: dividere questo in conv_single.c e conv_multi.c, ciascuno inizializza i propri parametri statici.
