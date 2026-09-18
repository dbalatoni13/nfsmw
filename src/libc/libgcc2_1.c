/* __ashrdi3 -- de gcc-2.95/gcc/libgcc2.c (L_ashrdi3) */
#include "libgcc2_common.h"

DItype __ashrdi3(DItype u, word_type b) {
    DIunion w;
    word_type bm;
    DIunion uu;

    if (b == 0)
        return u;

    uu.ll = u;

    bm = (sizeof(SItype) * BITS_PER_UNIT) - b;
    if (bm <= 0) {
        /* w.s.high = 1..1 or 0..0 */
        w.s.high = uu.s.high >> (sizeof(SItype) * BITS_PER_UNIT - 1);
        w.s.low = uu.s.high >> -bm;
    } else {
        USItype carries = (USItype)uu.s.high << bm;

        w.s.high = uu.s.high >> b;
        w.s.low = ((USItype)uu.s.low >> b) | carries;
    }

    return w.ll;
}
