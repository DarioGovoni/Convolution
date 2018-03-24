/*
 * bank.h
 *
 *  Created on: 17/feb/2018
 *      Author: ale
 *
 * Struttura BANCO
 * per generalizzare il concetto di più tensori diversi.
 *
 * Ogni tensore rappresenta 1 immagine o 1 filtro
 * Ogni banco costituisce 1 insieme di tensori, magari rappresentanti più immagini in ingresso
 *
 * banco: insiemi di tensori, banco di tensori
 *
 *
 */

#ifndef BANK_H_
#define BANK_H_

#include "matrix.h"
#include "tensor.h"


/*
 * banco di tensori
 * per esempio più filtri ognuno con più canali
 * per esempio più immagini ognuna con più canali
 * per esempio più output ognuno con più canali
 *
 * l'intera struttura va allocata e gestita con metodi ad hoc
 */
typedef struct{
	/*
	 * informazioni ridondanti che non sarebbero necessarie in quanto ricavabili,
	 * ma che mi ritornano comode in prima battuta. Se cercassi l'ottimizzazione
	 * massima le toglierei
	 */
	int w;//della singola matrice
	int h;//della singola matrice
	int ch;//channels dei tensori
	int p;//grado di parallelismo del bank

	//puntatore a puntatori agli elementi, in una struttura a doppia malloc
	//gli elementi sono struct tensor
	tensor **data;
} bank;


/*
 * fondamentale: la struttura contiene un puntatore che va gestito a malloc e free,
 * non tentare mai di cambiare valori della struttura senza passare per una
 * funzione dedicata
 *
 * il metodo dedicato si occupa di allocare tutti i campi: non solo i puntatori,
 * per una gestione omogenea
 */
bank* allocBank(int h, int w, int ch, int p);

void initBank(bank *b);

void freeBank(bank *b);

void printBank(bank *b);

void reallocBank(bank *b);

/*
 * utilissima: testa se m1 è uguale m2 elemento per elemento
 */
int equals(bank *m1, bank *m2);


#endif /* BANK_H_ */
