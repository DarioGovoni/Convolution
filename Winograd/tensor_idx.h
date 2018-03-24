//convenzione: tensore[canale][riga][colonna]
#define T_IDX(TENSOR_PTR,C,I,J) TENSOR_PTR->data[((C)*((TENSOR_PTR)->h)+(I))*((TENSOR_PTR)->w)+(J)]
