#pragma implementation
#include "Speed/Indep/Libs/realcore/6.24.00/source/file/gc/hd_device.h"

#include <string.h>

bool GcHdFileDeviceDriver::Init() {
    PCinit();
    return true;
}

EAFileHandle GcHdFileDeviceDriver::Open(const char *name, int oflags, int *pParentFileHandle) {
    int handle = -1;

    if (strncmp(name, "hd:", 3) == 0) {
        name += 3;
    }
    name += 1;

    switch (oflags & 7) {
    case 0:
        handle = PCopen(name, 2, 0);
        break;
    case 1:
        handle = PCopen(name, 0, 0);
        break;
    case 6:
        handle = PCcreat(name, 0);
        break;
    }

    return handle;
}

void GcHdFileDeviceDriver::Close(EAFileHandle h) {
    PCclose(h);
}

uint32_t GcHdFileDeviceDriver::Read(EAFileHandle h, void *buf, unsigned int bufsize, DeviceDriver *ddParent, EAFileHandle ddFileHandle) {
    return PCread(h, buf, bufsize);
}

uint32 GcHdFileDeviceDriver::Write(EAFileHandle h, const void *buf, unsigned int bufsize, DeviceDriver *ddParent, EAFileHandle ddFileHandle) {
    return PCwrite(h, buf, bufsize);
}

uint64_t GcHdFileDeviceDriver::Seek(EAFileHandle h, uint64_t offset, int whence, DeviceDriver *ddParent, EAFileHandle ddFileHandle) {
    int origin = 0;

    switch (whence) {
    case 1:
        origin = 1;
        break;
    case 2:
        origin = 2;
        break;
    case -1:
        origin = 0;
        break;
    }
    return PClseek(h, (long)offset, origin);
}

uint64_t GcHdFileDeviceDriver::Getsize(EAFileHandle h) {
    int cur = PClseek(h, 0, 1);
    int end = PClseek(h, 0, 2);

    PClseek(h, cur, 0);
    return end;
}

GcHdFileDeviceDriver GcHd_fdd;
