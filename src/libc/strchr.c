#define UNALIGNED(X) ((long)X & 3)
#define DETECTNULL(X) (((X)-0x01010101) & ~(X)&0x80808080)
#define DETECTCHAR(X, MASK) (DETECTNULL(X ^ MASK))

char *strchr(const char *s1, int i)
{
    const unsigned char *s = (const unsigned char *)s1;
    unsigned char c = i;
    unsigned long mask;
    int j;
    unsigned long *aligned_addr;

    if (!UNALIGNED(s)) {
        mask = 0;
        for (j = 0; j < 4; j++)
            mask = (mask << 8) | c;

        aligned_addr = (unsigned long *)s;
        while (!DETECTNULL(*aligned_addr) && !DETECTCHAR(*aligned_addr, mask))
            aligned_addr++;

        /* The block of bytes currently pointed to by aligned_addr
           contains either a null or the target char, or both.  We
           catch it using the bytewise search.  */
        s = (unsigned char *)aligned_addr;
    }

    while (*s && *s != c)
        s++;
    if (*s == c)
        return (char *)s;
    return 0;
}
