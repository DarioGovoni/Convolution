/*
 * CMM.h
 *
 *  Created on: 08/feb/2018
 *      Author: Alessio Onori
 *
 *	Trasformazioni necessarie ad attivare
 *	l'opreazione di CMM = Convolutional Matrix Multiplication
 *		- banco di tensori ==> struttura matrix 2D flat
 *		- 2D matrix ==> banco di tensori
 *
 *	Attualmente, implementazione CMM:
 *		- NAIVE = geometrie dettate dall'algoritmo di Gauss
 *		- con geometrie dettate dall'algoritmo di Strassen
 *			2 possibilità a questo punto:
 *			- implementazione della convoluzione elementare come naive ( soluzione OTTIMA )
 *			- implementazione della convoluzione elementare come Toeplitz + Strassen (inefficiente attualmente)
 *		- funzionanti per banchi aventi ( numero canali == grado di parallelità && multipli di potenze di 2 )
 *
 *	Obiettivo di confrontare un prodotto di convoluzione caso banchi di tensori,
 *	con una nuova rivisitazione chiamata prodotto convoluzionale tra matrici,
 *	al quale applicare le geometrie dell'algoritmo di strassen per ottimizzarla.
 *
 *	L'implemetazione tramite STRASSEN risulta essere la più efficiente, come volevasi dimostrare!!!!!
 *
 */

#ifndef CMM_H_
#define CMM_H_

#include "bank.h"
#include "convolution.h"
#include "conv_winograd_2x3.h"




/*
 * trasformazione da struttura bank a matrix,
 * srotolando i singoli tensori, spalmandoli su 2 dimensioni
 *
 * Nel caso del banco image lo srotolamento è VERTICALE, quindi ogni tensore rappresentante
 * una immagine diventa uno pseudo VETTORE COLONNA. La totalità dei vettori affiancati costituisce la MATRICE 2D DEGLI INGRESSI
 *
 * PRE: per ora suppongo h == w, image composta da sole matrici quadrate ==> non vi è PADDING
 */
matrix* matrixFromBank_image(bank *image);


/*
 * trasformazione da struttura bank a matrix,
 * srotolando i singoli tensori, spalmandoli su 2 dimensioni
 *
 * Nel caso del banco kernel lo srotolamento è ORIZZONTALE, quindi ogni tensore rappresentante
 * un kernel diventa uno pseudo VETTORE RIGA. I vettori, uno sotto l'altro, costituiscono la MATRICE 2D DEI FILTRI
 *
 * PRE: per ora suppongo h == w, kernel composto da sole matrici quadrate ==> non vi è PADDING
 */
matrix* matrixFromBank_kernel(bank *kernel);

/*
 * operazione duale di sintesi per aggregare il risultato finale e restituirlo in forma
 * di BANCO
 *
 * da notare che necessita ovviamente di informazioni aggiuntive sulle dimensioni dei BLOCCHI
 */
bank* bankFromMatrix_out(matrix *out,int ImageH,int imageW,int kernelP,int imageP);



/*
 * è necessario sezionare la matrice in 4 sottomatrici
 *
 * PRE: Le 4 sottomatrici le suppongo per ora perfettamente quadrate e identiche in quanto passata
 * una dimensione consona; tuttavia per il futuro ho già implementato un algoritmo di strassen
 * che applica del padding di 0 per portarsi a tale situazione
 *
 * PRE: le 4 strutture risultato passate come input vanno allocate prima della chiamata della funzione
 *
 * PRE: al momento questa operazione è cablata e pone il vincolo che si debba lavorare su tensori a 2 canali,
 * e il grado di parallelismo del blocco deve essere 2
 */
void split4Matrix(matrix *main, matrix *m11, matrix *m12, matrix *m21, matrix *m22);



//interfaccia di controllo
//chiama al suo interno convolveCMM_naive
bank* convolveCMM(bank *kernel, bank *image, int *nsum, int *nmul);

//chiama al suo interno convolveCMM_naive2
bank* convolveCMM2(bank *kernel, bank *image, int *nsum, int *nmul);

//chiama al suo interno convolveCMM_strassen
bank* convolveCMM3(bank *kernel, bank *image, int *nsum, int *nmul);

//chiama al suo interno convolveCMM_winograd
/*
 * PRE: kernel deve per forza avere dimensione 3x3 (h e w)
 */
bank* convolveCMM4(bank *kernel, bank *image, int *nsum, int *nmul);

/*
 * naive in quanto simula una CMM geometricamente uguale alla riduzione di gauss, quindi algoritmo base
 * Implementazione della singola e vera convoluzione come convolveNaive_border
 *
 * attualemente richiede
 * PRE: dimensioni banchi multiple di potenza di 2 e uguali fra loro
 * PRE: dimensioni matrici quadrate, qualsiasi, anche diverse tra kernel e image
 */
matrix * convolveCMM_naive(matrix *kernel, int kernelSize, matrix *image, int imageSize,int *nsum,int *nmul);

/*
 * naive in quanto simula una CMM geometricamente uguale alla riduzione di gauss, quindi algoritmo base
 * Implementazione della singola e vera convoluzione come convolveByToeplitz_strassen
 *
 * attualemente richiede
 * PRE: dimensioni banchi multiple di potenza di 2 e uguali fra loro
 * PRE: dimensioni matrici quadrate, qualsiasi, anche diverse tra kernel e image
 *
 * ATTUALMENTE INEFFICIENTE
 */
matrix * convolveCMM_naive2(matrix *kernel, int kernelSize, matrix *image, int imageSize,int *nsum,int *nmul);

/*
 * Mi aspetto che sia la versione più efficiente di tutte, ispirato al paper di Cong e Xiao.
 * Le "geometrie" di strassen vengono realizzate ad un livello macroscopico,
 * a levello della singola convoluzione viene implementata con algoritmo naive.
 * Su base di partenza convolveCMM_naive, al momento ne rispetta PRE e POST
 *
 * attualemente richiede
 * PRE: dimensioni banchi multiple di potenza di 2 e uguali fra loro
 * PRE: dimensioni matrici quadrate, qualsiasi, anche diverse tra kernel e image
 *
 * Come volevasi dimostrare è la versione più efficiente!!
 *
 */
matrix * convolveCMM_strassen(matrix *kernel, int kernelSize, matrix *image, int imageSize,int *nsum,int *nmul);



/*
 * Mi aspetto che sia la versione più efficiente di tutte, ispirato al paper di Cong e Xiao.
 * Le "geometrie" di strassen vengono realizzate ad un livello macroscopico,
 * a levello della singola convoluzione viene implementata con algoritmo WINOGRAD BY STRASSEN.
 * Su base di partenza convolveCMM_naive, al momento ne rispetta PRE e POST
 *
 * attualemente richiede
 * PRE: dimensioni banchi multiple di potenza di 2 e uguali fra loro
 * PRE: dimensioni matrici quadrate, qualsiasi, anche diverse tra kernel e image
 *
 * Come volevasi dimostrare è la versione più efficiente!!
 *
 */
matrix * convolveCMM_strassen_winograd(matrix *kernel, int kernelSize, matrix *image, int imageSize,int *nsum,int *nmul);


#endif /* CMM_H_ */
