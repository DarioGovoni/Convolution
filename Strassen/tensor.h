/*
 * tensor.h
 *
 *  Created on: 17/feb/2018
 *      Author: ale
 *
 *	Strutture Tensore per generalizzare il concetto di CNN a più canali e a più input
 *
 *	Tensore: implementato qui come tanti strati di matrici, appunto i CANALI
 *
 */



#ifndef TENSOR_H_
#define TENSOR_H_

#include "matrix.h"


typedef struct{
	//larghezza e altezza
	int w;
	int h;
	int ch;//channels
	//puntatore a puntatori agli elementi, in una struttura a doppia malloc
	//gli elementi sono struct matrix
	matrix **data;
	//questa struttura va allocata e gestita con metodi ad hoc

} tensor;


/*
 * fondamentale: la struttura contiene un puntatore che va gestito a malloc e free,
 * non tentare mai di cambiare valori della struttura senza passare per una
 * funzione dedicata
 */
tensor* allocTensor(int h, int w, int c);

void reallocTensor(tensor *t);

void initTensor(tensor *t);

void freeTensor(tensor *t);

void printTensor(tensor *t);





#endif /* TENSOR_H_ */
