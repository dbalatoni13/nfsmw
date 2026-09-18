// Prototipos locales (linkage C++ manglado, como la TU original).
typedef int FILEOP;

FILEOP FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata);
FILEOP FILESYS_readlarge(int filehandle, unsigned long long offset, void *buffer,
                         unsigned long long bytes, int priority, void *userdata);
FILEOP FILESYS_write(int filehandle, int offset, void *buffer, int bytes, int priority,
                     void *userdata);
FILEOP FILESYS_close(int filehandle, int timeout, void *userdata);
FILEOP FILESYS_size(int filehandle, int priority, void *userdata);
FILEOP FILESYS_exists(const char *name, int priority, void *userdata);
int FILESYS_waitop(FILEOP ophandle);
int FILESYS_completeop(FILEOP ophandle);

int FILESYS_opensync(const char *name, unsigned int modeflags, int priority) {
    FILEOP ophandle = FILESYS_open(name, modeflags, priority, 0);

    FILESYS_waitop(ophandle);
    return FILESYS_completeop(ophandle);
}

int FILESYS_readsync(int filehandle, int offset, void *buffer, int bytes, int priority) {
    FILEOP ophandle =
        FILESYS_readlarge(filehandle, offset, buffer, bytes, priority, 0);

    FILESYS_waitop(ophandle);
    return FILESYS_completeop(ophandle);
}

int FILESYS_writesync(int filehandle, int offset, void *buffer, int bytes, int priority) {
    FILEOP ophandle = FILESYS_write(filehandle, offset, buffer, bytes, priority, 0);

    FILESYS_waitop(ophandle);
    return FILESYS_completeop(ophandle);
}

bool FILESYS_closesync(int filehandle, int priority) {
    FILEOP ophandle = FILESYS_close(filehandle, priority, 0);

    FILESYS_waitop(ophandle);
    return FILESYS_completeop(ophandle) != 0;
}

int FILESYS_sizesync(int filehandle, int priority) {
    FILEOP ophandle = FILESYS_size(filehandle, priority, 0);

    FILESYS_waitop(ophandle);
    return FILESYS_completeop(ophandle);
}

bool FILESYS_existssync(const char *name, int priority) {
    FILEOP ophandle = FILESYS_exists(name, priority, 0);

    FILESYS_waitop(ophandle);
    return FILESYS_completeop(ophandle) != 0;
}
