/* sf_log10.c -- float version of e_log10.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/* Fuente fdlibm LIMPIA, sin barreras ni pines.  SOLO casa compilada con
 * -msafe-sda (configure.py) y con el rango .sdata 0x804FF158..0x804FF180
 * asignado a esta unidad en splits.txt: el sesgo int->double de
 * `(float)(k + i)` sale del pool anonimo del compilador, y ese pool solo va a
 * .sdata con @sda21 bajo TARGET_SAFE_SDATA (rs6000.c:6160).  Ver
 * docs/analisis/r66-libc-flags.md. */

#include "fdlibm.h"

extern float logf(float x);

static const float
two25     =  3.3554432000e+07, /* 0x4c000000 */
ivln10    =  4.3429449201e-01, /* 0x3ede5bd9 */
log10_2hi =  3.0102920532e-01, /* 0x3e9a2080 */
log10_2lo =  7.9034151668e-07; /* 0x355427db */

static const float zero = 0.0;

float log10f(float x)
{
    float y, z;
    int i, k, hx;

    GET_FLOAT_WORD(hx, x);

    k = 0;
    if (hx < 0x00800000) { /* x < 2**-126  */
        if ((hx & 0x7fffffff) == 0)
            return -two25 / zero; /* log(+-0)=-inf */
        if (hx < 0)
            return (x - x) / zero; /* log(-#) = NaN */
        k -= 25;
        x *= two25; /* subnormal number, scale up x */
        GET_FLOAT_WORD(hx, x);
    }
    if (hx >= 0x7f800000)
        return x + x;
    k += (hx >> 23) - 127;
    i = ((unsigned int)k & 0x80000000) >> 31;
    hx = (hx & 0x007fffff) | ((0x7f - i) << 23);
    y = (float)(k + i);
    SET_FLOAT_WORD(x, hx);
    z = y * log10_2lo + logf(x) * ivln10;
    return z + y * log10_2hi;
}
