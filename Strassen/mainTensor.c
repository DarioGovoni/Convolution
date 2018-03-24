/*
 ============================================================================
 Name        : mainTensor.c
 Author      : Alessio Onori
 Version     :
 Copyright   : Your copyright notice
 Description : test della convoluzione tra tensori
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include "convolution_tensor.h"


int maiiiiiiiin(void) {

	//definisco un tensore immagine
	tensor *T1=allocTensor(25,25,3);
	int numero=0;
	int i=0;
	int j=0;
	int c;
	for(c=0;c<T1->ch;c++)
		for(i=0;i<T1->h;i++)
			for(j=0;j<T1->w;j++){
				(T1->data)[c]->data[i][j]=numero;
				numero++;
			}


	//stampo
	printf("Tensore Immagine:\n");
	printTensor(T1);
	printf("\n");


	//definisco un tensore kernel
	tensor *TK=allocTensor(5,5,3);
	numero=1;
	for(c=0;c<TK->ch;c++)
		for(i=0;i<TK->h;i++)
			for(j=0;j<TK->w;j++){
				(TK->data)[c]->data[i][j]=numero;
				numero++;
			}



	//stampo
	printf("Tensore Kernel:\n");
	printTensor(TK);
	printf("\n");



	/*
	 * prodotto di convoluzione naive tra 2 tensori
	 */
	int nsum=0; int nmul=0;
	matrix *convolution_naive = convolveNaive_border_tensor(TK,T1,&nsum,&nmul);

	//stampo
	printf("Prodotto di convoluzione:\n");
	printMatrix(convolution_naive);
	printf("\n");
	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d", nmul);



	//dealloco tutto: T1,TK, convolution_naive
	freeTensor(T1); freeTensor(TK); freeMatrix(convolution_naive);


	return EXIT_SUCCESS;
}




