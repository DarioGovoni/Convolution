/*
 ============================================================================
 Name        : bank.c
 Author      : Alessio Onori
 Version     :
 Copyright   : Your copyright notice
 Description : implementazione di bank.h
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include "bank.h"



bank * allocBank(int h, int w, int ch, int p){
	bank *b = (bank *) malloc ( sizeof(bank) );
	b->ch=ch;
	b->h=h;
	b->w=w;
	b->p=p;

	/*
	 * primo livello di malloc: puntatore a più tensori
	 */
	b->data = (tensor **) malloc ( p * sizeof(tensor*) );
	int i;
	for(i=0;i<p;i++){
		/*
		 * secondo livello di malloc: puntatore a struct tensore
		 */
		b->data[i] = (tensor*) allocTensor(h,w,ch);
	}

	return b;
}

void freeBank(bank *b){
	//dealloco prima il secondo livello
	int i;
	for(i=0;i<(b->p);i++){
		//possibile in quanto ho definito bene la free dei tensor
		freeTensor(b->data[i]);
	}
	//dealloco il primo livello
	free(b->data);
	//poi la struttura intera
	free(b);
}



void reallocBank(bank *b){
	int h = b->h;
	int w = b->w;
	int ch = b->ch;
	int p = b->p;


	if(b){
		freeBank(b);
	}

	allocBank(h,w,ch,p);
}

void initBank(bank *b){
	//dealloco solo il livello piu profondo
	int i;
	for(i=0;i<(b->p);i++){
		initTensor(b->data[i]);
	}
}


void printBank(bank *b){
	int pElement;
	for(pElement=0;pElement<b->p;pElement++){//scorro i tensori paralleli
		printf("*************************************************************\n");
		printf("********* ELEMENTO PARALLELO %d del BANCO ***********\n",pElement);
		printf("*************************************************************\n");
		printTensor(b->data[pElement]);
	}
}



int equals(bank *m1, bank *m2){

	if(m1->ch != m2->ch || m1->h != m2->h || m1->p != m2->p || m1->w != m2->w)
		return 0;//0 = false

	int p;
	for(p=0;p<m1->p;p++){
		int ch;
		for (ch=0;ch<m1->ch;ch++){
			int i;
			for(i=0;i<m1->h;i++){
				int j;
				for(j=0;j<m1->w;j++){
					if( m1->data[p]->data[ch]->data[i][j] != m2->data[p]->data[ch]->data[i][j] )
						return 0;//0 = false
				}
			}
		}
	}

	return 1;//1 = true
}



