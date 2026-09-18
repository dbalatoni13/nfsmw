/* __floatdisf -- de gcc-2.95/gcc/libgcc2.c (L_floatdisf) */
#include "libgcc2_common.h"

typedef float SFtype;
typedef double DFtype;

#define WORD_SIZE (sizeof(SItype) * BITS_PER_UNIT)
#define HIGH_HALFWORD_COEFF (((UDItype)1) << (WORD_SIZE / 2))
#define HIGH_WORD_COEFF (((UDItype)1) << WORD_SIZE)

#define DI_SIZE (sizeof(DItype) * BITS_PER_UNIT)
#define DF_SIZE 53
#define SF_SIZE 24

SFtype __floatdisf(DItype u) {
    /* Do the calculation in DFmode
       so that we don't lose any of the precision of the high word
       while multiplying it.  */
    DFtype f;

    /* Protect against double-rounding error.
       Represent any low-order bits, that might be truncated in DFmode,
       by a bit that won't be lost.  The bit can go in anywhere below the
       rounding position of the SFmode.  A fixed mask and bit position
       handles all usual configurations.  It doesn't handle the case
       of 128-bit DImode, however.  */
    if (DF_SIZE < DI_SIZE && DF_SIZE > (DI_SIZE - DF_SIZE + SF_SIZE)) {
#define REP_BIT ((UDItype)1 << (DI_SIZE - DF_SIZE))
        if (!(-((DItype)1 << DF_SIZE) < u && u < ((DItype)1 << DF_SIZE))) {
            if ((USItype)u & (REP_BIT - 1)) {
                u |= REP_BIT;
            }
        }
    }
    f = (SItype)(u >> WORD_SIZE);
    f *= HIGH_HALFWORD_COEFF;
    f *= HIGH_HALFWORD_COEFF;
    f += (USItype)(u & (HIGH_WORD_COEFF - 1));

    return (SFtype)f;
}
