#ifndef TENSOR_H_
#define TENSOR_H_

#include "fixed.h"
#include "loop.h"

typedef struct {
	unsigned int h;
	unsigned int w;
	unsigned int ch;
	fixed* data;
} tensor;

#define ALLOC_TENSOR(PTR,H,W,C) PTR=malloc(sizeof(tensor)); PTR->h=H; PTR->w=W; PTR->ch=C; PTR->data=malloc(sizeof(fixed)*H*W*C)
#define FREE_TENSOR(PTR) free(PTR->data); free(PTR)

#endif /* TENSOR_H_ */
