typedef unsigned int size_t;
typedef unsigned int wchar_t;

/* Sin prototipo: el objetivo emite `crclr cr1eq` antes del `bl`, que es lo que
 * GCC hace cuando el llamado puede ser varargs (declaracion sin argumentos). */
extern void *memcpy();
size_t wcslen(const wchar_t *s);

wchar_t *wcscpy(wchar_t *dst, const wchar_t *src)
{
    size_t len = wcslen(src);

    memcpy(dst, src, len * sizeof(wchar_t));
    dst[len] = 0;
    return dst;
}
