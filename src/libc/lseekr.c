extern int errno;
extern long lseek(int, long, int);

long _lseek_r(void *ptr, int fd, long pos, int whence)
{
    long ret;

    errno = 0;
    if ((ret = lseek(fd, pos, whence)) == -1 && errno != 0)
        *(int *)ptr = errno;
    return ret;
}
