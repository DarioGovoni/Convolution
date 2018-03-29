/*
 ============================================================================
 Name        : tensor.c
 Author      : Alessio Onori
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : implementazione di tensor.h
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "tensor.h"


tensor * allocTensor(int h, int w, int c){
	tensor *t = (tensor *) malloc ( sizeof(tensor) );
	t->ch=c;
	t->h=h;
	t->w=w;

	t->data = (matrix **) malloc ( c * sizeof(matrix*) );//allocato il primo livello

	int i;
	for(i=0;i<c;i++){
		GET_TENSOR_ELEMENT(t,i) = (matrix*) allocMatrix(h,w);
	}//allocato il secondo livello

	return t;
}

void freeTensor(tensor *t){
	//dealloco prima il livello piu profondo
	int i;
	for(i=0;i<(t->ch);i++){
		freeMatrix(GET_TENSOR_ELEMENT(t,i));
	}

	free(t->data);//dealloco il primo livello

	free(t);//dealloco la struttura intera
}


void initTensor(tensor *t){
	//agisco solo sul livello piu profondo
	int i;
	for(i=0;i<(t->ch);i++){
		initMatrix(GET_TENSOR_ELEMENT(t,i));
	}
}


void printTensor(tensor *t){
	int channel;
	for(channel=0;channel<t->ch;channel++){//scorro canali
		printf("********* CANALE %d ***********\n",channel);
		printMatrix(GET_TENSOR_ELEMENT(t,channel));
	}
}




