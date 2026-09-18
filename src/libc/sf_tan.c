/* sf_tan.c -- float version of s_tan.c */
/* NOTA: esta unidad solo casa al 100%% con extra_cflags=["-fno-expensive-optimizations"].
 * Con el flag por defecto GCC iza &y[0] a un registro salvado y sobran stmw/mr/lmw.
 * No se puede poner en los cflags de toda la libreria: strcspn.c necesita el contrario. */
#include "fdlibm.h"



float tanf(float x) {
    float y[2], z = 0.0;
    int n, ix;

    GET_FLOAT_WORD(ix, x);

    ix &= 0x7fffffff;
    if (ix <= 0x3f490fda)
        return __kernel_tanf(x, z, 1);

    else if (ix >= 0x7f800000)
        return x - x; /* NaN */

    else {
        n = __ieee754_rem_pio2f(x, y);
        return __kernel_tanf(y[0], y[1], 1 - ((n & 1) << 1));
    }
}
