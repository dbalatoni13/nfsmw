typedef unsigned long size_t;

struct stat {
    int st_dev;    // offset 0x0
    int st_mode;   // offset 0x4
};

extern void InitializeUART(unsigned long);
extern unsigned long WriteUARTN(const void *, unsigned long);
extern int PCcreat(const char *, int);
extern int PCopen(const char *, int, int);
extern int PCclose(int);
extern int PCread(int);
extern int PCwrite(int, const void *, size_t);
extern int PClseek(int, long, int);

int __sn_serialp(const void *buffer, size_t numBytes) {
    static int first = 1;

    if (first != 0) {
        first = 0;
        InitializeUART(0);
    }
    WriteUARTN(buffer, numBytes);
    return numBytes;
}

int _write(int fd, const void *buffer, size_t numBytes) {
    if (fd == 1 || fd == 2) {
        return __sn_serialp(buffer, numBytes);
    }
    if (fd == 0) {
        return -1;
    }
    return PCwrite(fd, buffer, numBytes);
}

int write(int fd, const void *buffer, size_t numBytes) {
    return _write(fd, buffer, numBytes);
}

int close(int fd) {
    return PCclose(fd);
}

int fstat(int fd, struct stat *buf) {
    int result;

    result = 0;
    buf->st_mode = 0x2000;
    return result;
}

long lseek(int fd, long offset, int whence) {
    return PClseek(fd, offset, whence);
}

int read(int fd, char *buffer, size_t numBytes) {
    return PCread(fd);
}

int open(const char *path, int oflag, ...) {
    int fd;

    if (oflag & 0x200) {
        fd = PCcreat(path, 0);
        if (fd != -1) {
            if (PCclose(fd) != 0) {
                return -1;
            }
        }
    }
    return PCopen(path, oflag, 0);
}
