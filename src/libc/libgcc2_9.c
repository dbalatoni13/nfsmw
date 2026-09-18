/* __umoddi3 -- de gcc-2.95/gcc/libgcc2.c (L_umoddi3) */
#include "libgcc2_udivmoddi4.h"

UDItype __umoddi3(UDItype u, UDItype v)
{
    UDItype w;

    (void)__udivmoddi4(u, v, &w);

    return w;
}
