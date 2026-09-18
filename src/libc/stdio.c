/* No user fns here.  Pesch 15apr92. */

#include <stdio.h>

extern int _read_r(struct _reent *, int, void *, unsigned long);
extern int _write_r(struct _reent *, int, const void *, unsigned long);
extern _fpos_t _lseek_r(struct _reent *, int, _fpos_t, int);
extern int _close_r(struct _reent *, int);

int __sread(void *cookie, char *buf, int n)
{
    register FILE *fp = (FILE *)cookie;
    register int ret;

    ret = _read_r(fp->_data, fp->_file, buf, n);
    /* If the read succeeded, update the current offset.  */

    if (ret >= 0)
        fp->_offset += ret;
    else
        fp->_flags &= ~__SOFF; /* paranoia */
    return ret;
}

int __swrite(void *cookie, const char *buf, int n)
{
    register FILE *fp = (FILE *)cookie;

    if (fp->_flags & __SAPP)
        (void)_lseek_r(fp->_data, fp->_file, (_fpos_t)0, SEEK_END);
    fp->_flags &= ~__SOFF; /* in case FAPPEND mode is set */
    return _write_r(fp->_data, fp->_file, buf, n);
}

_fpos_t __sseek(void *cookie, _fpos_t offset, int whence)
{
    register FILE *fp = (FILE *)cookie;
    register _fpos_t ret;

    ret = _lseek_r(fp->_data, fp->_file, offset, whence);
    if (ret == -1L)
        fp->_flags &= ~__SOFF;
    else {
        fp->_flags |= __SOFF;
        fp->_offset = ret;
    }
    return ret;
}

int __sclose(void *cookie)
{
    FILE *fp = (FILE *)cookie;

    return _close_r(fp->_data, fp->_file);
}
