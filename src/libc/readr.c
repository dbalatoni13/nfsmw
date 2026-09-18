extern int errno;
extern int read(int, void *, unsigned long);

int _read_r(void *ptr, int fd, void *buf, unsigned long cnt)
{
    int ret;

    errno = 0;
    if ((ret = read(fd, buf, cnt)) == -1 && errno != 0)
        *(int *)ptr = errno;
    return ret;
}
