#include "../../../include/common/realcore/file/filesys.h"

struct FILEDEVICE;

struct FILEREQUEST {
    const char *name;
    void *location;
    int parm;
    int abortflag;
    int flags;
};

extern int FILESYS_atomic(FILESYS_ATOM func, FILEDEVICE *device, int priority, void *userdata);
extern FILEDEVICE *FILE_nametodevice(const char *name);

bool FILE_exists(const char *name) {
    return FILESYS_existssync(name, 100);
}

static int filesizeatom(int priority, void *userdata) {
    FILEREQUEST *req = static_cast<FILEREQUEST *>(userdata);
    int fhandle = FILESYS_opensync(req->name, 1, priority);
    if (fhandle != 0) {
        int size = FILESYS_sizesync(fhandle, priority - 1);
        FILESYS_closesync(fhandle, priority - 1);
        return size;
    }
    return 0;
}

int FILE_size(const char *name) {
    FILEREQUEST req;
    req.name = name;
    req.abortflag = 1;
    return FILESYS_atomic(&filesizeatom, FILE_nametodevice(req.name), 100, &req);
}
