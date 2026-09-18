typedef unsigned int size_t;

#define ALIGNED(X) (((long)X & 3) == 0)
#define DETECTNULL(X) (((X)-0x01010101) & ~(X)&0x80808080)

char *strncat(char *s1, const char *s2, size_t n)
{
    char *s = s1;

    /* Skip over the data in s1 as quickly as possible.  */
    if (ALIGNED(s1)) {
        unsigned long *aligned_s1 = (unsigned long *)s1;
        while (!DETECTNULL(*aligned_s1))
            aligned_s1++;

        s1 = (char *)aligned_s1;
    }

    while (*s1)
        s1++;

    while (n-- != 0) {
        if ((*s1++ = *s2++) == '\0')
            break;
        if (n == 0)
            *s1 = '\0';
    }

    return s;
}
