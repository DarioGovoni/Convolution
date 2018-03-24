/*
 * matrix.h
 *
 *  Created on: 07/feb/2018
 *      Author: ale
 *
 *	Tutti i passaggi delle strutture sono realizzati a puntatori per motivi di efficienza.
 *	Funziona con qualsiasi matrice, anche non quadrata
 *  Conteggio in strassen migliorato: non tiene più conto degli zeri
 *  Allocazione e gestione delle strutture migliorate e corrette
 *
 *
 *  Versione con DARIO! ora le matrici non alloggiano piu dei semplici interi, ma dei float! definiti tramite il nuovo
 *  tipo real... non è solo un capriccio: le trasformazioni con winograd fanno uso di divisioni, perdere precisione significa
 *  avere risultati sbagliati
 *
 *  ORA FUNZIONA TUTTO e WINOGRAD ottimizza ulteriormente il lavoro svolto!!! siamo soddisfatti sia io che Dario!!!
 *
 */

#ifndef MATRIX_H_
#define MATRIX_H_


//#define DIM_MATRIX 25
typedef float real;
typedef struct{
	//larghezza e altezza
	int w;
	int h;

	//puntatore a puntatori agli elementi, in una struttura a doppia malloc
	//per ora lavoriamo con soli INTERI
	real **data;
	//l'intera struttura va allocata e gestita con metodi ad hoc

} matrix;


/*
 * fondamentale: la struttura contiene un puntatore che va gestito a malloc e free,
 * non tentare mai di cambiare valori della struttura senza passare per una
 * funzione dedicata
 */
matrix* allocMatrix(int h, int w);

matrix* allocSquareMatrix(int dim);

void reallocMatrix(matrix *m);

void initMatrix(matrix *m);

void freeMatrix(matrix *m);

void printMatrix(matrix *m);


/*
 * SOURCE >> DEST
 * popolo per copia una matrice dest con un sottoinsieme degli elementi di una matrice source
 * PRE: dimensioni di dest entrambe minori delle dimensioni di source
 * Copia che procede in maniera ordinata ovviamente
 * previsti parametri di offset da cui considerare gli elementi di source: i controlli di
 * overflow devono essere fatti esternamente
 */
void copySubMatrix(matrix *source, matrix *dest, int offsetH, int offsetW);

/*
 * DEST >> SOURCE
 * Simile a subCopy. Qui la differenza è che è dest ad essere più grande della sorgente, gli offset sono quindi
 * riferiti alla destinazione in questo caso
 */
void copySubMatrix2(matrix *source, matrix *dest, int offsetH, int offsetW);


/*
 * PRE: suppongo m1 e m2 compatibili per la moltiplicazione,
 * controllo gia effettuato da qualcun altro,
 *
 */
matrix* multiplyMatrix_naive(matrix *m1, matrix *m2, int *nsum, int *nmul);


/*
 * PRE: suppongo m1 e m2 compatibili per la moltiplicazione,
 * controllo gia effettuato da qualcun altro,
 * NOTA: è un algoritmo RICORSIVO, significa dunque che il risultato va allocato prima PER OGNI ITERAZIONE
 *
 * m1 e m2 andrebbero passate per copia? non li devo mica modificare... NO per motivi di efficienza
 * meglio così
 *
 */
matrix* multiplyMatrix_strassen(matrix *m1, matrix *m2, int *nsum, int *nmul);

/*
 * PRE: suppongo m1 e m2 compatibile per la somma,
 * matrice per il risultato gia allocata
 *
 * DA CORREGGERE: non deve essere ritornato un void, non deve essere passato result...
 * INVECE NO: può essere comodo passare il riferimento risultato dall'esterno per questo tipo di operazione,
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
