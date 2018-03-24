#include "bmp.h"
#include "loop.h"
#include "matrix_idx.h"
#include "tensor_idx.h"
#include <stdlib.h>

typedef unsigned char u8;

void int_to_char(int n, u8* buf) {
	register int i;
	FOR(i,4) {
		int mask = 255 << (8*i);
		buf[i] = (n & mask) >> (8*i);
	}
}

void readBmpHeader(int fd, int *w, int *h) {
	register int i;
	u8 buffer[28];

	read(fd,buffer,18);
	(*w)=0;
	FOR(i,4) {
		u8 c;
		read(fd,&c,1);
		*w |= (c)<<(8*i);
	}
	//printf("read w: %d\n",(*w));
	(*h)=0;
	FOR(i,4) {
		u8 c;
		read(fd,&c,1);
		*h |= (c)<<(8*i);
	}
	//printf("read h: %d\n",(*h));
	read(fd,buffer,28);
}
void writeBmpHeader(int fd, int w, int h) {
	u8 BM[2] = {'B','M'};
	u8 size_char[4];
	u8 rsv_off_H2[12] = {0,0,0,0,0x36,0,0,0,0x28,0,0,0};
	u8 w_char[4];
	u8 h_char[4];
	u8 color_inf[8] = {1,0,0x18,0,0,0,0,0};
	u8 relevant_size_char[4];
	u8 header_end[16] = {0x13, 0xB,0,0,0x13, 0xB,0,0, 0,0,0,0, 0,0,0,0};
	register int line_size = w*3;
	int line_size_padded = line_size + ((line_size%4)?3 - line_size%4:0);
	int relevant_size = line_size_padded*h;
	int size = relevant_size+54;

	int_to_char(size,size_char);
	int_to_char(w,w_char);
	int_to_char(h,h_char);
	int_to_char(relevant_size,relevant_size_char);

	write(fd,BM,2);
	write(fd,size_char,4);
	write(fd,rsv_off_H2,12);
	write(fd,w_char,4);
	write(fd,h_char,4);
	write(fd,color_inf,8);
	write(fd,relevant_size_char,4);
	write(fd,header_end,16);
}

matrix* readImg(char* path, int shift) {
	register int i,j;
	int fd = open(path, O_RDONLY);
	char tmp;
	matrix*m;
	int w,h;

	readBmpHeader(fd,&w,&h);
	/*m = malloc(sizeof(matrix));
	m->w=w;
	m->h=h;
	allocMatrix(m);*/
	//m=reallocMatrix(m_empty);
	ALLOC_MATRIX(m,h,w);
	FOR(i,m->h) {
		//int rd = w*3;
		FOR(j,m->w) {
			u8 r,g,b;
			real lum;
			read(fd,&b,1);
			read(fd,&g,1);
			read(fd,&r,1);
			//convert to Luma
			lum = ((r*r_lum) + (g*g_lum) + (b*b_lum))>>8;
			//m->data[i * m->w + j] = lum;
			IDX(m,i,j) = (lum - shift);
		}
		//every line must contain a multiple of 4 bytes (padding with 0x00s)
		j*=3;
		while (j%4) {
			read(fd,&tmp,1);
			j++;
		}
	}
	close(fd);
	printf("%s read and stored\n",path);
	//printf("(size: %d x %d)\n",w,h);
	//printf("(size: %d x %d)\n",m->w,m->h);
	return m;
}

/*void fillLRGB(fixed data[][4], u8 r, u8 g, u8 b, int i, int j, int w) {
	fixed lum = ((r*r_lum) + (g*g_lum) + (b*b_lum));
	data[i*w+j][0] = lum/256;
	data[i*w+j][1] = (INT_2_FIXED(r))/256;
	data[i*w+j][2] = (INT_2_FIXED(g))/256;
	data[i*w+j][3] = (INT_2_FIXED(b))/256;
}*/

void fillLRGB(tensor* t, u8 r, u8 g, u8 b, int i, int j) {
	fixed lum = ((r*r_lum) + (g*g_lum) + (b*b_lum));
	T_IDX(t,0,i,j) = (INT_2_FIXED(r))/256;
	T_IDX(t,1,i,j) = (INT_2_FIXED(g))/256;
	T_IDX(t,2,i,j) = (INT_2_FIXED(b))/256;
	T_IDX(t,3,i,j) = lum/256;
	//IMPORTANTE!!!
	//nel tensore, i canali sono isolati, non interleaved
	//per puntare a un solo canale, si punta a {t->data + (t->h*t->w)*canale}
}

tensor* readImgLRGB(char* path) {
	register int i,j;
	int fd = open(path, O_RDONLY);
	char tmp;
	tensor*t;
	int w,h;

	readBmpHeader(fd,&w,&h);
	ALLOC_TENSOR(t,h,w,4);
	FOR(i,h) {
		FOR(j,w) {
			u8 r,g,b;
			//fixed lum;
			//fixed data[][4] = t->data;

			read(fd,&b,1);
			read(fd,&g,1);
			read(fd,&r,1);
			//convert to Luma
			//lum = ((r*r_lum) + (g*g_lum) + (b*b_lum));

			//data[i*w+j][0] = lum/256;
			//data[i*w+j][1] = (INT_2_FIXED(r))/256;
			//data[i*w+j][2] = (INT_2_FIXED(g))/256;
			//data[i*w+j][3] = (INT_2_FIXED(b))/256;
			//fillLRGB((void*)t->data,r,g,b,i,j,w);
			fillLRGB(t,r,g,b,i,j);
		}
		//every line must contain a multiple of 4 bytes (padding with 0x00s)
		j*=3;
		while (j%4) {
			read(fd,&tmp,1);
			j++;
		}
	}
	close(fd);
	printf("%s read and stored\n",path);
	return t;
}

void writeImgGrayscale(char* path, matrix* m, int shift, int r_mul, int g_mul, int b_mul) {
	register int i,j;
	u8 zero = 0;
	int fd = open(path, O_WRONLY|O_CREAT|O_TRUNC,0666);

	int all_unset=(!(r_mul || g_mul || b_mul));
	//if all_unset, remap lum from [-1,1] to [0,1]
	int all_set=(r_mul && g_mul && b_mul);
	//if all_set, output |lum|; otherwise output pos/neg values w/ opposite colors

	writeBmpHeader(fd,m->w,m->h);
	FOR(i,m->h) {
		FOR(j,m->w) {
			//real lum = m->data[i*m->w + j];
			real lum = IDX(m,i,j) + shift;
			//lum = lum<0?0:lum;
			lum = lum>=FIXED_ONE?(FIXED_ONE-1):lum;
			lum = lum<=(-FIXED_ONE)?(1-FIXED_ONE):lum;
			if (all_unset)
				lum = lum/2 + FIXED_HALF;
			if (lum>=0 || all_set) {
				if (lum<0) lum=-lum;
				u8 gray = (lum>>(FRACT_BITS-8)) & 0xFF;
				if (b_mul) write(fd,&gray,1); else write(fd,&zero,1);
				if (g_mul) write(fd,&gray,1); else write(fd,&zero,1);
				if (r_mul) write(fd,&gray,1); else write(fd,&zero,1);
			} else {
				u8 gray = ((-lum)>>(FRACT_BITS-8)) & 0xFF;
				if (!b_mul) write(fd,&gray,1); else write(fd,&zero,1);
				if (!g_mul) write(fd,&gray,1); else write(fd,&zero,1);
				if (!r_mul) write(fd,&gray,1); else write(fd,&zero,1);
			}
		}
		//every line must contain a multiple of 4 bytes (padding with 0x00s)
		j*=3;
		while (j%4) {
			write(fd,&zero,1);
			j++;
		}
	}
	close(fd);
	printf("stored image written to %s\n",path);
}
void writeImgRB(char* path, matrix* m, int shift) {
	register int i,j;
	u8 zero = 0;
	int fd = open(path, O_WRONLY|O_CREAT|O_TRUNC,0666);
	writeBmpHeader(fd,m->w,m->h);
	FOR(i,m->h) {
		FOR(j,m->w) {
			//real lum = m->data[i*m->w + j];
			real lum = IDX(m,i,j) + shift;
			//lum = lum<0?0:lum;
			lum = lum>=FIXED_ONE?(FIXED_ONE-1):lum;
			lum = lum<=(-FIXED_ONE)?(1-FIXED_ONE):lum;
			if (lum>=0) {
				u8 gray = (lum>>(FRACT_BITS-8)) & 0xFF;
				write(fd,&zero,1);//b
				write(fd,&zero,1);//g
				write(fd,&gray,1);//r
			} else {
				u8 gray = ((-lum)>>(FRACT_BITS-8)) & 0xFF;
				write(fd,&gray,1);//b
				write(fd,&zero,1);//g
				write(fd,&zero,1);//r
			}
		}
		//every line must contain a multiple of 4 bytes (padding with 0x00s)
		j*=3;
		while (j%4) {
			write(fd,&zero,1);
			j++;
		}
	}
	close(fd);
	printf("stored image written to %s\n",path);
}

void writeImgFromArray(char* path, float* values, int h, int w, int shift) {
	register int i,j;
	u8 zero = 0;
	int fd = open(path, O_WRONLY|O_CREAT|O_TRUNC);
	writeBmpHeader(fd,w,h);
	FOR(i,h) {
		FOR(j,w) {
			float lum = values[i*w + j] + shift;
			lum = lum<0.f?0.f:lum;
			u8 gray = (lum>=1)? 255: lum * 256;
			write(fd,&gray,1);
			write(fd,&gray,1);
			write(fd,&gray,1);
		}
		//every line must contain a multiple of 4 bytes (padding with 0x00s)
		j*=3;
		while (j%4) {
			write(fd,&zero,1);
			j++;
		}
	}
	close(fd);
	printf("array written to %s\n",path);
}
