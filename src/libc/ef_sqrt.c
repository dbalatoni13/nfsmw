/* ef_sqrt.c -- float version of e_sqrt.c */
#include "fdlibm.h"

float sqrtf(float x) {
    float z;
    int sign = (int)0x80000000;
    int ix, s, q, m, t, i;
    unsigned int r;

    GET_FLOAT_WORD(ix, x);

    /* take care of Inf and NaN */
    if ((ix & 0x7f800000) == 0x7f800000) {
        return x * x + x; /* sqrt(NaN)=NaN, sqrt(+inf)=+inf, sqrt(-inf)=sNaN */
    }
    /* take care of zero */
    if (ix <= 0) {
        if ((ix & (~sign)) == 0)
            return x; /* sqrt(+-0) = +-0 */
        else if (ix < 0)
            return (x - x) / (x - x); /* sqrt(-ve) = sNaN */
    }
    /* normalize x */
    m = (ix >> 23);
    if (m == 0) { /* subnormal x */
        for (i = 0; (ix & 0x00800000) == 0; i++)
            ix <<= 1;
        m -= i - 1;
    }
    m -= 127; /* unbias exponent */
    ix = (ix & 0x007fffff) | 0x00800000;
    if (m & 1) /* odd m, double x to make it even */
        ix += ix;
    m >>= 1; /* m = [m/2] */

    /* generate sqrt(x) bit by bit */
    ix += ix;
    q = s = 0;          /* q = sqrt(x) */
    r = 0x01000000;     /* r = moving bit from right to left */

    while (r != 0) {
        t = s + r;
        if (t <= ix) {
            s = t + r;
            ix -= t;
            q += r;
        }
        ix += ix;
        r >>= 1;
    }

    /* use floating add to find out rounding direction */
    if (ix != 0) {
        q += (q & 1);
    }
    ix = (q >> 1) + 0x3f000000;
    ix += (m << 23);
    SET_FLOAT_WORD(z, ix);
    return z;
}
