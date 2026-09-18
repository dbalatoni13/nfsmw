/* sf_scalbn.c -- float version of s_scalbn.c */
#include "fdlibm.h"


static const float
two25   =  3.355443200e+07,	/* 0x4c000000 */
twom25  =  2.9802322388e-08,	/* 0x33000000 */
huge   = 1.0e+30,
tiny   = 1.0e-30;

float scalbnf(float x, int n) {
    int k, ix;
    GET_FLOAT_WORD(ix, x);
    k = (ix & 0x7f800000) >> 23; /* extract exponent */
    if (k == 0) {                /* 0 or subnormal x */
        if ((ix & 0x7fffffff) == 0)
            return x; /* +-0 */
        x *= two25;
        GET_FLOAT_WORD(ix, x);
        k = ((ix & 0x7f800000) >> 23) - 25;
        if (n < -50000)
            return tiny * x; /*underflow*/
    }
    if (k == 0xff)
        return x + x; /* NaN or Inf */
    k = k + n;
    if (k > 0xfe)
        return copysignf(huge, x) * huge; /* overflow  */
    if (k > 0)                            /* normal result */
    {
        SET_FLOAT_WORD(x, (ix & 0x807fffff) | (k << 23));
        return x;
    }
    if (k <= -25) {
        if (n > 50000) /* in case integer overflow in n+k */
            return copysignf(huge, x) * huge; /*overflow*/
        else
            return copysignf(tiny, x) * tiny; /*underflow*/
    }
    k += 25; /* subnormal result */
    SET_FLOAT_WORD(x, (ix & 0x807fffff) | (k << 23));
    return x * twom25;
}
