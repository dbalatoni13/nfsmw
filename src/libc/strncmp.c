typedef unsigned long size_t;

int strncmp(const char *s1, const char *s2, size_t n)
{
    const char *p1;
    const char *p2;
    unsigned char c1;

    if (n == 0)
        return 0;

    if ((((unsigned int)s1 | (unsigned int)s2) & 3) == 0) {
        p1 = s1;
        p2 = s2;
        if (n > 3 && *(unsigned int *)s1 == *(unsigned int *)s2) {
            do {
                n -= 4;
                if (n == 0)
                    return 0;
                if ((*(unsigned int *)p1 - 0x01010101) & ~*(unsigned int *)p1 & 0x80808080)
                    return 0;
                p1 += 4;
                p2 += 4;
            } while (n > 3 && *(unsigned int *)p1 == *(unsigned int *)p2);
        }
        s1 = p1;
        s2 = p2;
    }

    while (n-- != 0) {
        if ((c1 = *(unsigned char *)s1) != *(unsigned char *)s2)
            break;
        if (n == 0)
            return 0;
        if (c1 == 0)
            return 0;
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}
