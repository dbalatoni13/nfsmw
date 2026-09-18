typedef unsigned long size_t;

size_t strcspn(const char *s1, const char *s2)
{
    const char *start = s1;
    const char *p;

    while (*s1 != '\0') {
        for (p = s2; *p != '\0' && *s1 != *p; p++)
            ;
        if (*p != '\0')
            break;
        s1++;
    }
    return s1 - start;
}
