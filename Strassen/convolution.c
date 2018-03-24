/*
 ============================================================================
 Name        : convolution.c
 Author      : Alessio Onori
 Version     :
 Copyright   : Your copyright notice
 Description : implementazione di convolution.h
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include "convolution.h"



void convolutionSize(matrix *m1, matrix *m2, int *h, int *w){
	//inizializzo per sicurezza
	*h=0;
	*w=0;

	*h=(m1->h)+(m2->h)-1;
	*w=(m1->w)+(m2->w)-1;

	/*
	 * ESEMPIO:
	 * 2x3 x 2x2 ==> deve tornare 3x4
	 */
}


matrix* convolveNaive(matrix *kernel, matrix *immagine){

	matrix *result = allocMatrix(immagine->h,immagine->w);
	initMatrix(result);

	// kernel è una matrice quadrata
	int m2 = (kernel->h)/2; //larghezza della cornice di zeri da applicare all'immagine, per ora un vincolo
	int i1 = m2; //indice di riga da cui si parte a popolare la matrice result
	int i2 = (immagine->h)-m2-1;//limite oltre al quale non far scorrere il kernel. Si noti come m2 per ora sia un vincolo
	int j1 = m2; //come i1 ma di colonna
	int j2 = (immagine->w)-m2-1;//come i2 ma di colonna

	int i;
	for(i=i1;i<=i2;i++){//scorro le righe dell'immagine
		int j;
		for(j=j1;j<=j2;j++){//scorro le colonne dell'immagine
			int val=0;

			int x;
			for(x=0;x< kernel->h;x++){//scorro le righe del kernel
				int y;
				for(y=0;y< kernel->w;y++){//scorro le colonne del kernel
					val+=(immagine->data)[i+m2-x][j+m2-y] * (kernel->data)[x][y];
				}
			}//righe kernel

			(result->data)[i][j]=val;
		}
	}//righe immagine

	return result;
}


matrix* convolveNaive_border(matrix *kernel, matrix *immagine, int *nsum, int *nmul){

	matrix *result = allocMatrix(immagine->h,immagine->w);
	initMatrix(result);

	// kernel è una matrice quadrata
	int m2 = (kernel->h)/2; //larghezza della cornice di zeri da applicare all'immagine
	int i1 = 0; //indice di riga da cui si parte a popolare la matrice result, non più vincolato dal bordo quindi da m2
	int i2 = (immagine->h)-1;//limite oltre al quale non far scorrere il kernel. Si noti come m2 non sia piu un vincolo
	int j1 = 0; //come i1 ma di colonna
	int j2 = (immagine->w)-1;//come i2 ma di colonna

//	//correzzione sul bordo dell'immagine
//	matrix *immagineCorretta = allocMatrix(immagine->h+m2,immagine->w+m2);
//	initMatrix(immagineCorretta);
//	copySubMatrix2(immagine,immagineCorretta,m2,m2);
	/*
	 * mi si complicherebbe la gestione degli indici....
	 * procedo con soluzione "gestione overflow"
	 */

	int i;
	for(i=i1;i<=i2;i++){//scorro le righe dell'immagine
		int j;
		for(j=j1;j<=j2;j++){//scorro le colonne dell'immagine
			int val=0;

			int x;
			for(x=0;x< kernel->h;x++){//scorro le righe del kernel
				int y;
				for(y=0;y< kernel->w;y++){//scorro le colonne del kernel
					int riga=i+m2-x;
					int colonna=j+m2-y;
					if(riga<0 || colonna<0 || riga>i2 || colonna >j2){
						//overflow significa che sono sul bordo di zeri
						val+=0;
//						if( !(x==0 && y==0) )
//							(*nsum)++;
//						(*nmul)++;
					}

					else{
						val+=(immagine->data)[i+m2-x][j+m2-y] * (kernel->data)[x][y];

						(*nmul)++;
						//la primissima somma non si conta
						if( !(x==0 && y==0) )
							(*nsum)++;

					}
				}
			}//righe kernel

			(result->data)[i][j]=val;
		}
	}//righe immagine

	return result;
}


matrix* convolveByToeplitz_naive(matrix *m1, matrix *m2, int *nsum, int *nmul){
	//ottengo la dimensione desiderata
	int h,w;
	convolutionSize(m1,m2,&h,&w);

	matrix *result=allocMatrix(h,w);

	/*
	 * PADDING m1 di zeri
	 */
	int m1Modified=0;//0=false;
	m1Modified=padMatrixToDim(&m1,h,w);
	/*
	 * ATTENZIONE:
	 * non era compito di questa funzione deallocare la struttura m.
	 * Nel caso sia necessario, la funzione padMatrixToDim alloca una nuova struttura: questa si che andrà deallocata!
	 */


	/*
	 * PER OGNI RIGA DI m1:
	 * costruzione di una nuova matrice di dimensioni
	 * m1->w x m2->w
	 * NOTA: per ogni riga di m1 == servono m1->h matrici
	 */
	matrix **Hi = malloc( (m1->h)*sizeof(matrix*)); //array dinamico di strutture matrice
	int i;
	for(i=0; i< m1->h; i++){
		Hi[i]=allocMatrix(m1->w,m2->w);

		//popolo la matrice verticalmente e ciclicamente, con shift
		int j,k;
		int shift=0;
		for(j=0;j<Hi[i]->w;j++){//colonne di Hi
			for(k=0;k<Hi[i]->h;k++){//righe di Hi
				int riga = (k+shift)%(Hi[i]->h);
				Hi[i]->data[riga][j]=(m1->data)[i][k];
			}
			shift++;
		}
	}
	int numH=i;//dimensione logica = numH = m1->h

	//debug
//	for(i=0;i<numH;i++){
//		printf("Hi[%d]:\n",i+1);
//		printMatrix(Hi[i]);
//	}


	/*
	 * costruisco la matrice di Toeplitz principale
	 * delle dimensioni numH*Hi[i]->h x Hi[i]->w*m2->h
	 */
	matrix *mainH=allocMatrix(numH*Hi[0]->h,(Hi[0]->w)*(m2->h));


	//popolo la matrice per copia, verticalmente e ciclicamente con shift
	int j;
	int shift=0;
	for(j=0;j<m2->w;j++){//colonne
		for(i=0;i<numH;i++){//righe
			int riga = (i+shift)%numH;
			copySubMatrix2(Hi[i],mainH,riga*Hi[i]->h,j*Hi[i]->w);
		}
		shift++;
	}

	//debug
//	puts("H GIGANTE:");
//	printMatrix(mainH);


	//trasformo m2 in un vettore colonna
	int dimM2 = m2->h * m2->w;
	matrix *m2Colonna=allocMatrix(dimM2,1);

	//unrol di m2 in colonna
	int k=0;
	for(i=0;i<m2->h;i++){//righe
		for(j=0;j<m2->w;j++){//colonne
			(m2Colonna->data)[k][0]=(m2->data)[i][j];
			k++;
		}
	}

	//debug
//	puts("m2 colonna:");
//	printMatrix(m2Colonna);


	/*
	 * è ora possibile eseguire il prodotto di convoluzione come il prodotto tra le 2
	 * matrici mainH e colonna: le 2 matrici sono compatibili per la moltiplicazione per costruzione
	 */
	matrix *convolution=multiplyMatrix_naive(mainH,m2Colonna,nsum,nmul);//implemento la moltiplicazione con quello che voglio
	//questo è un vettore colonna

	//riscrivo il risultato in result
	//matrix *convolutionFinal=allocMatrix(h,w);//calcolati all inizio
	k=0;
	for(i=0;i<h;i++){
		for(j=0;j<w;j++){
			result->data[i][j]=convolution->data[k][0];
			k++;
		}
	}

	//risultato del prodotto di convoluzine
	//debug
//	puts("convolution by toeplitz:");
//	printMatrix(result);

	/*
	 * ricordo di deallocare tutte le strutture
	 */
	if(m1Modified)
		freeMatrix(m1);
	freeMatrix(mainH);freeMatrix(m2Colonna);freeMatrix(convolution);
	for(i=0;i<numH;i++)
		freeMatrix(Hi[i]);

	return result;
}


matrix* convolveByToeplitz_strassen(matrix *m1, matrix *m2, int *nsum, int *nmul){
	//ottengo la dimensione desiderata
	int h,w;
	convolutionSize(m1,m2,&h,&w);

	matrix *result=allocMatrix(h,w);

	/*
	 * PADDING m1 di zeri
	 */
	int m1Modified=0;//0=false;
	m1Modified=padMatrixToDim(&m1,h,w);
	/*
	 * ATTENZIONE:
	 * non era compito di questa funzione deallocare la struttura m.
	 * Nel caso sia necessario, la funzione padMatrixToDim alloca una nuova struttura: questa si che andrà deallocata!
	 */


	/*
	 * PER OGNI RIGA DI m1:
	 * costruzione di una nuova matrice di dimensioni
	 * m1->w x m2->w
	 * NOTA: per ogni riga di m1 == servono m1->h matrici
	 */
	matrix **Hi = malloc( (m1->h)*sizeof(matrix*)); //array dinamico di strutture matrice
	int i;
	for(i=0; i< m1->h; i++){
		Hi[i]=allocMatrix(m1->w,m2->w);

		//popolo la matrice verticalmente e ciclicamente, con shift
		int j,k;
		int shift=0;
		for(j=0;j<Hi[i]->w;j++){//colonne di Hi
			for(k=0;k<Hi[i]->h;k++){//righe di Hi
				int riga = (k+shift)%(Hi[i]->h);
				Hi[i]->data[riga][j]=(m1->data)[i][k];
			}
			shift++;
		}
	}
	int numH=i;//dimensione logica = numH = m1->h

	//debug
//	for(i=0;i<numH;i++){
//		printf("Hi[%d]:\n",i+1);
//		printMatrix(Hi[i]);
//	}

	/*
	 * costruisco la matrice di Toeplitz principale
	 * delle dimensioni numH*Hi[i]->h x Hi[i]->w*m2->h
	 */
	matrix *mainH=allocMatrix(numH*Hi[0]->h,(Hi[0]->w)*(m2->h));


	//popolo la matrice per copia, verticalmente e ciclicamente con shift
	int j;
	int shift=0;
	for(j=0;j<m2->w;j++){//colonne
		for(i=0;i<numH;i++){//righe
			int riga = (i+shift)%numH;
			copySubMatrix2(Hi[i],mainH,riga*Hi[i]->h,j*Hi[i]->w);
		}
		shift++;
	}

	//debug
//	puts("H GIGANTE:");
//	printMatrix(mainH);


	//trasformo m2 in un vettore colonna
	int dimM2 = m2->h * m2->w;
	matrix *m2Colonna=allocMatrix(dimM2,1);

	//unrol di m2 in colonna
	int k=0;
	for(i=0;i<m2->h;i++){//righe
		for(j=0;j<m2->w;j++){//colonne
			(m2Colonna->data)[k][0]=(m2->data)[i][j];
			k++;
		}
	}

	//debug
//	puts("m2 colonna:");
//	printMatrix(m2Colonna);


	/*
	 * è ora possibile eseguire il prodotto di convoluzione come il prodotto tra le 2
	 * matrici mainH e colonna: le 2 matrici sono compatibili per la moltiplicazione per costruzione
	 */
	matrix *convolution=multiplyMatrix_strassen(mainH,m2Colonna,nsum,nmul);//implemento la moltiplicazione con quello che voglio
	//questo è un vettore colonna

	//riscrivo il risultato in result
	//matrix *convolutionFinal=allocMatrix(h,w);//calcolati all inizio
	k=0;
	for(i=0;i<h;i++){
		for(j=0;j<w;j++){
			result->data[i][j]=convolution->data[k][0];
			k++;
		}
	}

	//risultato del prodotto di convoluzine
	//debug
//	puts("convolution by toeplitz:");
//	printMatrix(result);

	/*
	 * ricordo di deallocare tutte le strutture
	 */
	if(m1Modified)
		freeMatrix(m1);
	freeMatrix(mainH);freeMatrix(m2Colonna);freeMatrix(convolution);
	for(i=0;i<numH;i++)
		freeMatrix(Hi[i]);

	return result;
}





