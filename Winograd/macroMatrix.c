#include "macroMatrix.h"
#include <stdlib.h>
#include "loop.h"
#include "tensor_idx.h"
#include "matrix_idx.h"

/*void extract_tiles(tensor* src, macroMatrix* dst, int nChannels, int nTiles, int tileDim, int step, int startTileX, int startTileY) {
	//int src_off_x=startTileX*step, src_off_y=startTileY*step;
	register int i,j,c,t;

	REALLOC_MMATRIX(dst,nChannels,nTiles); //in [CxT]: H=nCh, W=nT
	FOR(c,nChannels) {
		FOR(t,nTiles) {
			matrix* ptr = IDX(dst,c,t);
			REALLOC_MATRIX(ptr,tileDim,tileDim);
			FOR(i,tileDim) {
				FOR(j, tileDim) {
					IDX(ptr,i,j)=T_IDX(src,c,src_x+i,src_y+j);
				}
			}
		}
	}
}
*/
