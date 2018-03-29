/*
 ============================================================================
 Name        : bank.h
 Author      : Alessio Onori
 Version     : 1.0
 Copyright   : Your copyright notice
 Created on	 : 17/feb/2018
 Description : astrazione di banco di elementi di tipo "tensor"
 Ogni tensore rappresenta 1 immagine o 1 filtro
 Ogni banco costituisce 1 insieme di tensori, magari rappresentanti più immagini in ingresso
 banco: insiemi di tensori, banco di tensori
 per esempio più filtri ognuno con più canali
 per esempio più immagini ognuna con più canali
 per esempio più output ognuno con più canali
 ============================================================================
 */


#ifndef BANK_H_
#define BANK_H_

#include "matrix.h"
#include "tensor.h"


//l'intera struttura va allocata e gestita con metodi ad hoc
typedef struct{
	/*
	 * informazioni ridondanti che non sarebbero necessarie in quanto ricavabili,
	 * ma che mi ritornano comode in prima battuta. Se cercassi l'ottimizzazione
	 * massima le toglierei
	 */
	int w;//della singola matrice
	int h;//della singola matrice
	int ch;//channels dei tensori
	int p;//grado di parallelismo del bank, numero di elementi

	//puntatore a puntatori i quali puntantano gli elementi, in una struttura a doppia malloc
	tensor **data;
} bank;

/*
 * MACRO per accedere comodamente agli elementi della struttura
 *
 * se non dovesse piacere, è possibile cambiare il nome di tutte le occorrenze della macro
 * con un singolo colpo: refactor->rename di eclipse
 *
 * ELEMENT: singolo tensore
 * ECHANNEL: singolo canale del tensore, matrice dunque
 * REAL: valore numerico
 */
#define GET_BANK_ELEMENT(BANK_PTR,P) BANK_PTR->data[P]
#define GET_BANK_ECHANNEL(BANK_PTR,P,CH) GET_TENSOR_ELEMENT( GET_BANK_ELEMENT(BANK_PTR,P), CH )
#define GET_BANK_REAL(BANK_PTR,P,CH,I,J) GET_MATRIX_REAL( GET_BANK_ECHANNEL(BANK_PTR,P,CH), I, J )


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

bank* allocBank(int h, int w, int ch, int p);

void initBank(bank *b);

void freeBank(bank *b);

void printBank(bank *b);

/*
 * utilissima: testa se m1 è uguale m2 elemento per elemento
 */
int equals(bank *m1, bank *m2);


#endif /* BANK_H_ */
