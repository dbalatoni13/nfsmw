/* Internal stdio declarations, after newlib's libc/stdio/local.h.
   The SN ProDG runtime renames __sinit/__sfp to _sn_sinit/_sn_sfp and drops
   the dynamic-glue growth, but the rest is stock newlib 1.8.  */

#ifndef _LIBC_LOCAL_H_
#define _LIBC_LOCAL_H_

#include <stdio.h>

extern int __sread(void *, char *, int);
extern int __swrite(void *, const char *, int);
extern _fpos_t __sseek(void *, _fpos_t, int);
extern int __sclose(void *);
extern int __srefill(FILE *);
extern void __smakebuf(FILE *);
extern int __sflags(struct _reent *, const char *, int *);
extern FILE *_sn_sfp(struct _reent *);
extern int _fwalk(struct _reent *, int (*)());

/* declared without a prototype in the original: the call sites carry
   `crclr 4*cr1+eq'.  */
extern void _sn_sinit();

extern int _read_r(struct _reent *, int, void *, unsigned long);
extern int _write_r(struct _reent *, int, const void *, unsigned long);
extern _fpos_t _lseek_r(struct _reent *, int, _fpos_t, int);
extern int _close_r(struct _reent *, int);
extern int _open_r(struct _reent *, const char *, int, int);
extern int _fstat_r(struct _reent *, int, void *);

#define CHECK_INIT(fp)                          \
    do {                                        \
        if ((fp)->_data == 0)                   \
            (fp)->_data = _REENT;               \
        if (!(fp)->_data->__sdidinit)           \
            _sn_sinit((fp)->_data);             \
    } while (0)

#endif
