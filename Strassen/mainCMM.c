/*
 ============================================================================
 Name        : mainTensor.c
 Author      : Alessio Onori
 Version     :
 Copyright   : Your copyright notice
 Description : MAIN FONDAMENTALE, risultati della TESI a confronto
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include "CMM.h"
#include "bank.h"
#include "matrix.h"
#include "convolution.h"
#include "convolution_bank.h"


int main(void) {

	/*
	 * sarebbe interessante poter definire un banco avendo gia definito
	 * vari tensori per esempio...
	 * al momento devo invece procedere elemento per elemento
	 */

	/*
	 * banco immagini costituito da 2 immagini
	 */
	bank *imageBank = allocBank(6,6,4,4);//4 canali, 4 immagini
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


	//trasformo il banco in matrice
	matrix *imageCMM = matrixFromBank_image(imageBank);

	//stampo
	printf("Banco Immagine mappato in matrice:\n");
	printMatrix(imageCMM);
	printf("\n");


	//mi occupo delle 4 sottoMatrici
	matrix *I11=allocMatrix((imageCMM->h)/2,(imageCMM->w)/2);matrix *I12=allocMatrix((imageCMM->h)/2,(imageCMM->w)/2);
	matrix *I21=allocMatrix((imageCMM->h)/2,(imageCMM->w)/2);matrix *I22=allocMatrix((imageCMM->h)/2,(imageCMM->w)/2);
	split4Matrix(imageCMM,I11,I12,I21,I22);

	//stampo
	printf("Le 4 SottoMatrici...:\n");
	printMatrix(I11);
	printf("\n");
	printMatrix(I12);
	printf("\n");
	printMatrix(I21);
	printf("\n");
	printMatrix(I22);
	printf("\n");



	/*
	 * banco kernel costituito da 2 kernel
	 */
	bank *kernelBank = allocBank(3,3,4,4);//ci sono 4 filtri da 4 canali
	numero=0;
	for(p=0;p<kernelBank->p;p++){
		for(ch=0;ch<kernelBank->ch;ch++)
			for(i=0;i<kernelBank->h;i++)
				for(j=0;j<kernelBank->w;j++){
//					kernelBank->data[p]->data[ch]->data[i][j]=numero;
					kernelBank->data[p]->data[ch]->data[i][j]=1;
					numero++;
				}
	}




	//stampo
	printf("Banco Kernel:\n");
	printBank(kernelBank);
	printf("\n");


	//trasformo il banco in matrice
	matrix *kernelCMM = matrixFromBank_kernel(kernelBank);

	//stampo
	printf("Banco kernel mappato in matrice:\n");
	printMatrix(kernelCMM);
	printf("\n");

	//mi occupo delle 4 sottoMatrici
	matrix *K11=allocMatrix((kernelCMM->h)/2,(kernelCMM->w)/2);matrix *K12=allocMatrix((kernelCMM->h)/2,(kernelCMM->w)/2);
	matrix *K21=allocMatrix((kernelCMM->h)/2,(kernelCMM->w)/2);matrix *K22=allocMatrix((kernelCMM->h)/2,(kernelCMM->w)/2);
	split4Matrix(kernelCMM,K11,K12,K21,K22);

	//stampo
	printf("Le 4 SottoMatrici...:\n");
	printMatrix(K11);
	printf("\n");
	printMatrix(K12);
	printf("\n");
	printMatrix(K21);
	printf("\n");
	printMatrix(K22);
	printf("\n");




	//provo ora i prodotti di convoluzione
	int nsum=0,nmul=0;
	bank *result=convolveNaive_border_bank(kernelBank, imageBank,&nsum,&nmul);
	//stampo
	printf("Risultato della CONVOLUZIONE NAIVE:\n");
	printBank(result);
	printf("\n");
	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d\n", nmul);

	//salvo il conteggio
	int nsumNaive=nsum;
	int nmulNaive=nmul;

	//provo finalmente CMM
	nsum=0;nmul=0;
	bank *resultCMM = convolveCMM(kernelBank, imageBank,&nsum,&nmul);


	//stampo
	printf("\nRisultato della CONVOLUZIONE CMM:\n");
	printBank(resultCMM);
	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d\n", nmul);

	//salvo il conteggio
	int nsumCMMNaive=nsum;
	int nmulCMMNaive=nmul;

	//CMM con toeplitz + strassen a livello di convoluzione vera
	nsum=0;nmul=0;
	bank *resultCMM2 = convolveCMM2(kernelBank, imageBank,&nsum,&nmul);

	//stampo
	printf("\nRisultato della CONVOLUZIONE CMM + TOEPLITZ + STRASSEN a basso livello:\n");
	printBank(resultCMM2);
	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d\n", nmul);

	//salvo il conteggio
	int nsumCMMToep=nsum;
	int nmulCMMToep=nmul;


	//CMM con strassen a livello alto "di geometrie": mi aspetto il più performante
	nsum=0;nmul=0;
	bank *resultCMM3 = convolveCMM3(kernelBank, imageBank,&nsum,&nmul);

	//stampo
	printf("\nRisultato della CONVOLUZIONE STRASSEN ad alto livello:\n");
	printBank(resultCMM3);
	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d\n", nmul);

	//salvo il conteggio
	int nsumCMMStras=nsum;
	int nmulCMMStras=nmul;

	//CMM con strassen a livello alto "di geometrie": mi aspetto il più performante
	nsum=0;nmul=0;
	bank *resultCMM4 = convolveCMM4(kernelBank, imageBank,&nsum,&nmul);

	//stampo
	printf("\nRisultato della CONVOLUZIONE STRASSEN ad alto livello, winograd a basso:\n");
	printBank(resultCMM4);
	printf("numero somme: %d\n", nsum);
	printf("numero moltiplicazioni: %d\n", nmul);

	//salvo il conteggio
	int nsumCMMStrasWin=nsum;
	int nmulCMMStrasWin=nmul;


	printf("**************CONFRONTO****************:\n");
	printf("nsumNaive: %d\tnsumCMMNaive: %d\tnsumCMMToep: %d\tnsumCMMStras: %d\tnsumCMMStrasWin: %d\n", nsumNaive,nsumCMMNaive,nsumCMMToep,nsumCMMStras,nsumCMMStrasWin);
	printf("nmulNaive: %d\tnmulCMMNaive: %d\tnmulCMMToep: %d\tnmulCMMStras: %d\tnmulCMMStrasWin: %d\n", nmulNaive,nmulCMMNaive,nmulCMMToep,nmulCMMStras,nmulCMMStrasWin);

	//confronto automatico di Strassen con naive:
	if(equals(result,resultCMM4)){
		puts("il risultato è corretto");
	}
	else{
		puts("something wrong...");
	}

	//dealloco tutto:
	freeBank(imageBank);freeBank(kernelBank);freeMatrix(imageCMM);freeMatrix(kernelCMM);
	freeMatrix(I11);freeMatrix(I12);freeMatrix(I21);freeMatrix(I22);
	freeMatrix(K11);freeMatrix(K12);freeMatrix(K21);freeMatrix(K22);
	freeBank(result);freeBank(resultCMM);


	return EXIT_SUCCESS;
}




