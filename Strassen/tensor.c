/*
 ============================================================================
 Name        : tensor.c
 Author      : Alessio Onori
 Version     :
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

	t->data = (matrix **) malloc ( c * sizeof(matrix*) );
	int i;
	for(i=0;i<c;i++){
		t->data[i] = (matrix*) allocMatrix(h,w);
	}

	return t;
}

void freeTensor(tensor *t){
	//dealloco prima il livello piu profondo
	int i;
	for(i=0;i<(t->ch);i++){
		freeMatrix(t->data[i]);
	}
	//poi il primo livello
	free(t->data);
	//poi la struttura intera
	free(t);
}



void reallocTensor(tensor *t){
	int h = t->h;
	int w = t->w;
	int c = t->ch;

	if(t){
		freeTensor(t);
	}

	allocTensor(h,w,c);
}

void initTensor(tensor *t){
	//dealloco solo il livello piu profondo
	int i;
	for(i=0;i<(t->ch);i++){
		initMatrix(t->data[i]);
	}
}


void printTensor(tensor *t){
	int channel;
	for(channel=0;channel<t->ch;channel++){//scorro canali
		printf("********* CANALE %d ***********\n",channel);
		printMatrix(t->data[channel]);
	}
}




