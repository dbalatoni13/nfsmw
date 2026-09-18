struct _reent {
    int _errno;
};

#define __SRD 0x0004
#define __SWR 0x0008
#define __SRW 0x0010
#define __SAPP 0x0100

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_APPEND 0x0008
#define O_CREAT 0x0200
#define O_TRUNC 0x0400

#define EINVAL 22

int __sflags(struct _reent *ptr, const char *mode, int *optr)
{
    int ret, m, o;

    switch (mode[0]) {
    case 'r': /* open for reading */
        ret = __SRD;
        m = O_RDONLY;
        o = 0;
        break;

    case 'w': /* open for writing */
        ret = __SWR;
        m = O_WRONLY;
        o = O_CREAT | O_TRUNC;
        break;

    case 'a': /* open for appending */
        ret = __SWR | __SAPP;
        m = O_WRONLY;
        o = O_CREAT | O_APPEND;
        break;

    default: /* illegal mode */
        ptr->_errno = EINVAL;
        return 0;
    }

    if (mode[1] == '+' || mode[2] == '+') {
        ret = __SRW;
        m = O_RDWR;
    }

    *optr = m | o;
    return ret;
}
