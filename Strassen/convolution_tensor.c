/*
 ============================================================================
 Name        : convolution_tensor.c
 Author      : Alessio Onori
 Version     :
 Copyright   : Your copyright notice
 Description : implementazione di convolution_tensor.h
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include "convolution_tensor.h"




matrix* convolveNaive_border_tensor(tensor *kernels, tensor *immagini, int *nsum, int *nmul){

	matrix *result = allocMatrix(immagini->h,immagini->w);
	initMatrix(result);

	// kernels sono matrici quadrate
	int m2 = (kernels->h)/2; //larghezza della cornice di zeri da applicare all'immagine
	int i1 = 0; //indice di riga da cui si parte a popolare la matrice result, non più vincolato dal bordo quindi da m2
	int i2 = (immagini->h)-1;//limite oltre al quale non far scorrere il kernel. Si noti come m2 non sia piu un vincolo
	int j1 = 0; //come i1 ma di colonna
	int j2 = (immagini->w)-1;//come i2 ma di colonna

	int c;
	for(c=0;c<immagini->ch;c++){//scorro i canali dell'immagine e del kernel
		int i;
		for(i=i1;i<=i2;i++){//scorro le righe dell'immagine
			int j;
			for(j=j1;j<=j2;j++){//scorro le colonne dell'immagine
				int val=0;

				int x;
				for(x=0;x< kernels->h;x++){//scorro le righe del kernel
					int y;
					for(y=0;y< kernels->w;y++){//scorro le colonne del kernel
						int riga=i+m2-x;
						int colonna=j+m2-y;
						if(riga<0 || colonna<0 || riga>i2 || colonna >j2)//overflow significa che sono sul bordo di zeri
							val+=0;
						else{
							val+=(immagini->data)[c]->data[i+m2-x][j+m2-y] * (kernels->data)[c]->data[x][y];

							(*nsum)++; (*nmul)++;
						}
					}
				}//righe kernel

				(result->data)[i][j]+=val;
			}
		}//righe immagine
	}//canali

	return result;
}








