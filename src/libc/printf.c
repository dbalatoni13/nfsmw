#include "local.h"
#include <stdarg.h>

int printf(const char *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    _REENT->_stdout->_data = _REENT;
    ret = vfprintf(_REENT->_stdout, fmt, ap);
    va_end(ap);
    return ret;
}
