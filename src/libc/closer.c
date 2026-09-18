extern int errno;
extern int close(int);

int _close_r(void *ptr, int fd)
{
    int ret;

    errno = 0;
    if ((ret = close(fd)) == -1 && errno != 0)
        *(int *)ptr = errno;
    return ret;
}
