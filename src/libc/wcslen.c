typedef unsigned int size_t;
typedef unsigned int wchar_t;

size_t wcslen(const wchar_t *s)
{
    const wchar_t *p = s;

    while (*p != 0)
        p++;

    return p - s;
}
