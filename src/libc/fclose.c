#include "local.h"

int fclose(register FILE *fp)
{
    int r;

    if (fp == NULL)
        return (0); /* on NULL */

    CHECK_INIT(fp);

    if (fp->_flags == 0) /* not open! */
        return (0);
    r = fp->_flags & __SWR ? fflush(fp) : 0;
    if (fp->_close != NULL && (*fp->_close)(fp->_cookie) < 0)
        r = EOF;
    fp->_flags = 0; /* release this FILE for reuse */
    return (r);
}
