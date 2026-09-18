struct _reent {
    int _errno;
    void *_stdin;
    void *_stdout;
};
extern struct _reent *_impure_ptr;
extern int vfprintf(void *fp, const char *fmt, char *ap);

int vprintf(const char *fmt, char *ap)
{
    return vfprintf(_impure_ptr->_stdout, fmt, ap);
}
