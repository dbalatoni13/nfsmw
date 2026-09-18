extern int errno;
extern int open();

int _open_r(void *ptr, const char *file, int flags, int mode)
{
    int ret;

    errno = 0;
    if ((ret = open(file, flags, mode)) == -1 && errno != 0)
        *(int *)ptr = errno;
    return ret;
}
