extern int errno;
extern int fstat();

int _fstat_r(void *ptr, int fd, void *pstat)
{
    int ret;

    errno = 0;
    if ((ret = fstat(fd, pstat)) == -1 && errno != 0)
        *(int *)ptr = errno;
    return ret;
}
