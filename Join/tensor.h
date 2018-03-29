/*
 ============================================================================
 Name        : tensor.h
 Author      : Alessio Onori
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : astrazione di tensore di elementi di tipo "matrix"
 Tensore: implementato qui come tanti strati di matrici, appunto i CANALI
 ============================================================================
 */


#ifndef TENSOR_H_
#define TENSOR_H_

#include "matrix.h"

//l'intera struttura va allocata e gestita con metodi ad hoc
typedef struct{
	int w;//larghezza
	int h;//altezza
	int ch;//channels

	//puntatore a puntatori agli elementi, in una struttura a doppia malloc
	//gli elementi sono struct matrix
	matrix **data;
} tensor;

/*
 * MACRO per accedere comodamente agli elementi della struttura
 *
 * se non dovesse piacere, è possibile cambiare il nome di tutte le occorrenze della macro
 * con un singolo colpo: refactor->rename di eclipse
 *
 * ELEMENT: singolo canale, singola matrice
 * REAL: valore numerico
 */
#define GET_TENSOR_ELEMENT(TENSOR_PTR,CH) TENSOR_PTR->data[CH]
#define GET_TENSOR_REAL(TENSOR_PTR,CH,I,J) GET_MATRIX_REAL(GET_TENSOR_ELEMENT(TENSOR_PTR,CH),I,J)

/*
 *
 * GESTIONE DELLA STRUTTURA
 * fondamentale: la struttura contiene un puntatore che va gestito a malloc e free,
 * non tentare mai di cambiare valori della struttura senza passare per una
 * funzione dedicata
 *
 * il metodo dedicato si occupa di allocare tutti i campi: non solo i puntatori,
 * per una gestione omogenea
 *
 */

tensor* allocTensor(int h, int w, int c);

void initTensor(tensor *t);

void freeTensor(tensor *t);

void printTensor(tensor *t);





#endif /* TENSOR_H_ */
