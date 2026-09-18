#ifndef REALCORE_SOURCE_FILE_GC_HD_DEVICE_H
#define REALCORE_SOURCE_FILE_GC_HD_DEVICE_H

// Driver de "disco duro" via el filesystem PC del toolchain SN (PCxxx).


#pragma interface

#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/file/driver.h"

extern "C" {
extern int PCinit(void);
extern int PCcreat(const char *, int);
extern int PCopen(const char *, int, int);
extern int PCclose(int);
extern int PCread(int, void *, unsigned int);
extern int PCwrite(int, const void *, unsigned int);
extern int PClseek(int, long, int);
}

class GcHdFileDeviceDriver : public RealFile::DeviceDriver {
  public:
    GcHdFileDeviceDriver() : DeviceDriver("hd:") {}

    virtual ~GcHdFileDeviceDriver() {}

    virtual bool Init();
    virtual EAFileHandle Open(const char *name, int oflags, int *pParentFileHandle);
    virtual void Close(EAFileHandle h);
    virtual uint32_t Read(EAFileHandle h, void *buf, unsigned int bufsize, DeviceDriver *ddParent, EAFileHandle ddFileHandle);
    virtual uint32 Write(EAFileHandle h, const void *buf, unsigned int bufsize, DeviceDriver *ddParent, EAFileHandle ddFileHandle);
    virtual uint64_t Seek(EAFileHandle h, uint64_t offset, int whence, DeviceDriver *ddParent, EAFileHandle ddFileHandle);
    virtual uint64_t Getsize(EAFileHandle h);
    virtual void Restore() {}
    virtual uint64_t Getspace() { return 0; }
};

extern GcHdFileDeviceDriver GcHd_fdd;

#endif
