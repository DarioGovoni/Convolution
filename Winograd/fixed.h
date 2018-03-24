#include <stdint.h>

typedef int64_t fixed;
#define FRACT_BITS 16
#define HALF_FRACT_BITS 8
#define FIXED_ONE (1<<FRACT_BITS)
#define FIXED_HALF (1<<(FRACT_BITS-1))
#define INT_2_FIXED(x) (fixed)((x)<<FRACT_BITS)
#define FLOAT_2_FIXED(x) ((int)((x)*FIXED_ONE))
#define FIXED_2_INT(x) ((x)>>FRACT_BITS)
#define FIXED_2_FLOAT(x) (((float)(x)) / FIXED_ONE)
#define FIXED_MUL(x,y) (((x)>>HALF_FRACT_BITS)*((y)>>HALF_FRACT_BITS))

#define FIXED_DIV(x,y) ((((int64_t)(x))<<32)/y)
#define FIXED_INV(x) ((((int64_t)(1))<<32)/x)
