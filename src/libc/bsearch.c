typedef unsigned long size_t;

void *bsearch(const void *key, const void *base, size_t nmemb, size_t size,
              int (*compar)(const void *, const void *))
{
    size_t low = 0;
    size_t high = nmemb;
    size_t mid;
    const char *p;
    int c;

    if (nmemb == 0 || size == 0)
        return 0;

    while (low < high) {
        mid = (low + high) / 2;
        p = (const char *)base + mid * size;
        c = compar(key, p);
        if (c < 0)
            high = mid;
        else if (c > 0)
            low = mid + 1;
        else
            return (void *)p;
    }
    return 0;
}
