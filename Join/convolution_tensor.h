/*
 ============================================================================
 Name        : convolution_tensor.h
 Author      : Alessio Onori
 Version     : 1.0
 Copyright   : Your copyright notice
 Created on  : 17/feb/2018
 Description : Prodotto di convoluzione a livello di tensori di matrici
 ============================================================================
 */

#ifndef CONVOLUTION_TENSOR_H_
#define CONVOLUTION_TENSOR_H_

#include "tensor.h"


/*
 * correzzione sul bordo aggiungendo logicamente degli zeri,
 *
 * In: tensore dei canali del kernel, tensore dei canali dell'immagine
 * Out: matrice di output data dal prodotto di convoluzione
 * PRE: immagini->c == kernels->c 	... lo stesso numero di CANALI
 */
matrix* convolveNaive_border_tensor(tensor *kernels, tensor *immagini, int *nsum, int *nmul);




#endif /* CONVOLUTION_TENSOR_H_ */
