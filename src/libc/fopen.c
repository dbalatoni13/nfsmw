/* fopen.c de la libc de SN: el __sinit/__sfp de newlib (findfp.c), renombrados
 * _sn_sinit/_sn_sfp y con tabla fija de FILE, van en esta misma unidad (el ELF
 * original les pone snstd, _sn_iobf y _sn_stat_g detras de STT_FILE fopen.c). */
#include "local.h"

static FILE _sn_iobf[10];
static struct _glue _sn_stat_g;
extern struct __sbuf *_sn_IO_buf_ptr;
extern void _cleanup_r(struct _reent *);

static void snstd(FILE *ptr, int flags, int file, struct _reent *data)
{
    ptr->_p = 0;
    ptr->_r = 0;
    ptr->_w = 0;
    ptr->_flags = flags;
    ptr->_file = file;
    ptr->_bf._base = 0;
    ptr->_lbfsize = 0;
    ptr->_cookie = ptr;
    ptr->_read = __sread;
    ptr->_write = __swrite;
    ptr->_seek = __sseek;
    ptr->_close = __sclose;
    ptr->_data = data;
}

void _sn_sinit(struct _reent *s)
{
    s->__cleanup = _cleanup_r;
    s->__sdidinit = 1;
    snstd(&s->__sf[0], __SRD, 0, s);
    snstd(&s->__sf[1], __SWR | __SLBF, 1, s);
    snstd(&s->__sf[2], __SWR | __SNBF, 2, s);
    s->__sglue._next = &_sn_stat_g;
    s->__sglue._niobs = 3;
    s->__sglue._iobs = &s->__sf[0];
    _sn_stat_g._next = 0;
    _sn_stat_g._niobs = 10;
    _sn_stat_g._iobs = _sn_iobf;
}

FILE *_sn_sfp(struct _reent *d)
{
    FILE *fp;
    int n;
    struct _glue *g;
    struct __sbuf *b;
    volatile struct __sbuf *vb;

    if (!d->__sdidinit)
        _sn_sinit(d);
    if (d->__sglue._next == 0) {
        _sn_stat_g._next = 0;
        _sn_stat_g._niobs = 10;
        _sn_stat_g._iobs = _sn_iobf;
        d->__sglue._next = &_sn_stat_g;
    }
    for (g = &d->__sglue;; g = g->_next) {
        for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
            if (fp->_flags == 0)
                goto found;
    }

found:
    fp->_flags = 1; /* reserve this slot; caller sets real flags */
    fp->_r = fp->_w = 0;
    if (_sn_IO_buf_ptr[9 - n]._size > 0 && _sn_IO_buf_ptr[9 - n]._base != 0) {
        fp->_bf = _sn_IO_buf_ptr[9 - n];
        fp->_p = _sn_IO_buf_ptr[9 - n]._base;
        fp->_flags = __SLBF | __SMBF;
    } else {
        fp->_flags |= __SNBF;
        fp->_bf._base = fp->_p = fp->_nbuf;
        fp->_bf._size = 1;
    }
    fp->_file = -1; /* no file */
    fp->_lbfsize = 0; /* not line buffered */
    fp->_ub._base = NULL; /* no ungetc buffer */
    fp->_ub._size = 0;
    fp->_lb._base = NULL; /* no line buffer */
    fp->_lb._size = 0;
    fp->_data = d;
    return fp;
}

FILE *_fopen_r(struct _reent *ptr, const char *file, const char *mode)
{
    register FILE *fp;
    int f;
    int flags, oflags;

    if ((flags = __sflags(ptr, mode, &oflags)) == 0)
        return NULL;
    if ((fp = _sn_sfp(ptr)) == NULL)
        return NULL;

    fp->_flags &= __SNBF;

    if ((f = _open_r(fp->_data, file, oflags, 0666)) < 0) {
        fp->_flags = 0; /* release */
        return NULL;
    }

    fp->_file = f;
    fp->_flags = flags;
    fp->_cookie = (void *)fp;
    fp->_read = __sread;
    fp->_write = __swrite;
    fp->_seek = __sseek;
    fp->_close = __sclose;

    if (fp->_flags & __SAPP)
        fseek(fp, 0, SEEK_END);

    return fp;
}

FILE *fopen(const char *file, const char *mode)
{
    return _fopen_r(_REENT, file, mode);
}

void _cleanup_r(struct _reent *ptr)
{
    _fwalk(ptr, fflush);
}
