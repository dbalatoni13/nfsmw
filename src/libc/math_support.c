/* math_support.c -- las cuatro rutinas de fdlibm que el JUEGO compila con su
 * propio toolchain (a diferencia del resto de libm, que viene precompilada de
 * SN).  Se nota en el direccionamiento: aqui el pool de constantes esta en
 * .rodata con `lis @ha` + `lfd @l` (DOS instrucciones), igual que emite este
 * ngcc, mientras que `s_floor.c`/`kf_tan.c` de SN lo tienen en .sdata con
 * `@sda21` (UNA).  `sn_floor` mide 372 B contra los 348 B de `floor`: 24 B
 * exactos = los seis `lis` de mas.  Por eso aqui las constantes van
 * `static const` (se pliegan al pool) y NO con las macros SDA_*.
 *
 * sn_floor  = s_floor.c        (floor)
 * sn_fmod   = e_fmod.c         (__ieee754_fmod)
 * sn_log    = e_log.c          (__ieee754_log)
 * sn_log10  = e_log10.c        (__ieee754_log10)
 */
#include "fdlibm.h"

/* ---------------------------------------------------------------- floor */

static const double floor_huge = 1.0e300;
static const double floor_zero = 0.0;

double sn_floor(double x) {
    int i0, i1, j0;
    unsigned int i, j;
    EXTRACT_WORDS(i0, i1, x);
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    if (j0 < 20) {
        if (j0 < 0) {                       /* raise inexact if x != 0 */
            if (x + floor_huge > floor_zero) { /* return 0*sign(x) if |x|<1 */
                if (i0 >= 0) {
                    i0 = i1 = 0;
                } else if (((i0 & 0x7fffffff) | i1) != 0) {
                    i0 = 0xbff00000;
                    i1 = 0;
                }
            }
        } else {
            i = (0x000fffff) >> j0;
            if (((i0 & i) | i1) == 0)
                return x; /* x is integral */
            if (x + floor_huge > floor_zero) { /* raise inexact flag */
                if (i0 < 0)
                    i0 += (0x00100000) >> j0;
                i0 &= (~i);
                i1 = 0;
            }
        }
    } else if (j0 > 51) {
        if (j0 == 0x400)
            return x + x; /* inf or NaN */
        else
            return x; /* x is integral */
    } else {
        i = ((unsigned int)(0xffffffff)) >> (j0 - 20);
        if ((i1 & i) == 0)
            return x; /* x is integral */
        if (x + floor_huge > floor_zero) { /* raise inexact flag */
            if (i0 < 0) {
                if (j0 == 20)
                    i0 += 1;
                else {
                    j = i1 + (1 << (52 - j0));
                    if (j < i1)
                        i0 += 1; /* got a carry */
                    i1 = j;
                }
            }
            i1 &= (~i);
        }
    }
    INSERT_WORDS(x, i0, i1);
    return x;
}

/* ----------------------------------------------------------------- fmod */

static const double one = 1.0;
static const double Zero[] = {
    0.0,
    -0.0,
};

double sn_fmod(double x, double y) {
    int n, hx, hy, hz, ix, iy, sx, i;
    unsigned int lx, ly, lz;

    EXTRACT_WORDS(hx, lx, x);
    EXTRACT_WORDS(hy, ly, y);
    sx = hx & 0x80000000; /* sign of x */
    hx ^= sx;             /* |x| */
    hy &= 0x7fffffff;     /* |y| */

    /* purge off exception values */
    if ((hy | ly) == 0 || (hx >= 0x7ff00000) ||    /* y=0,or x not finite */
        ((hy | ((ly | -ly) >> 31)) > 0x7ff00000))  /* or y is NaN */
        return (x * y) / (x * y);
    if (hx <= hy) {
        if ((hx < hy) || (lx < ly))
            return x; /* |x|<|y| return x */
        if (lx == ly)
            return Zero[(unsigned int)sx >> 31]; /* |x|=|y| return x*0*/
    }

    /* determine ix = ilogb(x) */
    if (hx < 0x00100000) { /* subnormal x */
        if (hx == 0) {
            for (ix = -1043, i = lx; i > 0; i <<= 1)
                ix -= 1;
        } else {
            for (ix = -1022, i = (hx << 11); i > 0; i <<= 1)
                ix -= 1;
        }
    } else
        ix = (hx >> 20) - 1023;

    /* determine iy = ilogb(y) */
    if (hy < 0x00100000) { /* subnormal y */
        if (hy == 0) {
            for (iy = -1043, i = ly; i > 0; i <<= 1)
                iy -= 1;
        } else {
            for (iy = -1022, i = (hy << 11); i > 0; i <<= 1)
                iy -= 1;
        }
    } else
        iy = (hy >> 20) - 1023;

    /* set up {hx,lx}, {hy,ly} and align y to x */
    if (ix >= -1022)
        hx = 0x00100000 | (0x000fffff & hx);
    else { /* subnormal x, shift x to normal */
        n = -1022 - ix;
        if (n <= 31) {
            hx = (hx << n) | (lx >> (32 - n));
            lx <<= n;
        } else {
            hx = lx << (n - 32);
            lx = 0;
        }
    }
    if (iy >= -1022)
        hy = 0x00100000 | (0x000fffff & hy);
    else { /* subnormal y, shift y to normal */
        n = -1022 - iy;
        if (n <= 31) {
            hy = (hy << n) | (ly >> (32 - n));
            ly <<= n;
        } else {
            hy = ly << (n - 32);
            ly = 0;
        }
    }

    /* fix point fmod */
    n = ix - iy;
    /* El original liga el valor del post-decremento a `i` (muerta aqui): eso
     * es lo que produce el `mr r0,r11` que separa el test del decremento.
     * Con `while (n--)` pelado GCC coalesce la copia y falta esa instruccion. */
    while ((i = n--) != 0) {
        hz = hx - hy;
        lz = lx - ly;
        if (lx < ly)
            hz -= 1;
        if (hz < 0) {
            hx = hx + hx + (lx >> 31);
            lx = lx + lx;
        } else {
            if ((hz | lz) == 0) /* return sign(x)*0 */
                return Zero[(unsigned int)sx >> 31];
            hx = hz + hz + (lz >> 31);
            lx = lz + lz;
        }
    }
    hz = hx - hy;
    lz = lx - ly;
    if (lx < ly)
        hz -= 1;
    if (hz >= 0) {
        hx = hz;
        lx = lz;
    }

    /* convert back to floating value and restore the sign */
    if ((hx | lx) == 0) /* return sign(x)*0 */
        return Zero[(unsigned int)sx >> 31];
    while (hx < 0x00100000) { /* normalize x */
        hx = hx + hx + (lx >> 31);
        lx = lx + lx;
        iy -= 1;
    }
    if (iy >= -1022) { /* normalize output */
        hx = ((hx - 0x00100000) | ((iy + 1023) << 20));
        INSERT_WORDS(x, hx | sx, lx);
    } else { /* subnormal output */
        n = -1022 - iy;
        if (n <= 20) {
            lx = (lx >> n) | ((unsigned int)hx << (32 - n));
            hx >>= n;
        } else if (n <= 31) {
            lx = (hx << (32 - n)) | (lx >> n);
            hx = sx;
        } else {
            lx = hx >> (n - 32);
            hx = sx;
        }
        INSERT_WORDS(x, hx | sx, lx);
        x *= one; /* create necessary signal */
    }
    return x; /* exact output */
}

/* ------------------------------------------------------------------ log */

static const double ln2_hi = 6.93147180369123816490e-01,  /* 3fe62e42 fee00000 */
    ln2_lo = 1.90821492927058770002e-10,                  /* 3dea39ef 35793c76 */
    two54 = 1.80143985094819840000e+16,                   /* 43500000 00000000 */
    Lg1 = 6.666666666666735130e-01,                       /* 3FE55555 55555593 */
    Lg2 = 3.999999999940941908e-01,                       /* 3FD99999 9997FA04 */
    Lg3 = 2.857142874366239149e-01,                       /* 3FD24924 94229359 */
    Lg4 = 2.222219843214978396e-01,                       /* 3FCC71C5 1D8E78AF */
    Lg5 = 1.818357216161805012e-01,                       /* 3FC74664 96CB03DE */
    Lg6 = 1.531383769920937332e-01,                       /* 3FC39A09 D078C69F */
    Lg7 = 1.479819860511658591e-01;                       /* 3FC2F112 DF3E5244 */

static const double zero = 0.0;

double sn_log(double x) {
    double hfsq, f, s, z, R, w, t1, t2, dk;
    int k, hx, i, j;
    unsigned int lx;

    EXTRACT_WORDS(hx, lx, x);

    k = 0;
    if (hx < 0x00100000) {                 /* x < 2**-1022  */
        if (((hx & 0x7fffffff) | lx) == 0)
            return -two54 / zero;          /* log(+-0)=-inf */
        if (hx < 0)
            return (x - x) / zero;         /* log(-#) = NaN */
        k -= 54;
        x *= two54;                        /* subnormal number, scale up x */
        GET_HIGH_WORD(hx, x);
    }
    if (hx >= 0x7ff00000)
        return x + x;
    k += (hx >> 20) - 1023;
    hx &= 0x000fffff;
    i = (hx + 0x95f64) & 0x100000;
    SET_HIGH_WORD(x, hx | (i ^ 0x3ff00000)); /* normalize x or x/2 */
    k += (i >> 20);
    f = x - 1.0;
    if ((0x000fffff & (2 + hx)) < 3) { /* |f| < 2**-20 */
        if (f == zero) {
            if (k == 0)
                return zero;
            else {
                dk = (double)k;
                return dk * ln2_hi + dk * ln2_lo;
            }
        }
        R = f * f * (0.5 - 0.33333333333333333 * f);
        if (k == 0)
            return f - R;
        else {
            dk = (double)k;
            return dk * ln2_hi - ((R - dk * ln2_lo) - f);
        }
    }
    s = f / (2.0 + f);
    dk = (double)k;
    z = s * s;
    i = hx - 0x6147a;
    w = z * z;
    j = 0x6b851 - hx;
    t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
    t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
    i |= j;
    R = t2 + t1;
    if (i > 0) {
        hfsq = 0.5 * f * f;
        if (k == 0)
            return f - (hfsq - s * (hfsq + R));
        else
            return dk * ln2_hi - ((hfsq - (s * (hfsq + R) + dk * ln2_lo)) - f);
    } else {
        if (k == 0)
            return f - s * (f - R);
        else
            return dk * ln2_hi - ((s * (f - R) - dk * ln2_lo) - f);
    }
}

/* ---------------------------------------------------------------- log10 */

static const double two54_10 = 1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
    ivln10 = 4.34294481903251816668e-01,                   /* 0x3FDBCB7B, 0x1526E50E */
    log10_2hi = 3.01029995663611771306e-01,                /* 0x3FD34413, 0x509F6000 */
    log10_2lo = 3.69423907715893090010e-13;                /* 0x3D59FEF3, 0x11F12B36 */

static const double zero_10 = 0.0;

double sn_log10(double x) {
    double y, z;
    int i, k, hx;
    unsigned int lx;

    EXTRACT_WORDS(hx, lx, x);

    k = 0;
    if (hx < 0x00100000) { /* x < 2**-1022  */
        if (((hx & 0x7fffffff) | lx) == 0)
            return -two54_10 / zero_10; /* log(+-0)=-inf */
        if (hx < 0)
            return (x - x) / zero_10;   /* log(-#) = NaN */
        k -= 54;
        x *= two54_10;                  /* subnormal number, scale up x */
        GET_HIGH_WORD(hx, x);
    }
    if (hx >= 0x7ff00000)
        return x + x;
    k += (hx >> 20) - 1023;
    i = ((unsigned int)k & 0x80000000) >> 31;
    hx = (hx & 0x000fffff) | ((0x3ff - i) << 20);
    y = (double)(k + i);
    SET_HIGH_WORD(x, hx);
    z = y * log10_2lo + ivln10 * sn_log(x);
    return z + y * log10_2hi;
}
