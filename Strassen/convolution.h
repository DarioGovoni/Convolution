/*
 * convolution.h
 *
 *  Created on: 08/feb/2018
 *      Author: Alessio Onori
 *
 *	Obiettivo di mappare un prodotto di convoluzione tra 2 matrici 2D
 *	in un prodotto di 2 matrici, così da attivare una possibile ottimizzazione di tale prodotto magari
 *	con Strassen o Winograd
 *	Algoritmo: 2D linear convolution using doubly block Toeplitz matrices
 *
 */

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

#include "matrix.h"

/*
 * calcola le dimensioni della convuluzione fra le 2 matrici
 * date in ingresso
 */
void convolutionSize(matrix *m1,matrix *m2, int *h, int *w);


/*
 * dimensioni kernel < dimensioni immagine
 * dimensioni kernel dispari, matrice quadrata
 *
 * in realtà sembra funzionare anche senza queste prime 2 congetture
 *
 * POST:dimensioni risultato = dimensioni immagine
 * stride kernel = 1
 */
matrix* convolveNaive(matrix *kernel, matrix *immagine);

/*
 * correzzione sul bordo aggiungendo logicamente degli zeri,
 * in realtà non viene definita una nuova matrice bordata di zeri, bensì gestiti banalmente gli overflow
 */
matrix* convolveNaive_border(matrix *kernel, matrix *immagine, int *nsum, int *nmul);

/*
 * m1 è la matrice da trasformare con Toeplitz, m2 è quella che verra srotolata
 * ovviamente si ricorda che la CONVOLUZIONE gode di proprietà commutativa
 *
 * ritorna il riferiemento del risultato
 */
matrix* convolveByToeplitz_naive(matrix *m1, matrix *m2, int *nsum, int *nmul);
matrix* convolveByToeplitz_strassen(matrix *m1, matrix *m2, int *nsum, int *nmul);





#endif /* CONVOLUTION_H_ */
