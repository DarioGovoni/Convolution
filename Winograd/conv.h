
#ifndef CONV_H_
#define CONV_H_

#include "matrix.h"
#include "tensor.h"
#include "macroMatrix.h"

#include "conv_params.h"

void conv_single_naive(matrix* in, matrix* filter, matrix* out);
void conv_single(matrix* in, matrix* filter, matrix* out);
void conv_full(tensor* in, macroMatrix* filterbank, tensor* out);

int conv_elapsed;
unsigned long begin, end;

#endif /* CONV_H_ */
