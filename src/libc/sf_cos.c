/* sf_cos.c -- float version of s_cos.c */
/* NOTA: esta unidad solo casa al 100%% con extra_cflags=["-fno-expensive-optimizations"].
 * Con el flag por defecto GCC iza &y[0] a un registro salvado y sobran stmw/mr/lmw.
 * No se puede poner en los cflags de toda la libreria: strcspn.c necesita el contrario. */
#include "fdlibm.h"



float cosf(float x) {
    float y[2], z = 0.0;
    int n, ix;

    GET_FLOAT_WORD(ix, x);

    ix &= 0x7fffffff;
    if (ix <= 0x3f490fd8)
        return __kernel_cosf(x, z);

    else if (ix >= 0x7f800000)
        return x - x;

    else {
        n = __ieee754_rem_pio2f(x, y);
        switch (n & 3) {
        case 0:
            return __kernel_cosf(y[0], y[1]);
        case 1:
            return -__kernel_sinf(y[0], y[1], 1);
        case 2:
            return -__kernel_cosf(y[0], y[1]);
        default:
            return __kernel_sinf(y[0], y[1], 1);
        }
    }
}
