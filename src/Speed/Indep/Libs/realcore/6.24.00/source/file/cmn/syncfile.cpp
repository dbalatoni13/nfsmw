#include "types.h"

#include "../../../include/common/realcore/file/filesys.h"

int FILESYS_opensync(const char *name, unsigned int modeflags, int priority) {
    int op = FILESYS_open(name, modeflags, priority, nullptr);
    FILESYS_waitop(op);
    return FILESYS_completeop(op);
}

int FILESYS_readsync(int filehandle, int offset, void *buffer, int bytes, int priority) {
    int op = FILESYS_readlarge(filehandle, static_cast<unsigned long long>(offset), buffer,
                               static_cast<unsigned long long>(bytes), priority, nullptr);
    FILESYS_waitop(op);
    return FILESYS_completeop(op);
}

int FILESYS_writesync(int filehandle, int offset, void *buffer, int bytes, int priority) {
    int op = FILESYS_write(filehandle, offset, buffer, bytes, priority, nullptr);
    FILESYS_waitop(op);
    return FILESYS_completeop(op);
}

bool FILESYS_closesync(int filehandle, int priority) {
    int op = FILESYS_close(filehandle, priority, nullptr);
    FILESYS_waitop(op);
    return FILESYS_completeop(op) != 0;
}

int FILESYS_sizesync(int filehandle, int priority) {
    int op = FILESYS_size(filehandle, priority, nullptr);
    FILESYS_waitop(op);
    return FILESYS_completeop(op);
}

bool FILESYS_existssync(const char *name, int priority) {
    int op = FILESYS_exists(name, priority, nullptr);
    FILESYS_waitop(op);
    return FILESYS_completeop(op) != 0;
}
