/*
 ============================================================================
 Name        : matrix.h
 Author      : Alessio Onori
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : astrazione di matrice di elementi di tipo "real"
 ============================================================================
 */


#ifndef MATRIX_H_
#define MATRIX_H_

#include "real.h"

//l'intera struttura va allocata e gestita con metodi ad hoc
typedef struct{
	int w;//larghezza
	int h;//altezza

	//puntatore a puntatori i quali puntantano gli elementi, in una struttura a doppia malloc
	real* data;
} matrix;

/*
 * MACRO per accedere comodamente agli elementi della struttura
 *
 * se non dovesse piacere, è possibile cambiare il nome di tutte le occorrenze della macro
 * con un singolo colpo: refactor->rename di eclipse
 *
 * REAL: singolo valore numerico
 */
#define GET_MATRIX_REAL(MATRIX_PTR,I,J) MATRIX_PTR->data[(I)*((MATRIX_PTR)->w)+(J)]

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

matrix* allocMatrix(int h, int w);

matrix* allocSquareMatrix(int dim);

void initMatrix(matrix *m);

void freeMatrix(matrix *m);

void printMatrix(matrix *m);


/*
 *
 * METODI
 * Tutti i passaggi delle strutture sono realizzati a puntatori in un'ottica di efficienza.
 *
 */

/*
 * SOURCE >> DEST
 * popolo per copia una matrice dest con un sottoinsieme degli elementi di una matrice source
 *
 * PRE: dimensioni di dest entrambe minori delle dimensioni di source
 *
 * Copia che procede in maniera ordinata ovviamente: da sx verso dx, da alto verso il basso
 *
 * previsti parametri di offset da cui considerare gli elementi di source: i controlli di
 * overflow devono essere fatti esternamente
 */
void copySubMatrix(matrix *source, matrix *dest, int offsetH, int offsetW);

/*
 * DEST >> SOURCE
 * Simile a copySubMatrix. Qui la differenza è che è dest ad essere più grande della sorgente, gli offset sono quindi
 * riferiti alla destinazione in questo caso
 */
void copySubMatrix2(matrix *source, matrix *dest, int offsetH, int offsetW);


/*
 * PRE: suppongo m1 e m2 compatibili per la moltiplicazione,
 * quindi controllo gia effettuato da qualcun altro,
 *
 */
matrix* multiplyMatrix_naive(matrix *m1, matrix *m2, int *nsum, int *nmul);

/*
 * PRE: suppongo m1 e m2 compatibili per la moltiplicazione,
 * controllo gia effettuato da qualcun altro,
 *
 * NOTA: è un algoritmo RICORSIVO, significa dunque che il risultato va allocato prima PER OGNI ITERAZIONE
 * (la funzione alloca una matrice ad ogni chiamata intrinsecamente)
 *
 * concettualmente m1 e m2 andrebbero passate per copia in quanto non li devo modificare, MA per motivi di efficienza
 * meglio passare per riferimento
 *
 */
matrix* multiplyMatrix_strassen(matrix *m1, matrix *m2, int *nsum, int *nmul);

/*
 * PRE: suppongo m1 e m2 compatibili per la somma,
 * matrice per il risultato già allocata
 *
 * può essere comodo passare il riferimento risultato dall'esterno per questo tipo di operazione,
 * magari quando ho molte operazioni da fare in successione, defininedo così un risultato temporaneo inizializzato
 * tra un operazione e l'altra
 */
void sumMatrix(matrix *result, matrix *m1, matrix *m2, int *nsum, int *nmul);
void subMatrix(matrix *result, matrix *m1, matrix *m2, int *nsum, int *nmul);

/*
 * versione che ritorna il puntatore ad una matrice creata internamente
 * alla funzione
 */
matrix* sumMatrix2(matrix *m1, matrix *m2, int *nsum, int *nmul);
matrix* subMatrix2(matrix *m1, matrix *m2, int *nsum, int *nmul);

/*
 * Nel caso la matrice necessiti di padding viene allocata una nuova matrice delle
 * dimensioni corrette. Il puntatore alla vecchia matrice viene ridiretto alla nuova: salvare
 * il puntatore vecchio se vi è bisogno di deallocare la struttura old, questa funzione volontariamente non lo fa!
 *
 * Ritorna se è stato applicato del padding oppure no.
 * 0 false
 * 1 true
 */
int padMatrixToDim(matrix **m,int hTarget, int wTarget);



#endif /* MATRIX_H_ */
