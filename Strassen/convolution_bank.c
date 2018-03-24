/*
 ============================================================================
 Name        : convolution_bank.c
 Author      : Alessio Onori
 Version     :
 Copyright   : Your copyright notice
 Description : implementazione di convolution_bank.h
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include "convolution_bank.h"




bank* convolveNaive_border_bank(bank *kernel, bank *image, int *nsum, int *nmul){

	bank *result = allocBank(image->h,image->w,kernel->p,image->p);
	initBank(result);

	// kernels sono matrici quadrate
	int m2 = (kernel->h)/2; //larghezza della cornice di zeri da applicare all'immagine
	int i1 = 0; //indice di riga da cui si parte a popolare la matrice result, non più vincolato dal bordo quindi da m2
	int i2 = (image->h)-1;//limite oltre al quale non far scorrere il kernel. Si noti come m2 non sia piu un vincolo
	int j1 = 0; //come i1 ma di colonna
	int j2 = (image->w)-1;//come i2 ma di colonna


	//scorro il banco delle immagini
	int pI;
	for(pI=0;pI<image->p;pI++){
		//scorro il banco dei kernel
		int pK;
		for(pK=0;pK<kernel->p;pK++){

			int c;
			for(c=0;c<image->ch;c++){//scorro i canali dell'immagine e del kernel
				int i;
				for(i=i1;i<=i2;i++){//scorro le righe dell'immagine
					int j;
					for(j=j1;j<=j2;j++){//scorro le colonne dell'immagine
						int val=0;

						int x;
						for(x=0;x< kernel->h;x++){//scorro le righe del kernel, stesso ch immagine
							int y;
							for(y=0;y< kernel->w;y++){//scorro le colonne del kernel, stesso ch immagine

								int riga=i+m2-x;
								int colonna=j+m2-y;
								if(riga<0 || colonna<0 || riga>i2 || colonna >j2)//overflow significa che sono sul bordo di zeri
									val+=0;
								else{
									val+=( (image->data)[pI]->data[c]->data[i+m2-x][j+m2-y] ) * ( kernel->data[pK]->data[c]->data[x][y] );

									(*nmul)++;
									//la prima volta non si conta
									if( !(y==0 && x==0) )
										(*nsum)++;
								}
							}
						}//righe kernel

						result->data[pI]->data[pK]->data[i][j]+=val;

						//la passata dovuta al primo canale non si conta
						if(c!=0)
							(*nsum)++;
					}
				}//righe immagine
			}//canali

			//altro filtro altro ch immagine
		}//grado parallelismo kernel
	}//grado parallelismo immagine

	return result;
}








