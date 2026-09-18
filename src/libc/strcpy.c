#define UNALIGNED(X, Y) (((long)X & 3) | ((long)Y & 3))
#define DETECTNULL(X) (((X)-0x01010101) & ~(X)&0x80808080)

char *strcpy(char *dst0, const char *src0)
{
    char *dst = dst0;
    const char *src = src0;
    long *aligned_dst;
    const long *aligned_src;

    /* If SRC or DEST is unaligned, then copy bytes.  */
    if (!UNALIGNED(src, dst)) {
        aligned_dst = (long *)dst;
        aligned_src = (long *)src;

        /* SRC and DEST are both "long int" aligned, try to do "long int"
           sized copies.  */
        while (!DETECTNULL(*aligned_src)) {
            *aligned_dst++ = *aligned_src++;
        }

        dst = (char *)aligned_dst;
        src = (char *)aligned_src;
    }

    do {
    } while ((*dst++ = *src++));
    return dst0;
}
