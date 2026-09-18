typedef unsigned int size_t;

#define UNALIGNED(X, Y) (((long)X & 3) | ((long)Y & 3))
#define TOO_SMALL(LEN) ((LEN) < 16)

void *memcpy(void *dst0, const void *src0, size_t len0)
{
    char *dst = (char *)dst0;
    const char *src = (const char *)src0;
    long *aligned_dst;
    const long *aligned_src;

    /* If the size is small, or either SRC or DST is unaligned,
       then punt into the byte copy loop.  This should be rare.  */
    if (!TOO_SMALL(len0) && !UNALIGNED(src, dst)) {
        aligned_dst = (long *)dst;
        aligned_src = (long *)src;

        /* Copy 4X long words at a time if possible.  */
        while (len0 >= 16) {
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            len0 -= 16;
        }

        /* Copy one long word at a time if possible.  */
        while (len0 >= 4) {
            *aligned_dst++ = *aligned_src++;
            len0 -= 4;
        }

        /* Pick up any residual with a byte copier.  */
        dst = (char *)aligned_dst;
        src = (char *)aligned_src;
    }

    while (len0--)
        *dst++ = *src++;

    return dst0;
}
