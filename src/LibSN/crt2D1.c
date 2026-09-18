/* Global-constructor startup (libgcc2.c, L__main) from the GOWE69 runtime body. */
typedef void (*constructor_function)(void);
extern constructor_function __CTOR_LIST__[];

void __do_global_ctors(void) {
    do {
        unsigned long nptrs = (unsigned long)__CTOR_LIST__[0];
        unsigned i;
        if (nptrs == (unsigned long)-1)
            for (nptrs = 0; __CTOR_LIST__[nptrs + 1] != 0; nptrs++);
        for (i = nptrs; i >= 1; i--)
            __CTOR_LIST__[i]();
    } while (0);
}

void __main(void) {
    /* Support recursive calls to `main': run initializers just once.  */
    static int initialized;
    if (!initialized) {
        initialized = 1;
        __do_global_ctors();
    }
}
