/*
 ============================================================================
 Name        : Strassen.c
 Author      : Alessio Onori
 Version     :
 Copyright   : Your copyright notice
 Description : implementazione di matrix.h
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>//contiene per esempio INT_MAX e INT_MIN
#include <math.h>//funzioni matematiche ==> modifica su properties con -lm
#include "matrix.h"


matrix * allocMatrix(int h, int w){
	matrix *m = malloc ( sizeof(matrix) );
	m->h=h;
	m->w=w;

	m->data=(real**) malloc( (m->h)*sizeof(real*) );
	int i;
	for(i=0;i<(m->h);i++){
		(m->data)[i]=(real*) malloc( (m->w)*sizeof(real) );
	}

	return m;
}

void freeMatrix(matrix *m){
	//dealloco prima il livello piu profondo
	int i;
	for(i=0;i<(m->h);i++){
		free( (m->data)[i] );
	}
	//poi il primo livello
	free(m->data);
	//poi la struttura intera
	free(m);
}



matrix* allocSquareMatrix(int dim){
	return allocMatrix(dim,dim);
}

void reallocMatrix(matrix *m){
	int h = m->h;
	int w = m->w;

	if(m){
		freeMatrix(m);
	}

	allocMatrix(h,w);
}

void initMatrix(matrix *m){
	//dealloco solo il livello piu profondo
	int i;
	for(i=0;i<(m->h);i++){
		free( (m->data)[i] );
	}

	//agisco sui soli elementi puntati di secondo livello
	for(i=0;i<(m->h);i++){//per ogni riga
		(m->data)[i]=(real*) calloc( (m->w),sizeof(real) );
	}
}


void printMatrix(matrix *m){
	int r,c;
	for(r=0;r<(m->h);r++){//scorro righe
		for(c=0;c<(m->w);c++){//scorro colonne
			printf("%d | ", (int) (m->data)[r][c] );
		}
		printf("\n");
	}
	printf("\n");
}



matrix* multiplyMatrix_naive(matrix *m1, matrix *m2,int *nsum, int *nmul){

	matrix *result = allocMatrix(m1->h,m2->w);

	//scorro le colonne di M2, il ciclo più esterno
	int k;
	for(k=0;k<m2->w;k++){
		//scorro le righe di M1
		int i;
		for(i=0;i<m1->h;i++){
			int sum=0;
			//scorro le colonne di M1
			int j;
			for(j=0;j< m1->w ;j++){
				//eseguo prodotto
				//nota come sia il puntatore j l'unico a scorrere
				sum+= (m1->data)[i][j] * (m2->data)[j][k] ;

				//conto
				(*nmul)++;
				if(j!=0) //la prima è una somma con lo zero, non si conta
					(*nsum)++;
			}//j
			//finite le colonne su M1 aggiorno la matrice risultato
			(result->data)[i][k]=sum;
		}//i
	}//k

	return result;
}



void sumMatrix(matrix *result,matrix *m1, matrix *m2, int *nsum, int *nmul){

	//scorro le righe di M1
	int i;
	for(i=0;i< m1->h;i++){
		//scorro le colonne di M1
		int j;
		for(j=0;j< m1->w;j++){
			//eseguo somma
			(result->data)[i][j]=(m1->data)[i][j]+(m2->data)[i][j];

			//conto
			(*nsum)++;
		}//j
	}//i
}

//sottrazione
void subMatrix(matrix *result,matrix *m1, matrix *m2, int *nsum, int *nmul){

	int i;
	//scorro le righe di M1
	for(i=0;i< m1->h;i++){
		//scorro le colonne di M1
		int j;
		for(j=0;j< m1->w;j++){
			//eseguo somma
			(result->data)[i][j]=(m1->data)[i][j]-(m2->data)[i][j];

			//conto
			(*nsum)++;
		}//j
	}//i
}


matrix* sumMatrix2(matrix *m1, matrix *m2, int *nsum, int *nmul){

	//controllo sulle dimensioni avvenuto all'esterno, m1 e m2 compatibili = stesse dimensioni
	matrix *result = allocMatrix(m1->h,m1->w);

	//scorro le righe di M1
	int i;
	for(i=0;i< m1->h;i++){
		//scorro le colonne di M1
		int j;
		for(j=0;j< m1->w;j++){
			//eseguo somma
			(result->data)[i][j]=(m1->data)[i][j]+(m2->data)[i][j];

			//conto
			(*nsum)++;
		}//j
	}//i

	return result;
}

//sottrazione
matrix* subMatrix2(matrix *m1, matrix *m2, int *nsum, int *nmul){

	//controllo sulle dimensioni avvenuto all'esterno, m1 e m2 compatibili = stesse dimensioni
	matrix *result = allocMatrix(m1->h,m1->w);

	int i;
	//scorro le righe di M1
	for(i=0;i< m1->h;i++){
		//scorro le colonne di M1
		int j;
		for(j=0;j< m1->w;j++){
			//eseguo somma
			(result->data)[i][j]=(m1->data)[i][j]-(m2->data)[i][j];

			//conto
			(*nsum)++;
		}//j
	}//i

	return result;
}



/*
 * popolo per copia una matrice dest con un sottoinsieme degli elementi di una matrice source
 * PRE: dimensioni di dest entrambe minori delle dimensioni di source
 * Copia che procede in maniera ordinata ovviamente
 * previsti parametri di offset da cui considerare gli elementi di source
 */
void copySubMatrix(matrix *source, matrix *dest, int offsetH, int offsetW){
	int i,j,h,k;
	/*
	 * per entrambi i for: la prima condizione è fondamentale per non sforare la destinazione,
	 * la seconda è un controllo aggiuntivo che mi basta per inglobare tutti i worst case.
	 */
	for( i=offsetH,h=0;h<dest->h && i<source->h;i++,h++){//righe
		for( j=offsetW,k=0;k<dest->w && j<source->w;j++,k++){//colonne
			(dest->data)[h][k]=(source->data)[i][j];
		}
	}
}

/*
 * qui la differenza è che è dest ad essere più grande della sorgente, gli offset sono quindi
 * riferiti alla destinazione in questo caso
 */
void copySubMatrix2(matrix *source, matrix *dest, int offsetH, int offsetW){
	int i,j,h,k;
	/*
	 * per entrambi i for: la prima condizione è fondamentale per non sforare la destinazione,
	 * la seconda è un controllo aggiuntivo che mi basta per inglobare tutti i worst case.
	 */
	for( i=0,h=offsetH;h<dest->h && i<source->h;i++,h++){//righe
		for( j=0,k=offsetW;k<dest->w && j<source->w;j++,k++){//colonne
			(dest->data)[h][k]=(source->data)[i][j];
		}
	}
}


//mi tornano utili subito dopo, metodi privati
int getMaxMatrixDimension(matrix *a, matrix *b){
	int max = INT_MIN;
	if(a->h > max)
		max = a->h;
	if(a->w > max)
		max = a->w;
	if(b->h > max)
		max = b->h;
	if(b->w > max)
		max = b->w;
	return max;
}

int mySum(int a, int b, int *nsum){
	if(a!=0 && b!=0){
		(*nsum)++;
	}
	return a+b;

}

int mySub(int a, int b, int *nsum){
	if(a!=0 && b!=0){
		(*nsum)++;
	}
	return a-b;
}

int myMul(int a, int b, int *mul){
	if(a==0 || b==0){
		return 0;
	}
	(*mul)++;
	return a*b;
}




matrix* multiplyMatrix_strassen(matrix *m1, matrix *m2, int *nsum, int *nmul){

	matrix *result = allocMatrix(m1->h,m2->w);

	/*
	 * l'algoritmo di Strassen lavora solo su matrici quadrate di dimensione 2^n x 2^n.
	 * Niente paura: è possibile ricondurre qualsiasi matrice a tale dimensione aggiungendo
	 * un padding di zeri.
	 * Come prima cosa quindi ricavo la dimensione e verifico che soddisfi la condizione di Strassen,
	 * altimenti aggiungo del padding
	 */
	int d = (int) round( log2(getMaxMatrixDimension(m1,m2)) ) ;
	if( d < log2(getMaxMatrixDimension(m1,m2)))
		d++;
	//matrice quadrata, dimensione uguale alla minima potenza di 2 più vicina alla dimensione max precedente; tutte le matrici devono essere ricondotte a questa
	int dimensione = pow(2,d);

	/*
	 * PADDING m1
	 */
	int m1Modified=0;//0=false;
	m1Modified=padMatrixToDim(&m1,dimensione,dimensione);
	/*
	 * ATTENZIONE:
	 * non era compito di questa funzione deallocare la struttura m.
	 * Nel caso sia necessario la funzione padMatrixToDim alloca una nuova struttura: questa si che andrà deallocata!
	 */

	/*
	 * PADDING m2
	 */
	int m2Modified=0;//0=false;
	m2Modified=padMatrixToDim(&m2,dimensione,dimensione);
	/*
	 * ATTENZIONE:
	 * Nel caso sia necessario la funzione padMatrixToDim alloca una nuova struttura: questa si che andrà deallocata!
	 */


	//caso base, dimensione=2
	if(dimensione==2){
		//in questo caso trattiamo numeri

		//popolo le 7 matrici di Strassen
		//7 moltiplicazioni
		//10 fra somme e differenze se non vi è padding
		int s1,s2,s3,s4,s5,s6,s7;
		/*
		 * suppongo che somme e moltiplicazioni con lo zero
		 * non debbano essere conteggiate... ridefinisco le operazioni elementari
		 * per far si che ci sia un reale conteggio
		 */
		s1 = myMul( mySum((m1->data)[0][0],(m1->data)[1][1],nsum), mySum((m2->data)[0][0],(m2->data)[1][1],nsum), nmul);
		s2 = myMul( mySum((m1->data)[1][0],(m1->data)[1][1],nsum), (m2->data)[0][0], nmul);
		s3 = myMul( (m1->data)[0][0], mySub((m2->data)[0][1],(m2->data)[1][1],nsum), nmul);
		s4 = myMul( (m1->data)[1][1], mySub((m2->data)[1][0],(m2->data)[0][0],nsum), nmul);
		s5 = myMul( mySum((m1->data)[0][0],(m1->data)[0][1],nsum), (m2->data)[1][1], nmul);
		s6 = myMul( mySub((m1->data)[1][0],(m1->data)[0][0],nsum), mySum((m2->data)[0][0],(m2->data)[0][1],nsum), nmul);
		s7 = myMul( mySub((m1->data)[0][1],(m1->data)[1][1],nsum), mySum((m2->data)[1][0],(m2->data)[1][1],nsum), nmul);


		//risultato del prodotto tra matrici grazie a strassen
		//8 fra somme e differenze se non vi è padding
		(result->data)[0][0]=mySum(mySub(mySum(s1,s4,nsum),s5,nsum),s7,nsum);
		(result->data)[0][1]=mySum(s3,s5,nsum);
		(result->data)[1][0]=mySum(s2,s4,nsum);
		(result->data)[1][1]=mySub(mySum(s1,mySum(s3,s6,nsum),nsum),s2,nsum);


	}


	//ricorsione
	else{

		/*
		 * divido la complessita riducendo in 4 sottomatrici uguali di dimensione dim/2.
		 * Chiamero le 2 matrici A e B, ottenendo per ognuna le sottomatrici per copia.
		 * Le numero partendo da in alto a sinistra, finisco la riga e passo alla successiva
		 */
		matrix *a1 = allocSquareMatrix(dimensione/2); matrix *a2 = allocSquareMatrix(dimensione/2);
		matrix *a3 = allocSquareMatrix(dimensione/2); matrix *a4 = allocSquareMatrix(dimensione/2);

		matrix *b1 = allocSquareMatrix(dimensione/2); matrix *b2 = allocSquareMatrix(dimensione/2);
		matrix *b3 = allocSquareMatrix(dimensione/2); matrix *b4 = allocSquareMatrix(dimensione/2);

		//popolo correttamente per copia
		//matrice a
		copySubMatrix(m1,a1,0,0); copySubMatrix(m1,a2,0,dimensione/2);
		copySubMatrix(m1,a3,dimensione/2,0); copySubMatrix(m1,a4,dimensione/2,dimensione/2);

		//matrice b
		copySubMatrix(m2,b1,0,0); copySubMatrix(m2,b2,0,dimensione/2);
		copySubMatrix(m2,b3,dimensione/2,0); copySubMatrix(m2,b4,dimensione/2,dimensione/2);


//		//alloco le 7 matrici di strassen
//		matrix *s1 = allocSquareMatrix(dimensione/2);
//		matrix *s2 = allocSquareMatrix(dimensione/2);
//		matrix *s3 = allocSquareMatrix(dimensione/2);
//		matrix *s4 = allocSquareMatrix(dimensione/2);
//		matrix *s5 = allocSquareMatrix(dimensione/2);
//		matrix *s6 = allocSquareMatrix(dimensione/2);
//		matrix *s7 = allocSquareMatrix(dimensione/2);
		/*
		 * non più necessario allocarle manualmente, è cambiata
		 * la firma del metodo
		 */

		//alloco 2 matrici necessarie a contenere i risultati parziali
		matrix *operando1 = allocSquareMatrix(dimensione/2);
		matrix *operando2 = allocSquareMatrix(dimensione/2);

		//s1: prodotto con strassen, di due operandi somma... (a1+a4)x(b1+b4)
		sumMatrix(operando1,a1, a4, nsum, nmul);
		sumMatrix(operando2,b1, b4, nsum, nmul);
		matrix *s1 = multiplyMatrix_strassen(operando1,operando2,nsum,nmul);
		initMatrix(operando1);
		initMatrix(operando2);


		//s2 = (a3+a4) *b1
		sumMatrix(operando1,a3, a4, nsum, nmul);
		matrix *s2=multiplyMatrix_strassen(operando1,b1,nsum,nmul);
		initMatrix(operando1);

		//s3 = a1 * (b2-b4)
		subMatrix(operando1,b2, b4, nsum, nmul);
		matrix *s3=multiplyMatrix_strassen(a1,operando1,nsum,nmul);
		initMatrix(operando1);

		//s4 = a4 * (b3-b1)
		subMatrix(operando1,b3, b1, nsum, nmul);
		matrix *s4=multiplyMatrix_strassen(a4,operando1,nsum,nmul);
		initMatrix(operando1);

		//s5 = (a1+a2) x b4
		sumMatrix(operando1,a1, a2, nsum, nmul);
		matrix *s5=multiplyMatrix_strassen(operando1,b4,nsum,nmul);
		initMatrix(operando1);

		//s6 = (a3-a1)x(b1+b2)
		subMatrix(operando1,a3, a1, nsum, nmul);
		sumMatrix(operando2,b1, b2, nsum, nmul);
		matrix *s6=multiplyMatrix_strassen(operando1,operando2,nsum,nmul);
		initMatrix(operando1);
		initMatrix(operando2);

		//s7 = (a2-a4)x(b3+b4)
		subMatrix(operando1,a2, a4, nsum, nmul);
		sumMatrix(operando2,b3, b4, nsum, nmul);
		matrix *s7=multiplyMatrix_strassen(operando1,operando2,nsum,nmul);
		initMatrix(operando1);
		initMatrix(operando2);

		//libero le 2 matrici operando
		freeMatrix(operando1);freeMatrix(operando2);


		//allocazione matrice temporanea: da ricordarsi deallocare
		matrix *tmp = allocSquareMatrix(dimensione/2);

		//popolo la matrice risultato, 1 elemento alla volta, procedendo per quadranti come al solito

		//s1+s4-s5+s7
		sumMatrix(tmp,s1,s4,nsum,nmul);
		subMatrix(tmp,tmp,s5,nsum,nmul);
		sumMatrix(tmp,tmp,s7,nsum,nmul);
		copySubMatrix2(tmp,result,0,0);
		initMatrix(tmp);

		//s3+s5
		sumMatrix(tmp,s3,s5,nsum,nmul);
		copySubMatrix2(tmp,result,0,dimensione/2);
		initMatrix(tmp);

		//s2+s4
		sumMatrix(tmp,s2,s4,nsum,nmul);
		copySubMatrix2(tmp,result,dimensione/2,0);
		initMatrix(tmp);

		//s1-s2+s3+s6
		subMatrix(tmp,s1,s2,nsum,nmul);
		sumMatrix(tmp,tmp,s3,nsum,nmul);
		sumMatrix(tmp,tmp,s6,nsum,nmul);
		copySubMatrix2(tmp,result,dimensione/2,dimensione/2);
		freeMatrix(tmp);

		//finito finalmente, libero tutte le strutture allocate
		freeMatrix(a1);freeMatrix(a2);freeMatrix(a3);freeMatrix(a4);
		freeMatrix(b1);freeMatrix(b2);freeMatrix(b3);freeMatrix(b4);
		freeMatrix(s1);freeMatrix(s2);freeMatrix(s3);freeMatrix(s4);
		freeMatrix(s5);freeMatrix(s6);freeMatrix(s7);

		/*
		 * non dimenticamoci delle eventuali m1 e m2 riallocate in caso abbia dovuto
		 * aggiungere del padding
		 */
		if(m1Modified)
			freeMatrix(m1);
		if(m2Modified)
			freeMatrix(m2);
	}

	return result;
}//strassen



int padMatrixToDim(matrix **m,int hTarget, int wTarget){
	int result=0;//false, ancora non è stato necessario allocare nulla

	if((*m)->h < hTarget || (*m)->w < wTarget){
		//alloco un altra struttura delle dimensioni giuste
		matrix *tmp = allocMatrix(hTarget,wTarget);
		result=1;

		//ricopio la matrice in tmp
		copySubMatrix2(*m,tmp,0,0);

		//aggiungo il padding mancante
		int i,j;
		for(i=0;i< tmp->h;i++)
			for(j=0;j< tmp->w;j++)
				if(i >= (*m)->h || j >= (*m)->w )
					(tmp->data)[i][j]=0;

		/*
		 * sgancio il puntatore m dalla vecchia struttura puntata
		 */
		*m = tmp;
	}

	return result;
}



