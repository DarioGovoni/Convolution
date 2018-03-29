/*
 ============================================================================
 Name        : bank.c
 Author      : Alessio Onori
 Version     : 1.0
 Copyright   : Your copyright notice
 Created on	 : 17/feb/2018
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

	b->data = (tensor **) malloc ( p * sizeof(tensor*) );//allocato il primo livello: puntatore a piu tensori

	int i;
	for(i=0;i<p;i++){
		b->data[i] = (tensor*) allocTensor(h,w,ch);
	}//allocato il secondo livello

	return b;
}

void freeBank(bank *b){
	//dealloco prima il livello piu profondo
	int i;
	for(i=0;i<(b->p);i++){
		//possibile in quanto ho definito bene la free dei tensor
		freeTensor(b->data[i]);
	}

	free(b->data);//dealloco il primo livello

	free(b);//dealloco la struttura intera
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
		printTensor(GET_BANK_ELEMENT(b,pElement));
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
					//if( m1->data[p]->data[ch]->data[i][j] != m2->data[p]->data[ch]->data[i][j] )
					if(GET_BANK_REAL(m1,p,ch,i,j) != GET_BANK_REAL(m2,p,ch,i,j))
						return 0;//0 = false
				}
			}
		}
	}

	return 1;//1 = true
}



