#ifndef STANDARD_KERNELS_H_
#define STANDARD_KERNELS_H_

#include "matrix.h"

fixed zero_data[9] = {
	0,0,0,
	0,0,0,
	0,0,0
};
fixed identity_data[9] = {
	0,0,0,
	0,FIXED_ONE/8,0,
	0,0,0
};
fixed what_edge_data[9] = {
	FIXED_ONE,0,-FIXED_ONE,
	0,0,0,
	-FIXED_ONE,0,FIXED_ONE
};
fixed vert_edge_data[9]={
	-FIXED_HALF/4,0,FIXED_HALF/4,
	-FIXED_ONE/4,0,FIXED_ONE/4,
	-FIXED_HALF/4,0,FIXED_HALF/4
};
fixed vert_edge_rev_data[9]={
	FIXED_HALF/4,0,-FIXED_HALF/4,
	FIXED_ONE/4,0,-FIXED_ONE/4,
	FIXED_HALF/4,0,-FIXED_HALF/4
};
fixed horiz_edge_data[9]={
	-FIXED_HALF/4,-FIXED_ONE/4,-FIXED_HALF/4,
	0,0,0,
	FIXED_HALF/4,FIXED_ONE/4,FIXED_HALF/4
};
fixed horiz_edge_rev_data[9]={
	FIXED_HALF/4,FIXED_ONE/4,FIXED_HALF/4,
	0,0,0,
	-FIXED_HALF/4,-FIXED_ONE/4,-FIXED_HALF/4
};
fixed sharpen_data[9] = {
	0,-FIXED_ONE,0,
	-FIXED_ONE,FIXED_ONE*4+FIXED_HALF/2,-FIXED_ONE,
	0,-FIXED_ONE,0
};
fixed blur_data[9]={
	FIXED_ONE/16,FIXED_ONE/20,FIXED_ONE/16,
	FIXED_ONE/20,FIXED_ONE/16,FIXED_ONE/20,
	FIXED_ONE/16,FIXED_ONE/20,FIXED_ONE/16
};
fixed blur_corner_data[9]={
	0,FIXED_ONE/24,FIXED_ONE/16,
	FIXED_ONE/24,FIXED_ONE/16,FIXED_ONE/8,
	FIXED_ONE/16,FIXED_ONE/8,FIXED_ONE/4
};

matrix zerofilter = {3,3,zero_data};
matrix identity = {3,3,identity_data};
matrix what_edge = {3,3,what_edge_data};
matrix horiz_edge = {3,3,horiz_edge_data};
matrix vert_edge = {3,3,vert_edge_data};
matrix horiz_edge_rev = {3,3,horiz_edge_rev_data};
matrix vert_edge_rev = {3,3,vert_edge_rev_data};
matrix sharpen = {3,3,sharpen_data};
matrix blur = {3,3,blur_data};
matrix blur_corner = {3,3,blur_corner_data};

#endif /* STANDARD_KERNELS_H_ */
