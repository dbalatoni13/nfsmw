typedef unsigned int size_t;

#define UNALIGNED(X, Y) (((long)X & 3) | ((long)Y & 3))
#define TOO_SMALL(LEN) ((LEN) < 16)

void *memmove(void *dst_void, const void *src_void, size_t length)
{
    char *dst = dst_void;
    const char *src = src_void;
    long *aligned_dst;
    const long *aligned_src;
    size_t n = length;

    if (src < dst && dst < src + n) {
        /* Destructive overlap...have to copy backwards */
        src += n;
        dst += n;
        while (n--) {
            *--dst = *--src;
        }
    } else {
        /* Use optimizing algorithm for a non-destructive copy to closely
           match memcpy. If the size is small or either SRC or DST is
           unaligned, then punt into the byte copy loop.  This should be
           rare.  */
        if (!TOO_SMALL(length) && !UNALIGNED(src, dst)) {
            aligned_dst = (long *)dst;
            aligned_src = (long *)src;

            /* Copy 4X long words at a time if possible.  */
            while (n >= 16) {
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                *aligned_dst++ = *aligned_src++;
                n -= 16;
            }

            /* Copy one long word at a time if possible.  */
            while (n >= 4) {
                *aligned_dst++ = *aligned_src++;
                n -= 4;
            }

            /* Pick up any residual with a byte copier.  */
            dst = (char *)aligned_dst;
            src = (char *)aligned_src;
        }

        while (n--) {
            *dst++ = *src++;
        }
    }

    return dst_void;
}
