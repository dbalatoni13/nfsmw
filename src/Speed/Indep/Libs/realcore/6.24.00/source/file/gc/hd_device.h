#ifndef REALCORE_FILE_GC_HD_DEVICE_H
#define REALCORE_FILE_GC_HD_DEVICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../../../include/common/realcore/file/driver.h"

struct GcHdFileDeviceDriver : public RealFile::DeviceDriver {
    GcHdFileDeviceDriver(const char *pName) : RealFile::DeviceDriver(pName) {}
    virtual ~GcHdFileDeviceDriver() {}
    virtual void Restore() {}
    virtual unsigned long long Getspace() { return 0; }
    virtual bool Init();
    virtual EAFileHandle Open(const char *name, int, int *);
    virtual void Close(EAFileHandle h);
    virtual unsigned int Read(EAFileHandle h, void *buf, unsigned int bufsize,
                              RealFile::DeviceDriver *, EAFileHandle);
    virtual unsigned int Write(EAFileHandle h, const void *buf, unsigned int bufsize,
                               RealFile::DeviceDriver *, EAFileHandle);
    virtual unsigned long long Seek(EAFileHandle h, unsigned long long offset, int whence,
                                    RealFile::DeviceDriver *, EAFileHandle);
    virtual unsigned long long Getsize(EAFileHandle h);
};

#endif
