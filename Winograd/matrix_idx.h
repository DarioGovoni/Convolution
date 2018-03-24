
/*
 * restituisce un boolean
 * se indice è dentro la matrice
 */
#define IDX_INSIDE(MATRIX_PTR,I,J) ((I>=0) && (I<((MATRIX_PTR)->h)) && (J>=0) && (J<((MATRIX_PTR)->w)))
#define IDX_INSIDE_OFF(MATRIX_PTR,OFF_H,OFF_W,I,J) IDX_INSIDE(MATRIX_PTR,OFF_H+I,OFF_W+J)


#define IDX(MATRIX_PTR,I,J) MATRIX_PTR->data[(I)*((MATRIX_PTR)->w)+(J)]

/*
 * SAFE: 0 se indice è esterno, altrimenti restituisce il valore puntato
 * assume i valori fuori dalla matrice siano tutti zeri
 * macro utile
 */
#define IDX_SAFE(MATRIX_PTR,I,J) (((I<0) || (I>=((MATRIX_PTR)->h)) || (J<0) || (J>=((MATRIX_PTR)->w)))?0:IDX(MATRIX_PTR,I,J))
#define IDX_OFFSET(MATRIX_PTR,OFF_H,OFF_W,I,J) IDX(MATRIX_PTR,OFF_H+I,OFF_W+J)
#define IDX_OFFSET_SAFE(MATRIX_PTR,OFF_H,OFF_W,I,J) IDX_SAFE(MATRIX_PTR,OFF_H+I,OFF_W+J)
#define IDX_OFFSET_SAFE_WRITE(MATRIX_PTR,OFF_H,OFF_W,I,J) if(IDX_INSIDE_OFF(MATRIX_PTR,OFF_H,OFF_W,I,J)) IDX_OFFSET(MATRIX_PTR,OFF_H,OFF_W,I,J)
//spiegazione in più:
//IDX_OFFSET_SAFE è un valore numerico, da usare alla destra di un'assegnazione.
	//uso corretto: {variabile = IDX_OFFSET_SAFE(...);} espande in {variabile = (IDX_INSIDE(...)?a[...]:0);}
	//uso errato: {IDX_OFFSET_SAFE(...) = numero;} espande in {(IDX_INSIDE(...)?a[...]:0) = numero;} errore di compilazione (l'op. ternario non è un lvalue)
//IDX_OFFSET_SAFE_WRITE va usato come variabile vuota, alla sinistra di un =.
	//uso corretto: {IDX_SAFE_WRITE(...) = numero;} espande in {if(IDX_INSIDE(...))  a[...]=numero;}
	//uso errato: {variabile = IDX_SAFE_WRITE(...);} espande in {variabile = if(IDX_INSIDE(...)) a[...];} errore di compilazione.

#define IDX_T(MATRIX_PTR,J,I) MATRIX_PTR->data[(I)*((MATRIX_PTR)->w)+(J)]
#define IDX_T_SAFE(MATRIX_PTR,J,I) (I<0 || I>=((MATRIX_PTR)->h) || J<0 || J>=((MATRIX_PTR)->w))?0:IDX(MATRIX_PTR,I,J)
#define IDX_T_OFFSET(MATRIX_PTR,OFF_W,OFF_H,J,I) IDX(MATRIX_PTR,OFF_H+I,OFF_W+J)
#define IDX_T_OFFSET_SAFE(MATRIX_PTR,OFF_W,OFF_H,J,I) IDX_SAFE(MATRIX_PTR,OFF_H+I,OFF_W+J)
