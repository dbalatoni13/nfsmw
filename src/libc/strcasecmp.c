extern int _tolower(int);

int strcasecmp(const char *s1, const char *s2)
{
    while (*s1 != '\0' && _tolower(*s1) == _tolower(*s2)) {
        s1++;
        s2++;
    }
    return _tolower(*(unsigned char *)s1) - _tolower(*(unsigned char *)s2);
}
