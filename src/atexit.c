/* GOWE69 callback registration, reconstructed from the original 152-byte body.
 * Uses the existing reentrancy header and the original libc compilation flags.
 */
#include <stdlib.h>

int atexit(void (*function)(void)) {
    struct _atexit *table = _impure_ptr->_atexit;
    if (table == NULL) {
        table = &_impure_ptr->_atexit0;
        _impure_ptr->_atexit = table;
    }
    if (table->_ind >= _ATEXIT_SIZE) {
        table = (struct _atexit *)malloc(sizeof(struct _atexit));
        if (table == NULL)
            return -1;
        table->_ind = 0;
        table->_next = _impure_ptr->_atexit;
        _impure_ptr->_atexit = table;
    }
    table->_fns[table->_ind++] = function;
    return 0;
}
