/*
 * CMM.c
 *
 *  Created on: 20 feb 2018
 *      Author: ale
 *
 *      implemetazione di CMM.h
 */


#include "CMM.h"
#include <stdio.h>

matrix* matrixFromBank_image(bank *image){

	//matrice risultato che verrà tornata
	/*
	 * altezza dipendente dai canali del tensore
	 * larghezza dipendente dal parallelismo del banco
	 *
	 * probabilmente qui andrebbero calcolati i massimi e poi la piu vicina
	 * dimensione quadrata alla potenza di 2
	 * successivamente fatto il padding con gli zeri ( non necessaria se
	 * fatta una operazione di init )
	 *
	 * Per ora suppongo che h e w siano uguali
	 */
	matrix *result = allocMatrix(image->ch * image->h, image->w * image->p);
	initMatrix(result);


	//ora la matrice va riempita
	//indici di altezza e larghezza assoluti riferiti alla matrice result
	int i=0,j=0;

	int p;//parallelismo banchi
	for(p=0;p<image->p;p++){
		int ch;//canali tensore
		for(ch=0;ch<image->ch;ch++){
			int h;
			for(h=0;h<image->h;h++){//scorro le righe
				int w;
				for(w=0;w<image->w;w++){//scorro colonne
					result->data[i][j] = image->data[p]->data[ch]->data[h][w];
					j++;
				}//col

				//...prima di cambiare riga
				i++;
				j-=image->w;
			}//row
		}//canali

		//..prima di cambiare tensore
		j+=image->w;
		i=0;
	}//tensori

	return result;
}



matrix* matrixFromBank_kernel(bank *kernel){

	//matrice risultato che verrà tornata
	/*
	 * larghezza dipendente dai canali del tensore
	 * altezza dipendente dal parallelismo del banco
	 *
	 * probabilmente qui andrebbero calcolati i massimi e poi la piu vicina
	 * dimensione quadrata alla potenza di 2
	 * successivamente fatto il padding con gli zeri ( non necessaria se
	 * fatta una operazione di init )
	 *
	 * Per ora suppongo che h e w siano uguali
	 */
	matrix *result = allocMatrix(kernel->p * kernel->h, kernel->w * kernel->ch);
	initMatrix(result);


	//ora la matrice va riempita
	//indici di altezza e larghezza assoluti riferiti alla matrice result
	int i=0,j=0;

	int p;//parallelismo banchi
	for(p=0;p<kernel->p;p++){
		int ch;//canali tensore
		for(ch=0;ch<kernel->ch;ch++){
			int h;
			for(h=0;h<kernel->h;h++){//scorro le righe
				int w;
				for(w=0;w<kernel->w;w++){//scorro colonne
					result->data[i][j] = kernel->data[p]->data[ch]->data[h][w];
					j++;
				}//col

				//...prima di cambiare riga
				i++;
				j-=kernel->w;
			}//row

			//prima di cambiare canale
			i-=kernel->h;
			j+=kernel->w;
		}//canali

		//..prima di cambiare tensore
		j=0;
		i+=kernel->h;
	}//tensori

	return result;
}



bank* bankFromMatrix_out(matrix *out,int imageH,int imageW,int kernelP,int imageP){

	bank *result = allocBank(imageH,imageW,kernelP,imageP);
	initBank(result);


	//riempio il banco

	//indici di altezza e larghezza assoluti riferiti alla matrix out
	int i=0,j=0;

	int tensore;
	for(tensore=0;tensore<imageP;tensore++){
		int canale;
		for(canale=0;canale<kernelP;canale++){
			int h;
			for(h=0;h<imageH;h++){
				int w;
				for(w=0;w<imageW;w++){
					result->data[tensore]->data[canale]->data[h][w] = out->data[i][j];
					j++;
				}

				j-=imageW;
				i++;
			}//cambio riga
		}//cambio canale
		i=0;
		j+=imageW;
	}//cambio tensore


	return result;
}


void split4Matrix(matrix *main, matrix *m11, matrix *m12, matrix *m21, matrix *m22){

	//suppongo tutte le dimensioni pari e quindi perfettamente divisibili per 2
	copySubMatrix(main,m11,0,0);copySubMatrix(main,m12,0,(main->w)/2);
	copySubMatrix(main,m21,(main->h)/2,0);copySubMatrix(main,m22,(main->h)/2,(main->w)/2);

}



bank* convolveCMM(bank *kernel, bank *image, int *nsum, int *nmul){

	//il risultato è un banco di tensori
	bank *result;

	//per ora non generalizzo per tutte le dimensioni, tengo semplice

	//trasformo il banco immagine in una matrice
	matrix *imageMatrix = matrixFromBank_image(image);

	//trasformo il banco kernel in una matrice
	matrix *kernelMatrix = matrixFromBank_kernel(kernel);

	//suppongo quadrate le dimensioni di kernel e image
	matrix *outMatrix = convolveCMM_naive(kernelMatrix, kernel->h, imageMatrix, image->h,nsum,nmul);
	//debug
	//	puts("*******DEBUG1*******");
	//	printMatrix(outMatrix);
	//	puts("**************");

	/*
	 * operazione inversa di sintesi: bankFromMatrix_out
	 */
	result=bankFromMatrix_out(outMatrix,image->h,image->w,kernel->p,image->p);

	return result;
}

matrix * convolveCMM_naive(matrix *kernel, int kernelSize, matrix *image, int imageSize,int *nsum,int *nmul){

	matrix *result;

	/*
	 * dimensione dell'elemento elementare: il BLOCCO
	 */
	if(kernel->h == kernelSize && image->h == imageSize){
		//posso svolgere la convoluzione elementare tra elementi BLOCCO, naive border questa implementazione
		result = convolveNaive_border(kernel,image,nsum,nmul);
	}
	else{
		//ALTRIMENTI continuo a dividere la complessità

		matrix *i11,*i12,*i21,*i22;
		int iAlloc=0;//false
		if(image->h > imageSize){
			//alloco 4 nuove strutture di dimensione dimezzata
			i11 = allocSquareMatrix(image->h/2); i12 = allocSquareMatrix(image->h/2);
			i21 = allocSquareMatrix(image->h/2); i22 = allocSquareMatrix(image->h/2);
			iAlloc=1;//true

			//riempio per copia
			copySubMatrix(image,i11,0,0);copySubMatrix(image,i12,0,image->w/2);
			copySubMatrix(image,i21,image->h/2,0);copySubMatrix(image,i22,image->h/2,image->w/2);

			//			puts("*******DEBUG2*******");
			//			printMatrix(i11);printMatrix(i12);printMatrix(i21);printMatrix(i22);
			//			puts("**************");
		}
		else{
			//non corretto ma al momento non mi importa
			i11 = image;i12 = image;i21 = image;i22 = image;
		}

		matrix *k11,*k12,*k21,*k22;
		int kAlloc=0;//false
		if(kernel->h > kernelSize){
			//alloco 4 nuove strutture di dimensione dimezzata
			k11 = allocSquareMatrix(kernel->h/2); k12 = allocSquareMatrix(kernel->h/2);
			k21 = allocSquareMatrix(kernel->h/2); k22 = allocSquareMatrix(kernel->h/2);
			kAlloc=1;//true

			//riempio per copia
			copySubMatrix(kernel,k11,0,0);copySubMatrix(kernel,k12,0,kernel->w/2);
			copySubMatrix(kernel,k21,kernel->h/2,0);copySubMatrix(kernel,k22,kernel->h/2,kernel->w/2);
		}
		else{
			//non corretto ma al momento non mi importa
			k11=kernel;k12=kernel;k21=kernel;k22=kernel;
		}

		//result è calcolata in maniera naive: moltiplicazione convoluzionale a blocchi con geometria Gauss

		matrix *tmp1=convolveCMM_naive(k11,kernelSize,i11,imageSize,nsum,nmul);
		matrix *tmp2=convolveCMM_naive(k12,kernelSize,i21,imageSize,nsum,nmul);
		matrix *Y11=sumMatrix2(tmp1,tmp2,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		tmp1=convolveCMM_naive(k11,kernelSize,i12,imageSize,nsum,nmul);
		tmp2=convolveCMM_naive(k12,kernelSize,i22,imageSize,nsum,nmul);
		matrix *Y12=sumMatrix2(tmp1,tmp2,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		tmp1=convolveCMM_naive(k21,kernelSize,i11,imageSize,nsum,nmul);
		tmp2=convolveCMM_naive(k22,kernelSize,i21,imageSize,nsum,nmul);
		matrix *Y21=sumMatrix2(tmp1,tmp2,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		tmp1=convolveCMM_naive(k21,kernelSize,i12,imageSize,nsum,nmul);
		tmp2=convolveCMM_naive(k22,kernelSize,i22,imageSize,nsum,nmul);
		matrix *Y22=sumMatrix2(tmp1,tmp2,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//popolo result
		result=allocMatrix(image->h,image->w);
		copySubMatrix2(Y11,result,0,0);copySubMatrix2(Y12,result,0,image->w/2);
		copySubMatrix2(Y21,result,image->h/2,0);copySubMatrix2(Y22,result,image->h/2,image->w/2);

		freeMatrix(Y11);freeMatrix(Y12);freeMatrix(Y21);freeMatrix(Y22);
		if(iAlloc){
			freeMatrix(i11);freeMatrix(i12);freeMatrix(i21);freeMatrix(i22);
		}
		if(kAlloc){
			freeMatrix(k11);freeMatrix(k12);freeMatrix(k21);freeMatrix(k22);
		}
	}

	//debug
	//	puts("*******DEBUG*******");
	//	printMatrix(result);
	//	puts("**************");

	return result;
}


bank* convolveCMM2(bank *kernel, bank *image, int *nsum, int *nmul){

	//il risultato è un banco di tensori
	bank *result;

	//per ora non generalizzo per tutte le dimensioni, tengo semplice

	//trasformo il banco immagine in una matrice
	matrix *imageMatrix = matrixFromBank_image(image);

	//trasformo il banco kernel in una matrice
	matrix *kernelMatrix = matrixFromBank_kernel(kernel);

	//suppongo quadrate le dimensioni di kernel e image
	matrix *outMatrix = convolveCMM_naive2(kernelMatrix, kernel->h, imageMatrix, image->h,nsum,nmul);

	//debug
	//	puts("*******DEBUG1*******");
	//	printMatrix(outMatrix);
	//	puts("**************");

	/*
	 * operazione inversa di sintesi: bankFromMatrix_out
	 */
	result=bankFromMatrix_out(outMatrix,image->h,image->w,kernel->p,image->p);

	return result;
}



matrix * convolveCMM_naive2(matrix *kernel, int kernelSize, matrix *image, int imageSize,int *nsum,int *nmul){

	matrix *result;

	if(kernel->h == kernelSize && image->h == imageSize){
		//posso svolgere la convoluzione elementare tra elementi BLOCCO, tramite Toeplitz + naive questa implementazione
		result = convolveByToeplitz_naive(kernel,image,nsum,nmul);
	}
	else{
		//ALTRIMENTI continuo a dividere la complessità

		matrix *i11,*i12,*i21,*i22;
		int iAlloc=0;//false
		if(image->h > imageSize){
			//alloco 4 nuove strutture di dimensione dimezzata
			i11 = allocSquareMatrix(image->h/2); i12 = allocSquareMatrix(image->h/2);
			i21 = allocSquareMatrix(image->h/2); i22 = allocSquareMatrix(image->h/2);
			iAlloc=1;//true

			//riempio per copia
			copySubMatrix(image,i11,0,0);copySubMatrix(image,i12,0,image->w/2);
			copySubMatrix(image,i21,image->h/2,0);copySubMatrix(image,i22,image->h/2,image->w/2);

			//			puts("*******DEBUG2*******");
			//			printMatrix(i11);printMatrix(i12);printMatrix(i21);printMatrix(i22);
			//			puts("**************");
		}
		else{
			//non corretto ma al momento non mi importa
			i11 = image;i12 = image;i21 = image;i22 = image;
		}

		matrix *k11,*k12,*k21,*k22;
		int kAlloc=0;//false
		if(kernel->h > kernelSize){
			//alloco 4 nuove strutture di dimensione dimezzata
			k11 = allocSquareMatrix(kernel->h/2); k12 = allocSquareMatrix(kernel->h/2);
			k21 = allocSquareMatrix(kernel->h/2); k22 = allocSquareMatrix(kernel->h/2);
			kAlloc=1;//true

			//riempio per copia
			copySubMatrix(kernel,k11,0,0);copySubMatrix(kernel,k12,0,kernel->w/2);
			copySubMatrix(kernel,k21,kernel->h/2,0);copySubMatrix(kernel,k22,kernel->h/2,kernel->w/2);
		}
		else{
			//non corretto ma al momento non mi importa
			k11=kernel;k12=kernel;k21=kernel;k22=kernel;
		}

		//result è calcolata in maniera naive: moltiplicazione convoluzionale a blocchi

		matrix *tmp1=convolveCMM_naive2(k11,kernelSize,i11,imageSize,nsum,nmul);
		matrix *tmp2=convolveCMM_naive2(k12,kernelSize,i21,imageSize,nsum,nmul);
		matrix *Y11=sumMatrix2(tmp1,tmp2,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		tmp1=convolveCMM_naive2(k11,kernelSize,i12,imageSize,nsum,nmul);
		tmp2=convolveCMM_naive2(k12,kernelSize,i22,imageSize,nsum,nmul);
		matrix *Y12=sumMatrix2(tmp1,tmp2,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		tmp1=convolveCMM_naive2(k21,kernelSize,i11,imageSize,nsum,nmul);
		tmp2=convolveCMM_naive2(k22,kernelSize,i21,imageSize,nsum,nmul);
		matrix *Y21=sumMatrix2(tmp1,tmp2,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		tmp1=convolveCMM_naive2(k21,kernelSize,i12,imageSize,nsum,nmul);
		tmp2=convolveCMM_naive2(k22,kernelSize,i22,imageSize,nsum,nmul);
		matrix *Y22=sumMatrix2(tmp1,tmp2,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//popolo result
		result=allocMatrix(image->h,image->w);
		copySubMatrix2(Y11,result,0,0);copySubMatrix2(Y12,result,0,image->w/2);
		copySubMatrix2(Y21,result,image->h/2,0);copySubMatrix2(Y22,result,image->h/2,image->w/2);

		freeMatrix(Y11);freeMatrix(Y12);freeMatrix(Y21);freeMatrix(Y22);
		if(iAlloc){
			freeMatrix(i11);freeMatrix(i12);freeMatrix(i21);freeMatrix(i22);
		}
		if(kAlloc){
			freeMatrix(k11);freeMatrix(k12);freeMatrix(k21);freeMatrix(k22);
		}
	}

	//debug
	//	puts("*******DEBUG*******");
	//	printMatrix(result);
	//	puts("**************");

	return result;
}



bank* convolveCMM3(bank *kernel, bank *image, int *nsum, int *nmul){

	//il risultato è un banco di tensori
	//bank *result = allocBank(image->h,image->w,kernel->p,image->p);
	bank *result;

	//per ora non generalizzo per tutte le dimensioni, tengo semplice

	//trasformo il banco immagine in una matrice
	matrix *imageMatrix = matrixFromBank_image(image);

	//trasformo il banco kernel in una matrice
	matrix *kernelMatrix = matrixFromBank_kernel(kernel);

	//suppongo quadrate le dimensioni di kernel e image
	matrix *outMatrix = convolveCMM_strassen(kernelMatrix, kernel->h, imageMatrix, image->h,nsum,nmul);

	//debug
	//	puts("*******DEBUG1*******");
	//	printMatrix(outMatrix);
	//	puts("**************");

	/*
	 * operazione inversa di sintesi: bankFromMatrix_out
	 */
	result=bankFromMatrix_out(outMatrix,image->h,image->w,kernel->p,image->p);

	return result;
}




matrix * convolveCMM_strassen(matrix *kernel, int kernelSize, matrix *image, int imageSize,int *nsum,int *nmul){

	matrix *result;

	/*
	 * debug che torna la partizione delle matrici attuali
	 */
	//		puts("*******************debug INTERNO*************:");
	//		puts("kernel:");
	//		printf("kernel->w:%d\n",kernel->w);
	//		printMatrix(kernel);
	//		puts("image:");
	//		printf("image->h:%d\n",image->h);
	//		printMatrix(image);
	//		puts("********************************************:");


	/*
	 * sarebbe più corretto testare il kernel su w, image su h, per come vengono mappati i banchi sulle
	 * matrici; ma ho come PRE che il numero di canali e il paralellismo siano uguali.
	 */
	if(kernel->h == kernelSize && image->h == imageSize){
		//posso svolgere la convoluzione elementare tra elementi BLOCCO, tramite naive border questa implementazione
		result = convolveNaive_border(kernel,image,nsum,nmul);
	}
	else{
		/*
		 * ALTRIMENTI continuo a dividere la complessità,
		 * questa vota finalmente secondo GEOMETRIE STRASSEN!!!
		 */

		matrix *i11,*i12,*i21,*i22;
		int iAlloc=0;//false
		if(image->h > imageSize){
			//alloco 4 nuove strutture di dimensione dimezzata
			i11 = allocSquareMatrix(image->h/2); i12 = allocSquareMatrix(image->h/2);
			i21 = allocSquareMatrix(image->h/2); i22 = allocSquareMatrix(image->h/2);
			iAlloc=1;//true

			//riempio per copia
			copySubMatrix(image,i11,0,0);copySubMatrix(image,i12,0,image->w/2);
			copySubMatrix(image,i21,image->h/2,0);copySubMatrix(image,i22,image->h/2,image->w/2);

			//						puts("*******DEBUG2*******");
			//						printMatrix(i11);printMatrix(i12);printMatrix(i21);printMatrix(i22);
			//						puts("**************");
		}
		else{
			//non corretto ma al momento non mi importa
			i11 = image;i12 = image;i21 = image;i22 = image;
		}

		matrix *k11,*k12,*k21,*k22;
		int kAlloc=0;//false
		if(kernel->w > kernelSize){
			//alloco 4 nuove strutture di dimensione dimezzata
			k11 = allocSquareMatrix(kernel->w/2); k12 = allocSquareMatrix(kernel->w/2);
			k21 = allocSquareMatrix(kernel->w/2); k22 = allocSquareMatrix(kernel->w/2);
			kAlloc=1;//true

			//riempio per copia
			copySubMatrix(kernel,k11,0,0);copySubMatrix(kernel,k12,0,kernel->w/2);
			copySubMatrix(kernel,k21,kernel->h/2,0);copySubMatrix(kernel,k22,kernel->h/2,kernel->w/2);
		}
		else{
			//non corretto ma al momento non mi importa
			k11=kernel;k12=kernel;k21=kernel;k22=kernel;
		}

		//debug
		//		puts("*******************debug INTERNO2*************:");
		//		puts("partizioni kernel:");
		//		printMatrix(k11);printMatrix(k12);printMatrix(k21);printMatrix(k22);
		//		puts("partizioni image:");
		//		printMatrix(i11);printMatrix(i12);printMatrix(i21);printMatrix(i22);
		//		puts("********************************************:");


		/************************** FINO A QUI NON è CAMBIATO NULLA rispetto a convolveCMM_strassen... **************************************/

		//result è calcolata come moltiplicazione convoluzionale a blocchi, BASATA SU STRASSEN!

		//s1: prodotto con strassen, di due operandi somma... (k11+k22)x(i11+i22)
		matrix *tmp1=sumMatrix2(k11,k22,nsum,nmul);
		matrix *tmp2=sumMatrix2(i11,i22,nsum,nmul);
		matrix *s1=convolveCMM_strassen(tmp1,kernelSize,tmp2,imageSize,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//s2 = (k21+k22) *i11
		tmp1=sumMatrix2(k21,k22,nsum,nmul);
		matrix *s2=convolveCMM_strassen(tmp1,kernelSize,i11,imageSize,nsum,nmul);
		freeMatrix(tmp1);

		//s3 = k11 * (i12-i22)
		tmp1=subMatrix2(i12,i22,nsum,nmul);
		matrix *s3=convolveCMM_strassen(k11,kernelSize,tmp1,imageSize,nsum,nmul);
		freeMatrix(tmp1);

		//s4 = k22 * (i21-i11)
		tmp1=subMatrix2(i21,i11,nsum,nmul);
		matrix *s4=convolveCMM_strassen(k22,kernelSize,tmp1,imageSize,nsum,nmul);
		freeMatrix(tmp1);

		//s5 = (k11+k12) * i22
		tmp1=sumMatrix2(k11,k12,nsum,nmul);
		matrix *s5=convolveCMM_strassen(tmp1,kernelSize,i22,imageSize,nsum,nmul);
		freeMatrix(tmp1);

		//s6 = (k21-k11)*(i11+i12)
		tmp1=subMatrix2(k21,k11,nsum,nmul);
		tmp2=sumMatrix2(i11,i12,nsum,nmul);
		matrix *s6=convolveCMM_strassen(tmp1,kernelSize,tmp2,imageSize,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//s7 = (k12-k22)*(i21+i22)
		tmp1=subMatrix2(k12,k22,nsum,nmul);
		tmp2=sumMatrix2(i21,i22,nsum,nmul);
		matrix *s7=convolveCMM_strassen(tmp1,kernelSize,tmp2,imageSize,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//popolo la matrice risultato, 1 elemento alla volta, procedendo per quadranti blocco y11,y12,y21,y22

		//Y11 = s1+s4-s5+s7
		tmp1=sumMatrix2(s1,s4,nsum,nmul);
		tmp2=subMatrix2(tmp1,s5,nsum,nmul);
		matrix *Y11=sumMatrix2(tmp2,s7,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//Y12 = s3+s5
		matrix *Y12=sumMatrix2(s3,s5,nsum,nmul);

		//Y21 = s2+s4
		matrix *Y21=sumMatrix2(s2,s4,nsum,nmul);

		//Y22 = s1-s2+s3+s6
		tmp1=subMatrix2(s1,s2,nsum,nmul);
		tmp2=sumMatrix2(s3,s6,nsum,nmul);
		matrix *Y22=sumMatrix2(tmp1,tmp2,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//popolo result
		result=allocMatrix(image->h,image->w);
		copySubMatrix2(Y11,result,0,0);copySubMatrix2(Y12,result,0,image->w/2);
		copySubMatrix2(Y21,result,image->h/2,0);copySubMatrix2(Y22,result,image->h/2,image->w/2);



		freeMatrix(Y11);freeMatrix(Y12);freeMatrix(Y21);freeMatrix(Y22);
		if(iAlloc){
			freeMatrix(i11);freeMatrix(i12);freeMatrix(i21);freeMatrix(i22);
		}
		if(kAlloc){
			freeMatrix(k11);freeMatrix(k12);freeMatrix(k21);freeMatrix(k22);
		}
	}

	//debug
	//		puts("*******DEBUG*******");
	//		printMatrix(result);
	//		puts("**************");

	return result;
}


bank* convolveCMM4(bank *kernel, bank *image, int *nsum, int *nmul){

	//il risultato è un banco di tensori
	//bank *result = allocBank(image->h,image->w,kernel->p,image->p);
	bank *result;

	//per ora non generalizzo per tutte le dimensioni, tengo semplice

	//trasformo il banco immagine in una matrice
	matrix *imageMatrix = matrixFromBank_image(image);

	//trasformo il banco kernel in una matrice
	matrix *kernelMatrix = matrixFromBank_kernel(kernel);

	//suppongo quadrate le dimensioni di kernel e image
	matrix *outMatrix = convolveCMM_strassen_winograd(kernelMatrix, kernel->h, imageMatrix, image->h,nsum,nmul);

	//debug
	//	puts("*******DEBUG1*******");
	//	printMatrix(outMatrix);
	//	puts("**************");

	/*
	 * operazione inversa di sintesi: bankFromMatrix_out
	 */
	result=bankFromMatrix_out(outMatrix,image->h,image->w,kernel->p,image->p);

	return result;
}



matrix * convolveCMM_strassen_winograd(matrix *kernel, int kernelSize, matrix *image, int imageSize,int *nsum,int *nmul){

	matrix *result;

	/*
	 * debug che torna la partizione delle matrici attuali
	 */
	//		puts("*******************debug INTERNO*************:");
	//		puts("kernel:");
	//		printf("kernel->w:%d\n",kernel->w);
	//		printMatrix(kernel);
	//		puts("image:");
	//		printf("image->h:%d\n",image->h);
	//		printMatrix(image);
	//		puts("********************************************:");


	/*
	 * sarebbe più corretto testare il kernel su w, image su h, per come vengono mappati i banchi sulle
	 * matrici; ma ho come PRE che il numero di canali e il paralellismo siano uguali.
	 */
	if(kernel->h == kernelSize && image->h == imageSize){
		//implemento con soluzione by dario
		result = conv_winograd_2x3_bis(image,kernel,nsum,nmul);
	}
	else{
		/*
		 * ALTRIMENTI continuo a dividere la complessità,
		 * questa vota finalmente secondo GEOMETRIE STRASSEN!!!
		 */

		matrix *i11,*i12,*i21,*i22;
		int iAlloc=0;//false
		if(image->h > imageSize){
			//alloco 4 nuove strutture di dimensione dimezzata
			i11 = allocSquareMatrix(image->h/2); i12 = allocSquareMatrix(image->h/2);
			i21 = allocSquareMatrix(image->h/2); i22 = allocSquareMatrix(image->h/2);
			iAlloc=1;//true

			//riempio per copia
			copySubMatrix(image,i11,0,0);copySubMatrix(image,i12,0,image->w/2);
			copySubMatrix(image,i21,image->h/2,0);copySubMatrix(image,i22,image->h/2,image->w/2);

			//						puts("*******DEBUG2*******");
			//						printMatrix(i11);printMatrix(i12);printMatrix(i21);printMatrix(i22);
			//						puts("**************");
		}
		else{
			//non corretto ma al momento non mi importa
			i11 = image;i12 = image;i21 = image;i22 = image;
		}

		matrix *k11,*k12,*k21,*k22;
		int kAlloc=0;//false
		if(kernel->w > kernelSize){
			//alloco 4 nuove strutture di dimensione dimezzata
			k11 = allocSquareMatrix(kernel->w/2); k12 = allocSquareMatrix(kernel->w/2);
			k21 = allocSquareMatrix(kernel->w/2); k22 = allocSquareMatrix(kernel->w/2);
			kAlloc=1;//true

			//riempio per copia
			copySubMatrix(kernel,k11,0,0);copySubMatrix(kernel,k12,0,kernel->w/2);
			copySubMatrix(kernel,k21,kernel->h/2,0);copySubMatrix(kernel,k22,kernel->h/2,kernel->w/2);
		}
		else{
			//non corretto ma al momento non mi importa
			k11=kernel;k12=kernel;k21=kernel;k22=kernel;
		}

		//debug
		//		puts("*******************debug INTERNO2*************:");
		//		puts("partizioni kernel:");
		//		printMatrix(k11);printMatrix(k12);printMatrix(k21);printMatrix(k22);
		//		puts("partizioni image:");
		//		printMatrix(i11);printMatrix(i12);printMatrix(i21);printMatrix(i22);
		//		puts("********************************************:");


		/************************** FINO A QUI NON è CAMBIATO NULLA rispetto a convolveCMM_strassen... **************************************/

		//result è calcolata come moltiplicazione convoluzionale a blocchi, BASATA SU STRASSEN!

		//s1: prodotto con strassen, di due operandi somma... (k11+k22)x(i11+i22)
		matrix *tmp1=sumMatrix2(k11,k22,nsum,nmul);
		matrix *tmp2=sumMatrix2(i11,i22,nsum,nmul);
		matrix *s1=convolveCMM_strassen_winograd(tmp1,kernelSize,tmp2,imageSize,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//s2 = (k21+k22) *i11
		tmp1=sumMatrix2(k21,k22,nsum,nmul);
		matrix *s2=convolveCMM_strassen_winograd(tmp1,kernelSize,i11,imageSize,nsum,nmul);
		freeMatrix(tmp1);

		//s3 = k11 * (i12-i22)
		tmp1=subMatrix2(i12,i22,nsum,nmul);
		matrix *s3=convolveCMM_strassen_winograd(k11,kernelSize,tmp1,imageSize,nsum,nmul);
		freeMatrix(tmp1);

		//s4 = k22 * (i21-i11)
		tmp1=subMatrix2(i21,i11,nsum,nmul);
		matrix *s4=convolveCMM_strassen_winograd(k22,kernelSize,tmp1,imageSize,nsum,nmul);
		freeMatrix(tmp1);

		//s5 = (k11+k12) * i22
		tmp1=sumMatrix2(k11,k12,nsum,nmul);
		matrix *s5=convolveCMM_strassen_winograd(tmp1,kernelSize,i22,imageSize,nsum,nmul);
		freeMatrix(tmp1);

		//s6 = (k21-k11)*(i11+i12)
		tmp1=subMatrix2(k21,k11,nsum,nmul);
		tmp2=sumMatrix2(i11,i12,nsum,nmul);
		matrix *s6=convolveCMM_strassen_winograd(tmp1,kernelSize,tmp2,imageSize,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//s7 = (k12-k22)*(i21+i22)
		tmp1=subMatrix2(k12,k22,nsum,nmul);
		tmp2=sumMatrix2(i21,i22,nsum,nmul);
		matrix *s7=convolveCMM_strassen_winograd(tmp1,kernelSize,tmp2,imageSize,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//popolo la matrice risultato, 1 elemento alla volta, procedendo per quadranti blocco y11,y12,y21,y22

		//Y11 = s1+s4-s5+s7
		tmp1=sumMatrix2(s1,s4,nsum,nmul);
		tmp2=subMatrix2(tmp1,s5,nsum,nmul);
		matrix *Y11=sumMatrix2(tmp2,s7,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//Y12 = s3+s5
		matrix *Y12=sumMatrix2(s3,s5,nsum,nmul);

		//Y21 = s2+s4
		matrix *Y21=sumMatrix2(s2,s4,nsum,nmul);

		//Y22 = s1-s2+s3+s6
		tmp1=subMatrix2(s1,s2,nsum,nmul);
		tmp2=sumMatrix2(s3,s6,nsum,nmul);
		matrix *Y22=sumMatrix2(tmp1,tmp2,nsum,nmul);
		freeMatrix(tmp1);freeMatrix(tmp2);

		//popolo result
		result=allocMatrix(image->h,image->w);
		copySubMatrix2(Y11,result,0,0);copySubMatrix2(Y12,result,0,image->w/2);
		copySubMatrix2(Y21,result,image->h/2,0);copySubMatrix2(Y22,result,image->h/2,image->w/2);



		freeMatrix(Y11);freeMatrix(Y12);freeMatrix(Y21);freeMatrix(Y22);
		if(iAlloc){
			freeMatrix(i11);freeMatrix(i12);freeMatrix(i21);freeMatrix(i22);
		}
		if(kAlloc){
			freeMatrix(k11);freeMatrix(k12);freeMatrix(k21);freeMatrix(k22);
		}
	}

	//debug
	//		puts("*******DEBUG*******");
	//		printMatrix(result);
	//		puts("**************");

	return result;
}


