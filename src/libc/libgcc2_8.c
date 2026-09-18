/* __udivdi3 -- de gcc-2.95/gcc/libgcc2.c (L_udivdi3) */
#include "libgcc2_udivmoddi4.h"

UDItype __udivdi3(UDItype n, UDItype d)
{
    return __udivmoddi4(n, d, (UDItype *)0);
}
