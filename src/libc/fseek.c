/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 */

#include "local.h"
#include <sys/stat.h>
#include <errno.h>

#define POS_ERR (-(fpos_t)1)

#define HASUB(fp) ((fp)->_ub._base != NULL)

int fseek(register FILE *fp, long offset, int whence)
{
    struct _reent *ptr;
    fpos_t(*seekfn)(void *, fpos_t, int);
    fpos_t target, curoff;
    size_t n;
    struct stat st;
    int havepos;

    /* Make sure stdio is set up.  */

    CHECK_INIT(fp);
    ptr = fp->_data;

    /* If we've been doing some writing, and we're in append mode
       then we don't really know where the filepos is.  */

    if (fp->_flags & __SAPP && fp->_flags & __SWR) {
        /* So flush the buffer and seek to the end.  */
        fflush(fp);
    }

    /* Have to be able to seek.  */

    if ((seekfn = fp->_seek) == NULL) {
        ptr->_errno = ESPIPE; /* ??? */
        return -1;
    }

    /*
     * Change any SEEK_CUR to SEEK_SET, and check `whence' argument.
     * After this, whence is either SEEK_SET or SEEK_END.
     */

    switch (whence) {
    case SEEK_CUR:
        /*
         * In order to seek relative to the current stream offset,
         * we have to first find the current stream offset a la
         * ftell (see ftell for details).
         */
        fflush(fp); /* may adjust seek offset on append stream */
        if (fp->_flags & __SOFF)
            curoff = fp->_offset;
        else {
            curoff = (*seekfn)(fp->_cookie, (fpos_t)0, SEEK_CUR);
            if (curoff == -1L)
                return -1;
        }
        if (fp->_flags & __SRD) {
            curoff -= fp->_r;
            if (HASUB(fp))
                curoff -= fp->_ur;
        } else if (fp->_flags & __SWR && fp->_p != NULL)
            curoff += fp->_p - fp->_bf._base;

        offset += curoff;
        whence = SEEK_SET;
        havepos = 1;
        break;

    case SEEK_SET:
    case SEEK_END:
        havepos = 0;
        break;

    default:
        ptr->_errno = EINVAL;
        return (EOF);
    }

    /*
     * Can only optimise if:
     *	reading (and not reading-and-writing);
     *	not unbuffered; and
     *	this is a `regular' Unix file (and hence seekfn==__sseek).
     * We must check __NBF first, because it is possible to have __NBF
     * and __SOPT both set.
     */

    if (fp->_bf._base == NULL)
        __smakebuf(fp);
    if (fp->_flags & (__SWR | __SRW | __SNBF | __SNPT))
        goto dumb;
    if ((fp->_flags & __SOPT) == 0) {
        if (seekfn != __sseek || fp->_file < 0
            || _fstat_r(ptr, fp->_file, &st)
            || (st.st_mode & S_IFMT) != S_IFREG) {
            fp->_flags |= __SNPT;
            goto dumb;
        }
        fp->_blksize = S_BLKSIZE;
        fp->_flags |= __SOPT;
    }

    /*
     * We are reading; we can try to optimise.
     * Figure out where we are going and where we are now.
     */

    if (whence == SEEK_SET)
        target = offset;
    else {
        if (_fstat_r(ptr, fp->_file, &st))
            goto dumb;
        target = st.st_size + offset;
    }

    if (!havepos) {
        if (fp->_flags & __SOFF)
            curoff = fp->_offset;
        else {
            curoff = (*seekfn)(fp->_cookie, (fpos_t)0, SEEK_CUR);
            if (curoff == POS_ERR)
                goto dumb;
        }
        curoff -= fp->_r;
        if (HASUB(fp))
            curoff -= fp->_ur;
    }

    /*
     * Compute the number of bytes in the input buffer (pretending
     * that any ungetc() input has been discarded).  Adjust current
     * offset backwards by this count so that it represents the
     * file offset for the first byte in the current input buffer.
     */

    if (HASUB(fp)) {
        curoff += fp->_r; /* kill off ungetc */
        n = fp->_up - fp->_bf._base;
        curoff -= n;
        n += fp->_ur;
    } else {
        n = fp->_p - fp->_bf._base;
        curoff -= n;
        n += fp->_r;
    }

    /*
     * If the target offset is within the current buffer,
     * simply adjust the pointers, clear EOF, undo ungetc(),
     * and return.
     */

    if ((fp->_flags & __SMOD) == 0 && target >= curoff && target < curoff + n) {
        register int o = target - curoff;

        fp->_p = fp->_bf._base + o;
        fp->_r = n - o;
        fp->_flags &= ~__SEOF;
        return 0;
    }

    /*
     * The place we want to get to is not within the current buffer,
     * but we can still be kind to the kernel copyout mechanism by
     * not stopping at a block boundary.  Round the target to the
     * nearest block boundary.
     */

    curoff = target & ~(fp->_blksize - 1);
    if ((*seekfn)(fp->_cookie, curoff, SEEK_SET) == POS_ERR)
        goto dumb;
    fp->_r = 0;
    fp->_flags &= ~__SEOF;
    n = target - curoff;
    if (n) {
        if (__srefill(fp) || fp->_r < n)
            goto dumb;
        fp->_r -= n;
        fp->_p += n;
    }
    return 0;

    /*
     * We get here if we cannot optimise the seek ... just
     * do it.  Allow the seek function to change fp->_bf._base.
     */

dumb:
    if (fflush(fp) || (*seekfn)(fp->_cookie, (fpos_t)offset, whence) == POS_ERR)
        return -1;
    /* success: clear EOF indicator and discard ungetc() data */
    fp->_p = fp->_bf._base;
    fp->_r = 0;
    /* fp->_w = 0; */ /* unnecessary (I think...) */
    fp->_flags &= ~__SEOF;
    return 0;
}
