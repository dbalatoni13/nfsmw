char *strrchr(const char *s, int c)
{
    const char *last = 0;

    c = (char)c;
    while (*s != '\0') {
        if (*s == c)
            last = s;
        s++;
    }
    if (*s == c)
        last = s;
    return (char *)last;
}
