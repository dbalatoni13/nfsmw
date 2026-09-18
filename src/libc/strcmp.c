#define UNALIGNED(X, Y) (((long)X & 3) | ((long)Y & 3))
#define DETECTNULL(X) (((X)-0x01010101) & ~(X)&0x80808080)

int strcmp(const char *s1, const char *s2)
{
    unsigned long *a1;
    unsigned long *a2;

    /* If s1 or s2 are unaligned, then compare bytes. */
    if (!UNALIGNED(s1, s2)) {
        /* If s1 and s2 are word-aligned, compare them a word at a time. */
        a1 = (unsigned long *)s1;
        a2 = (unsigned long *)s2;
        while (*a1 == *a2) {
            /* To get here, *a1 == *a2, thus if we find a null in *a1,
               then the strings must be equal, so return zero.  */
            if (DETECTNULL(*a1))
                return 0;

            a1++;
            a2++;
        }

        /* A difference was detected in last few bytes of s1, so search
           bytewise */
        s1 = (char *)a1;
        s2 = (char *)a2;
    }

    while (*s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (*(unsigned char *)s1) - (*(unsigned char *)s2);
}
