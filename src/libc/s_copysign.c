/* @(#)s_copysign.c 5.1 93/09/24 */
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

typedef union {
    double value;
    struct {
        unsigned int msw;
        unsigned int lsw;
    } parts;
} ieee_double_shape_type;

#define GET_HIGH_WORD(i, d)                                                    \
    do {                                                                       \
        ieee_double_shape_type gh_u;                                           \
        gh_u.value = (d);                                                      \
        (i) = gh_u.parts.msw;                                                  \
    } while (0)

#define SET_HIGH_WORD(d, v)                                                    \
    do {                                                                       \
        ieee_double_shape_type sh_u;                                           \
        sh_u.value = (d);                                                      \
        sh_u.parts.msw = (v);                                                  \
        (d) = sh_u.value;                                                      \
    } while (0)

double copysign(double x, double y) {
    unsigned int hx, hy;
    GET_HIGH_WORD(hx, x);
    GET_HIGH_WORD(hy, y);
    SET_HIGH_WORD(x, (hx & 0x7fffffff) | (hy & 0x80000000));
    return x;
}
