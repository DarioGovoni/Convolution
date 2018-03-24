/*
 *
 * versione che propone correzione sugli indici
 * delle matrici, implementata in conv_winograd_2x3_bis
 *
 */
#include "matrix.h"

void conv_winograd_2x3(matrix* in, matrix* filter, matrix* out,int *nsum, int *nmul);

/*
 * creazione interna della matrice risultato, di dimensioni pari alle dimensioni della
 * matrice di input immagine in questo caso
 */
matrix* conv_winograd_2x3_bis(matrix* in, matrix* filter,int *nsum, int *nmul);
