#include "conv.h"
#include "bmp.h"
#include "tensor_select.h"
#include <stdio.h>
#include <stdlib.h>
#include "standard_kernels.h"

#define path_in "/Users/Dario/Desktop/winograd/V5-tests/test_sm64.bmp"

#define path_fb_lum "/Users/Dario/Desktop/winograd/V5-tests/test_sm64_lum.bmp"
#define path_fb_r "/Users/Dario/Desktop/winograd/V5-tests/test_sm64_r.bmp"
#define path_fb_g "/Users/Dario/Desktop/winograd/V5-tests/test_sm64_g.bmp"
#define path_fb_b "/Users/Dario/Desktop/winograd/V5-tests/test_sm64_b.bmp"

#define path_out_N "/Users/Dario/Desktop/winograd/V5-tests/test_sm64_N.bmp"
#define path_out_W23 "/Users/Dario/Desktop/winograd/V5-tests/test_sm64_W23.bmp"
#define path_out_W33 "/Users/Dario/Desktop/winograd/V5-tests/test_sm64_W33.bmp"

#define path_out_dedge "/Users/Dario/Desktop/winograd/V5-tests/multi/test_sm64_dedge.bmp"
#define path_out_hedge "/Users/Dario/Desktop/winograd/V5-tests/multi/test_sm64_hedge.bmp"
#define path_out_sharp "/Users/Dario/Desktop/winograd/V5-tests/multi/test_sm64_sharp.bmp"
#define path_out_blur "/Users/Dario/Desktop/winograd/V5-tests/multi/test_sm64_blur.bmp"

void test_single() {
	//matrix* in = readImg(path_in,0);
	tensor* in_t = readImgLRGB(path_in);
	matrix np_in = /*{
			in_t->w,
			in_t->h,
			(in_t->data + ((in_t->h)*(in_t->w))*3)
	}*/SELECT_TENSOR_CHANNEL(in_t,3);
	matrix in_r = SELECT_TENSOR_CHANNEL(in_t,0);
	matrix in_g = SELECT_TENSOR_CHANNEL(in_t,1);
	matrix in_b = SELECT_TENSOR_CHANNEL(in_t,2);
	matrix* in = &np_in;
	matrix* out;
	real fdata[9]={
			-FIXED_HALF,0,FIXED_HALF,
			-FIXED_ONE,0,FIXED_ONE,
			-FIXED_HALF,0,FIXED_HALF
	};
	matrix np_fil = {
			R,
			R,
			fdata
	};
	matrix* fil = &np_fil;

	//writeImgGrayscale(path_fb_lum, in, 0, 1,1,1);
	//writeImgGrayscale(path_fb_r, &in_r, 0, 1,0,0);
	//writeImgGrayscale(path_fb_g, &in_g, 0, 0,1,0);
	//writeImgGrayscale(path_fb_b, &in_b, 0, 0,0,1);

	ALLOC_MATRIX(out,in->h,in->w);

	conv_single_naive(in,fil,out);
	printf("Naive elapsed: %d (%lu - %lu)\n",conv_elapsed, end, begin);
	writeImgRB(path_out_N, out, 0);

	conv_single(in,fil,out);
	printf("Winograd elapsed: %d (%lu - %lu)\n",conv_elapsed, end, begin);
	writeImgRB(path_out_W23, out, 0);
}

void test_multi() {
	tensor* in_t = readImgLRGB(path_in);
	tensor*out_t;
	matrix* filters[16] = {
		&what_edge,&vert_edge,&what_edge,&vert_edge_rev,
		&horiz_edge,&vert_edge,&horiz_edge,&vert_edge,
		&zerofilter,&zerofilter,&zerofilter,&sharpen,
		&blur_corner,&blur_corner,&blur,&zerofilter};
	macroMatrix filterMatrix = {
		K,
		C,
		filters
	};
	ALLOC_TENSOR(out_t,in_t->h,in_t->w,K);
	printf("Start conv\n");
	conv_full(in_t, &filterMatrix, out_t);
	printf("Winograd elapsed: %d (%lu - %lu)\n",conv_elapsed, end, begin);
	{
		matrix out_whatEdge = SELECT_TENSOR_CHANNEL(out_t,0);
		matrix out_horizEdge = SELECT_TENSOR_CHANNEL(out_t,1);
		matrix out_sharpen = SELECT_TENSOR_CHANNEL(out_t,2);
		matrix out_blur = SELECT_TENSOR_CHANNEL(out_t,3);

		//writeImgGrayscale(path_out_dedge, &out_whatEdge, 0, 0,0,1);
		//writeImgGrayscale(path_out_hedge, &out_horizEdge, 0, 1,0,0);
		//writeImgGrayscale(path_out_sharp, &out_sharpen, 0, 0,1,0);
		//writeImgGrayscale(path_out_blur, &out_blur, 0, 1,1,1);
	}
}

int main() {
	test_single();
}
