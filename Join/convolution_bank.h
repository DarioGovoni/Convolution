/*
 ============================================================================
 Name        : convolution_bank.h
 Author      : Alessio Onori
 Version     : 1.0
 Copyright   : Your copyright notice
 Created on  : 17/feb/2018
 Description : Prodotto di convoluzione a livello di banchi di tensori
 ============================================================================
 */


#ifndef CONVOLUTION_BANK_H_
#define CONVOLUTION_BANK_H_

#include "bank.h"


/*
 * correzzione sul bordo aggiungendo logicamente degli zeri,
 *
 * In: banco dei tensori kernel, banco dei tensori immagine
 * Out: banco di output dato dal prodotto di convoluzione
 * PRE: immagini->ch == kernels->ch 	... lo stesso numero di CANALI
 */
bank* convolveNaive_border_bank(bank *kernel, bank *immage, int *nsum, int *nmul);




#endif /* CONVOLUTION_BANK_H_ */
