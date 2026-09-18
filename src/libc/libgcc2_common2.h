/* longlong.h generico de gcc-2.95 (sin las especializaciones de PowerPC: el
 * objetivo usa umul_ppmm y count_leading_zeros por software) mas la tabla
 * __clz_tab de libgcc2.c.
 *
 * La tabla se direcciona con @sda21 en el objetivo; para que ngcc use esa
 * forma la declaracion `extern const` MIENTE sobre el tamano (solo los objetos
 * de <= 8 B reciben SYMBOL_REF_FLAG) y el almacenamiento real va aparte con
 * nombre de ensamblador. */
#ifndef _LONGLONG_H_
#define _LONGLONG_H_

typedef unsigned char UQItype;

#define SI_TYPE_SIZE 32
#define __BITS4 (SI_TYPE_SIZE / 4)
#define __ll_B ((USItype)1 << (SI_TYPE_SIZE / 2))
#define __ll_lowpart(t) ((USItype)(t) % __ll_B)
#define __ll_highpart(t) ((USItype)(t) / __ll_B)

#define __udiv_qrnnd_c(q, r, n1, n0, d)                                        \
    do {                                                                       \
        USItype __d1, __d0, __q1, __q0;                                        \
        USItype __r1, __r0, __m;                                               \
        __d1 = __ll_highpart(d);                                               \
        __d0 = __ll_lowpart(d);                                                \
                                                                               \
        __r1 = (n1) % __d1;                                                    \
        __q1 = (n1) / __d1;                                                    \
        __m = (USItype)__q1 * __d0;                                            \
        __r1 = __r1 * __ll_B | __ll_highpart(n0);                              \
        if (__r1 < __m) {                                                      \
            __q1--, __r1 += (d);                                               \
            if (__r1 >= (d))                                                   \
                if (__r1 < __m)                                                \
                    __q1--, __r1 += (d);                                       \
        }                                                                      \
        __r1 -= __m;                                                           \
                                                                               \
        __r0 = __r1 % __d1;                                                    \
        __q0 = __r1 / __d1;                                                    \
        __m = (USItype)__q0 * __d0;                                            \
        __r0 = __r0 * __ll_B | __ll_lowpart(n0);                               \
        if (__r0 < __m) {                                                      \
            __q0--, __r0 += (d);                                               \
            if (__r0 >= (d))                                                   \
                if (__r0 < __m)                                                \
                    __q0--, __r0 += (d);                                       \
        }                                                                      \
        __r0 -= __m;                                                           \
                                                                               \
        (q) = (USItype)__q1 * __ll_B | __q0;                                   \
        (r) = __r0;                                                            \
    } while (0)

#define udiv_qrnnd __udiv_qrnnd_c
#define UDIV_NEEDS_NORMALIZATION 1

#define sub_ddmmss(sh, sl, ah, al, bh, bl)                                     \
    do {                                                                       \
        USItype __x;                                                           \
        __x = (al) - (bl);                                                     \
        (sh) = (ah) - (bh) - (__x > (al));                                     \
        (sl) = __x;                                                            \
    } while (0)

#define umul_ppmm(w1, w0, u, v)                                                \
    do {                                                                       \
        USItype __x0, __x1, __x2, __x3;                                        \
        USItype __ul, __vl, __uh, __vh;                                        \
                                                                               \
        __ul = __ll_lowpart(u);                                                \
        __uh = __ll_highpart(u);                                               \
        __vl = __ll_lowpart(v);                                                \
        __vh = __ll_highpart(v);                                               \
                                                                               \
        __x0 = (USItype)__ul * __vl;                                           \
        __x1 = (USItype)__ul * __vh;                                           \
        __x2 = (USItype)__uh * __vl;                                           \
        __x3 = (USItype)__uh * __vh;                                           \
                                                                               \
        __x1 += __ll_highpart(__x0); /* this can't give carry */               \
        __x1 += __x2;                /* but this indeed can */                 \
        if (__x1 < __x2)             /* did we get it? */                      \
            __x3 += __ll_B;          /* yes, add it in the proper pos. */      \
                                                                               \
        (w1) = __x3 + __ll_highpart(__x1);                                     \
        (w0) = __ll_lowpart(__x1) * __ll_B + __ll_lowpart(__x0);               \
    } while (0)

/* Tal cual en el longlong.h de SN (NGC_GNU_SRC/NGC/gcc/longlong.h:1259). */
extern const UQItype __clz_tab[] __attribute__((section(".sdata2")));
#define count_leading_zeros(count, x)                                          \
    do {                                                                       \
        USItype __xr = (x);                                                    \
        USItype __a;                                                           \
                                                                               \
        __a = __xr < ((USItype)1 << 2 * __BITS4)                               \
                  ? (__xr < ((USItype)1 << __BITS4) ? 0 : __BITS4)             \
                  : (__xr < ((USItype)1 << 3 * __BITS4) ? 2 * __BITS4          \
                                                        : 3 * __BITS4);        \
                                                                               \
        (count) = SI_TYPE_SIZE - (__clz_tab[__xr >> __a] + __a);                \
    } while (0)

#endif
