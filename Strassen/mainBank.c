/*
 ============================================================================
 Name        : mainBank.c
 Author      : Alessio Onori
 Version     :
 Copyright   : Your copyright notice
 Description : test della convoluzione tra banchi
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include "convolution_bank.h"
#include "bank.h"


int maiiiiiiiiiiin(void) {

	/*
	 * sarebbe interessante poter definire un banco avendo gia definito
	 * vari tensori per esempio...
	 * al momento devo invece procedere elemento per elemento
	 */

	/*
	 * banco immagini costituito da 2 immagini
	 */
//	bank *imageBank = allocBank(5,5,2,2);//2 canali, 2 immagini
	bank *imageBank = allocBank(5,5,2,1);
	int numero=0;
	int i=0;
	int j=0;
	int ch;
	int p;
	for(p=0;p<imageBank->p;p++){
		for(ch=0;ch<imageBank->ch;ch++)
			for(i=0;i<imageBank->h;i++)
				for(j=0;j<imageBank->w;j++){
					imageBank->data[p]->data[ch]->data[i][j]=numero;
					numero++;
				}
	}



	//stampo
	printf("Banco Immagine:\n");
	printBank(imageBank);
	printf("\n");


	/*
	 * banco kernel costituito da 2 kernel
	 */
//	bank *kernelBank = allocBank(3,3,2,4);//ci sono 4 filtri da 2 canali
	bank *kernelBank = allocBank(3,3,2,1);
	numero=0;
	for(p=0;p<kernelBank->p;p++){
		for(ch=0;ch<kernelBank->ch;ch++)
			for(i=0;i<kernelBank->h;i++)
				for(j=0;j<kernelBank->w;j++){
					kernelBank->data[p]->data[ch]->data[i][j]=numero;
					numero++;
				}
	}



	//stampo
	printf("Banco Kernel:\n");
	printBank(kernelBank);
	printf("\n");


	int nsum=0; int nmul=0;
	bank *convolution = convolveNaive_border_bank(kernelBank,imageBank,&nsum,&nmul);

	//stampo
	printf("Risultato della CONVOLUZIONE NAIVE:\n");
	printBank(convolution);
	printf("\n");
	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d", nmul);




	//dealloco tutto: T1,TK, convolution_naive
	freeBank(imageBank);freeBank(kernelBank);


	return EXIT_SUCCESS;
}




