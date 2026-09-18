extern int _tolower(int);

int strncasecmp(const char *s1, const char *s2, unsigned long n)
{
    if (n == 0)
        return 0;

    while (n-- != 0) {
        if (_tolower(*s1) != _tolower(*s2))
            break;
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }
    return _tolower(*(unsigned char *)s1) - _tolower(*(unsigned char *)s2);
}
