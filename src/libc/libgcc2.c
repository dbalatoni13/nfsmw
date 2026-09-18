/* __ashldi3 -- de gcc-2.95/gcc/libgcc2.c (L_ashldi3) */
#include "libgcc2_common.h"

DItype __ashldi3(DItype u, word_type b) {
    DIunion w;
    word_type bm;
    DIunion uu;

    if (b == 0)
        return u;

    uu.ll = u;

    bm = (sizeof(SItype) * BITS_PER_UNIT) - b;
    if (bm <= 0) {
        w.s.low = 0;
        w.s.high = (USItype)uu.s.low << -bm;
    } else {
        USItype carries = (USItype)uu.s.low >> bm;

        w.s.low = (USItype)uu.s.low << b;
        w.s.high = ((USItype)uu.s.high << b) | carries;
    }

    return w.ll;
}
