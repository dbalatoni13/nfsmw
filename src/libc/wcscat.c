typedef unsigned int size_t;
typedef unsigned int wchar_t;

/* Sin prototipo: el objetivo emite `crclr cr1eq` antes del `bl`, que es lo que
 * GCC hace cuando el llamado puede ser varargs (declaracion sin argumentos). */
extern void *memcpy();
size_t wcslen(const wchar_t *s);

wchar_t *wcscat(wchar_t *dst, const wchar_t *src)
{
    size_t dlen = wcslen(dst);
    size_t slen = wcslen(src);
    /* Keep the byte offset as the first operand of the original address add. */
    wchar_t *p = (wchar_t *)(dlen * sizeof(wchar_t) + (unsigned int)dst);

    memcpy(p, src, slen * sizeof(wchar_t));
    p[slen] = 0;
    return dst;
}
