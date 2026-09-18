typedef unsigned int size_t;

#define UNALIGNED(X, Y) (((long)X & 3) | ((long)Y & 3))
#define TOO_SMALL(LEN) ((LEN) < 4)

int memcmp(const void *m1, const void *m2, size_t n)
{
    unsigned char *s1 = (unsigned char *)m1;
    unsigned char *s2 = (unsigned char *)m2;
    unsigned long *a1;
    unsigned long *a2;

    /* If the size of the areas is not too small,
       and both pointers are aligned, use word comparison.  */
    if (!TOO_SMALL(n) && !UNALIGNED(s1, s2)) {
        a1 = (unsigned long *)s1;
        a2 = (unsigned long *)s2;

        while (n >= 4) {
            if (*a1 != *a2)
                break;
            a1++;
            a2++;
            n -= 4;
        }

        /* check m mod 4 remaining characters */
        s1 = (unsigned char *)a1;
        s2 = (unsigned char *)a2;
    }

    for (; n--; s1++, s2++) {
        if (*s1 != *s2)
            return *s1 - *s2;
    }

    return 0;
}
