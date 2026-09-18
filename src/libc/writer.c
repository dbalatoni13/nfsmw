extern int errno;
extern int write(int, const void *, unsigned long);

int _write_r(void *ptr, int fd, const void *buf, unsigned long cnt)
{
    int ret;

    errno = 0;
    if ((ret = write(fd, buf, cnt)) == -1 && errno != 0)
        *(int *)ptr = errno;
    return ret;
}
