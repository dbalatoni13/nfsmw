char *strstr(const char *s1, const char *s2)
{
    if (*s1 == 0) {
        if (*s2)
            return (char *)0;
        return (char *)s1;
    }

    while (*s1) {
        unsigned long i;
        i = 0;

        while (1) {
            if (s2[i] == 0) {
                return (char *)s1;
            }

            if (s2[i] != s1[i]) {
                break;
            }
            i++;
        }
        s1++;
    }

    return (char *)0;
}
