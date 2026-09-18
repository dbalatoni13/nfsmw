// Prototipos locales (linkage C++ manglado, como la TU original).
int FILESYS_existssync(const char *name, int priority);
int FILESYS_opensync(const char *name, unsigned int modeflags, int priority);
int FILESYS_sizesync(int filehandle, int priority);
bool FILESYS_closesync(int filehandle, int priority);
struct FILEDEVICE;
int FILESYS_atomic(int (*func)(int, void *), FILEDEVICE *device, int priority, void *arg);
int FILE_nametodevice(const char *name);

int FILE_exists(const char *name) {
    return FILESYS_existssync(name, 100);
}

static int filesizeatom(int priority, void *arg) {
    int filehandle = FILESYS_opensync(*(char **)arg, 1, priority);

    if (filehandle != 0) {
        int filesize = FILESYS_sizesync(filehandle, priority - 1);
        FILESYS_closesync(filehandle, priority - 1);
        return filesize;
    }

    return 0;
}

int FILE_size(const char *name) {
    struct {
        const char *name;
        int pad1;
        int pad2;
        unsigned int flags;
        int pad3;
    } atom;

    atom.flags = 1;
    atom.name = name;
    return FILESYS_atomic(filesizeatom, (FILEDEVICE *)FILE_nametodevice(name), 100, &atom);
}
