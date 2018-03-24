#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "matrix.h"
#include "tensor.h"

typedef unsigned char u8;

static real r_lum = FLOAT_2_FIXED(0.2126);
static real g_lum = FLOAT_2_FIXED(0.7152);
static real b_lum = FLOAT_2_FIXED(0.0722);

void int_to_char(int n, u8* buf);

void readBmpHeader(int fd, int *w, int *h);
void writeBmpHeader(int fd, int w, int h);

matrix* readImg(char* path, int shift);
tensor* readImgLRGB(char* path);
 void writeImgGrayscale(char* path, matrix* m, int shift, int r_mul, int g_mul, int b_mul);
 void writeImgRB(char* path, matrix* m, int shift);
 void writeImgFromArray(char* path, float* values, int h, int w, int shift);
