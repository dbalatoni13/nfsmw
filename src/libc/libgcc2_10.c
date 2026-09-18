/* Principio del bloque L_eh de libgcc2.c de SN (NGC_GNU_SRC/NGC/gcc/libgcc2.c).
 * Del bloque solo sobreviven en el DOL __default_terminate y __terminate: el
 * resto lo descarta el enlazador, pero la cadena de aviso de new_eh_context se
 * queda en .rodata (0x8040FE40), y por eso el fichero llega hasta ella.
 * -G0 (configure.py): __terminate_func mide 4 B y el original la tiene en .data
 * y la direcciona con @ha/@l. */
#include <stdlib.h>
#include <string.h>

/* Shared exception handling support routines.  */

extern void __default_terminate(void) __attribute__((__noreturn__));

void __default_terminate() {
    abort();
}

void (*__terminate_func)() = __default_terminate;

void __terminate() {
    (*__terminate_func)();
}

void *__throw_type_match(void *catch_type, void *throw_type, void *obj) {
    if (strcmp((const char *)catch_type, (const char *)throw_type) == 0)
        return obj;
    return 0;
}

void __empty() {
}

/* Include definitions of EH context and table layout (eh-common.h) */

struct eh_context {
    void *handler_label;
    void **dynamic_handler_chain;
    /* This is language dependent part of the eh context. */
    void *info;
    /* This is used to remember where we threw for re-throws */
    void *table_index; /* address of exception table entry to rethrow from */
};

/* Allocate and return a new EH context structure. */

extern void __throw();

static void *new_eh_context() {
    struct eh_full_context {
        struct eh_context c;
        void *top_elt[2];
    } *ehfc;

    /* SN-Dave: check for _register_malloc */
    if (_register_malloc == NULL) {
        OSPanic(0, 0, "\n*** Library error ***\nException handling requires access to heap memory\nHook _register_malloc to your malloc rountine - see ProDG manual.\n");
    }

    ehfc = (struct eh_full_context *)(*_register_malloc)(sizeof *ehfc);

    if (!ehfc)
        __terminate();

    memset(ehfc, 0, sizeof *ehfc);

    ehfc->c.dynamic_handler_chain = (void **)ehfc->top_elt;

    /* This should optimize out entirely.  This should always be true,
       but just in case it ever isn't, don't allow bogus code to be
       generated.  */

    if ((void *)(&ehfc->c) != (void *)ehfc)
        __terminate();

    return &ehfc->c;
}
