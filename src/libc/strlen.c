typedef unsigned long size_t;

size_t strlen(const char *s)
{
    const char *p = s;
    unsigned int w;

    if (((unsigned int)s & 3) == 0) {
        w = *(unsigned int *)s;
        w = (w - 0x01010101) & ~w;
        while ((w & 0x80808080) == 0) {
            s += 4;
            w = *(unsigned int *)s;
            w = (w - 0x01010101) & ~w;
        }
    }

    while (*s != '\0')
        s++;

    return s - p;
}
