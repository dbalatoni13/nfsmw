#include "local.h"
#include <sys/stat.h>

void __smakebuf(register FILE *fp)
{
    struct stat st;

    if (fp->_flags & __SNBF) {
        fp->_bf._base = fp->_p = fp->_nbuf;
        fp->_bf._size = 1;
        return;
    }
    if (fp->_file >= 0 && _fstat_r(fp->_data, fp->_file, &st) >= 0
        && (st.st_mode & S_IFMT) == S_IFREG && fp->_seek == __sseek) {
        fp->_flags |= __SOPT;
        fp->_blksize = S_BLKSIZE;
    } else
        fp->_flags |= __SNPT;

    fp->_flags |= __SNBF;
    fp->_bf._base = fp->_p = fp->_nbuf;
    fp->_bf._size = 1;
}
