#include "local.h"

static int lflush(FILE *fp)
{
    return fflush(fp);
}

int __srefill(register FILE *fp)
{
    /* make sure stdio is set up */

    CHECK_INIT(fp);

    fp->_r = 0; /* largely a convenience for callers */

    /* SysV does not make this test; take it out for compatibility */
    if (fp->_flags & __SEOF)
        return EOF;

    /* if not already reading, have to be reading and writing */
    if ((fp->_flags & __SRD) == 0) {
        if ((fp->_flags & __SRW) == 0)
            return EOF;
        /* switch to reading */
        if (fp->_flags & __SWR) {
            if (fflush(fp))
                return EOF;
            fp->_flags &= ~__SWR;
            fp->_w = 0;
            fp->_lbfsize = 0;
        }
        fp->_flags |= __SRD;
    } else {
        /*
         * We were reading.  If there is an ungetc buffer,
         * we must have been reading from that.  Drop it,
         * restoring the previous buffer (if any).  If there
         * is anything in that buffer, return.
         */
        if (fp->_ub._base != NULL) {
            if ((fp->_r = fp->_ur) != 0) {
                fp->_p = fp->_up;
                return 0;
            }
        }
    }

    if (fp->_bf._base == NULL)
        __smakebuf(fp);

    /*
     * Before reading from a line buffered or unbuffered file,
     * flush all line buffered output files, per the ANSI C
     * standard.
     */
    if (fp->_flags & (__SLBF | __SNBF))
        (void)_fwalk(fp->_data, lflush);
    fp->_p = fp->_bf._base;
    fp->_r = (*fp->_read)(fp->_cookie, (char *)fp->_p, fp->_bf._size);
    fp->_flags &= ~__SMOD; /* buffer contents are again pristine */
    if (fp->_r <= 0) {
        if (fp->_r == 0)
            fp->_flags |= __SEOF;
        else {
            fp->_r = 0;
            fp->_flags |= __SERR;
        }
        return EOF;
    }

    return 0;
}
