/* __lshrdi3 -- de gcc-2.95/gcc/libgcc2.c (L_lshrdi3) */
#include "libgcc2_common.h"

DItype __lshrdi3(DItype u, word_type b) {
    DIunion w;
    word_type bm;
    DIunion uu;

    if (b == 0)
        return u;

    uu.ll = u;

    bm = (sizeof(SItype) * BITS_PER_UNIT) - b;
    if (bm <= 0) {
        w.s.high = 0;
        w.s.low = (USItype)uu.s.high >> -bm;
    } else {
        USItype carries = (USItype)uu.s.high << bm;

        w.s.high = (USItype)uu.s.high >> b;
        w.s.low = ((USItype)uu.s.low >> b) | carries;
    }

    return w.ll;
}
