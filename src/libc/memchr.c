typedef unsigned int size_t;

#define UNALIGNED(X) ((long)X & 3)
#define TOO_SMALL(LEN) ((LEN) < 4)
#define DETECTNULL(X) (((X)-0x01010101) & ~(X)&0x80808080)
#define DETECTCHAR(X, MASK) (DETECTNULL(X ^ MASK))

void *memchr(const void *src_void, int c, size_t length)
{
    const unsigned char *src = (const unsigned char *)src_void;
    unsigned char d = c;
    unsigned long *asrc;
    unsigned long mask;
    const unsigned char *p;
    int i;

    if (!TOO_SMALL(length) && !UNALIGNED(src)) {
        /* If we get this far, we know that length is large and src is
           word-aligned. */
        asrc = (unsigned long *)src;
        mask = 0;
        for (i = 0; i < 4; i++)
            mask = (mask << 8) + d;

        while (length >= 4) {
            if (DETECTCHAR(*asrc, mask)) {
                p = (const unsigned char *)asrc;
                for (i = 0; i < 4; i++) {
                    if (*p == d)
                        return (void *)p;
                    p++;
                }
            }
            length -= 4;
            asrc++;
        }

        src = (unsigned char *)asrc;
    }

    while (length--) {
        if (*src == d)
            return (void *)src;
        src++;
    }

    return 0;
}
