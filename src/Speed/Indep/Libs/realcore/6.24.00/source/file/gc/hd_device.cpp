#include "hd_device.h"

#include <string.h>

#include <libsn.h>

GcHdFileDeviceDriver GcHd_fdd("hd:");

bool GcHdFileDeviceDriver::Init() {
    PCinit();
    return true;
}

void GcHdFileDeviceDriver::Close(EAFileHandle h) {
    PCclose(h);
}

unsigned int GcHdFileDeviceDriver::Read(EAFileHandle h, void *buf, unsigned int bufsize,
                                        RealFile::DeviceDriver *, EAFileHandle) {
    return PCread(h, buf, bufsize);
}

unsigned int GcHdFileDeviceDriver::Write(EAFileHandle h, const void *buf, unsigned int bufsize,
                                         RealFile::DeviceDriver *, EAFileHandle) {
    return PCwrite(h, const_cast<void *>(buf), bufsize);
}

unsigned long long GcHdFileDeviceDriver::Seek(EAFileHandle h, unsigned long long offset, int whence,
                                              RealFile::DeviceDriver *, EAFileHandle) {
    int origin;

    origin = 0;
    switch (whence) {
    case 1:
        origin = 1;
        break;
    case 2:
        origin = 2;
        break;
    default:
        break;
    }
    return PClseek(h, offset, origin);
}

unsigned long long GcHdFileDeviceDriver::Getsize(EAFileHandle h) {
    int len;
    long curpos;

    curpos = PClseek(h, 0, 1);
    len = PClseek(h, 0, 2);
    PClseek(h, curpos, 0);
    return len;
}

EAFileHandle GcHdFileDeviceDriver::Open(const char *name, int oflags, int *) {
    int m;
    int hFile;

    hFile = -1;
    m = strncmp(name, "hd:", 3);
    if (m == 0) {
        name += 3;
    }
    name++;
    switch (oflags & 7) {
    case 0:
        hFile = PCopen(name, 2, 0);
        break;
    case 1:
        hFile = PCopen(name, 0, 0);
        break;
    case 6:
        hFile = PCcreat(name, 0);
        break;
    default:
        break;
    }
    return hFile;
}
