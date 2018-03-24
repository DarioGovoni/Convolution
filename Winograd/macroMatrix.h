#ifndef MACROMATRIX_H_
#define MACROMATRIX_H_

#include "matrix.h"
#include "tensor.h"

typedef struct {
	unsigned int w;
	unsigned int h;
	matrix** data; //matrix* data[];
} macroMatrix;

#define ALLOC_MMATRIX(PTR,H,W) (PTR)=malloc(sizeof(macroMatrix));(PTR)->h=H;(PTR)->w=W;(PTR)->data=malloc(sizeof(matrix*)*(H)*(W))
#define FREE_MMATRIX(PTR) free(PTR->data); free(PTR)
#define REALLOC_MMATRIX(PTR,H,W) PTR->data=realloc(PTR->data,sizeof(matrix*)*(H)*(W));PTR->h=H;PTR->w=W

//void extract_tiles(tensor* src, macroMatrix* dst, int nChannels, int nTiles, int tileDim, int step, int startTileX, int startTileY);

#endif /* MACROMATRIX_H_ */
