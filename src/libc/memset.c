typedef unsigned int size_t;

#define UNALIGNED(X) ((long)X & 3)
#define TOO_SMALL(LEN) ((LEN) < 4)

void *memset(void *m, int c, size_t n)
{
    char *s = (char *)m;
    int count, i;
    unsigned long buffer;
    unsigned long *aligned_addr;
    unsigned char *unaligned_addr;

    if (!TOO_SMALL(n) && !UNALIGNED(m)) {
        /* If we get this far, we know that n is large and m is word-aligned. */
        aligned_addr = (unsigned long *)m;

        /* Store C into each char sized location in BUFFER so that
           we can set large blocks quickly.  */
        c &= 0xff;
        if (sizeof(long) == 4) {
            buffer = (c << 8) | c;
            buffer |= (buffer << 16);
        } else {
            buffer = 0;
            for (i = 0; i < sizeof(long); i++)
                buffer = (buffer << 8) | c;
        }

        /* Unroll the loop.  */
        while (n >= 16) {
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            *aligned_addr++ = buffer;
            n -= 16;
        }

        while (n >= 4) {
            *aligned_addr++ = buffer;
            n -= 4;
        }
        /* Pick up the remainder with a bytewise loop.  */
        s = (char *)aligned_addr;
    }

    while (n--) {
        *s++ = (char)c;
    }

    return m;
}
