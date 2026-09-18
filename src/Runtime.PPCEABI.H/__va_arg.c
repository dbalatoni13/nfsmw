/* EABI va_list: the original counters are signed bytes (lbz/extsb).
 * Kept local to this runtime TU; no shared SDK header is changed.
 * r39: 244 B exact with MW 1.2.5n, including branches and pointer updates.
 * See docs/analisis/r39-runtime.md for compiler/ABI evidence and link tests.
 */
typedef struct {
    signed char gpr;
    signed char fpr;
    char reserved[2];
    char *overflow;
    char *saved;
} SNVaList;

void *__va_arg(SNVaList *list, unsigned char type) {
    char *result;
    signed char *counter = &list->gpr;
    int index = list->gpr;
    int limit = 8;
    int size = 4;
    int step = 1;
    int skip = 0;
    int base = 0;
    int stride = 4;

    if (type == 4) {
        result = (char *)(((unsigned int)list->overflow + 15) & ~15u);
        list->overflow = result + 16;
        return result;
    }
    if (type == 3) {
        counter = &list->fpr;
        index = list->fpr;
        size = 8;
        base = 32;
        stride = 8;
    }
    if (type == 2) {
        size = 8;
        --limit;
        if (index & 1)
            skip = 1;
        step = 2;
    }
    if (index < limit) {
        index += skip;
        result = list->saved + base + (index * stride);
        *counter = index + step;
    } else {
        *counter = 8;
        result = list->overflow;
        result = (char *)(((unsigned int)result + (size - 1)) & ~(size - 1));
        list->overflow = result + size;
    }
    if (type == 0)
        result = *(char **)result;
    return result;
}
