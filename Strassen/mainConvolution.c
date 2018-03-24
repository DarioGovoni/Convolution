/*
 ============================================================================
 Name        : Strassen.c
 Author      : Alessio Onori
 Version     :
 Copyright   : Your copyright notice
 Description : convoluzione tra 2 matrici e confronto tra metodologia naive e tramite ausilio di Toeplitz
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "convolution.h"
#include "conv_winograd_2x3.h"


int maiiiiin(void) {

#define DIM 3


	//definisco la prima matrice di interi e la alloco, l'immagine
	matrix *M1=allocSquareMatrix(DIM);

	//riempio banalmente
	int numero=0;
	int i=0;
	int j=0;
	for(i=0;i<M1->h;i++)
		for(j=0;j<M1->w;j++){
			(M1->data)[i][j]=numero;
			numero++;
		}


	//definisco il kernel e lo riempio manualmente
	matrix *M2=allocMatrix(3,3);
	M2->data[0][0]=1;M2->data[0][1]=1;M2->data[0][2]=1;
	M2->data[1][0]=1;M2->data[1][1]=1;M2->data[1][2]=1;
	M2->data[2][0]=1;M2->data[2][1]=1;M2->data[2][2]=1;


	//stampo
	printf("Immagine:\n");
	printMatrix(M1);
	printf("\n");


	printf("Kernel:\n");
	printMatrix(M2);
	printf("\n");



	/*
	 * prodotto di convoluzione tra kernel e immagine
	 */
	int nsum=0; int nmul=0;
	matrix *O = convolveNaive_border(M2,M1,&nsum,&nmul);


	//stampo
	printf("Risultato del prodotto di convoluzione:\n");
	printMatrix(O);
	printf("\n");

	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d\n\n", nmul);

	//salvo temporaneo
	//int tmpSum=nsum; int tmpMul=nmul;

	nsum=0;nmul=0;
	matrix *OToeplitz = convolveByToeplitz_naive(M1,M2,&nsum,&nmul);

	//stampo
//	printf("Risultato del prodotto di convoluzione con Topeliz + naive:\n");
//	printMatrix(OToeplitz);
	printf("\n");

	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d\n\n", nmul);



	nsum=0;nmul=0;
	matrix *OWinograd = allocSquareMatrix(DIM);
	conv_winograd_2x3(M1,M2,OWinograd,&nsum,&nmul);

	//stampo
//	printf("Risultato del prodotto di convoluzione con Winograd:\n");
//	printMatrix(OWinograd);
	printf("\n");

	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d\n\n", nmul);

	nsum=0;nmul=0;
	matrix *OWinograd2 = conv_winograd_2x3_bis(M1,M2,&nsum,&nmul);


	//stampo
	printf("Risultato del prodotto di convoluzione con Winograd bis:\n");
	printMatrix(OWinograd2);
	printf("\n");

	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d\n\n", nmul);




	//dealloco tutto: M1,M2,O
	freeMatrix(M1);freeMatrix(M2);
	freeMatrix(O); freeMatrix(OWinograd);freeMatrix(OWinograd2);


	return EXIT_SUCCESS;
}




