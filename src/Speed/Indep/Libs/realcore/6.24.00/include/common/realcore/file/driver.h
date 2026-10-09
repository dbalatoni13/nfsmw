#ifndef REALCORE_COMMON_FILE_DRIVER_H
#define REALCORE_COMMON_FILE_DRIVER_H

#include "types.h"

typedef void ASYNCFILE_CALLBACK(intptr_t);
typedef intptr_t FILEHANDLE;

namespace RealFile {

// TODO pure virtuals?
// total size: 0x14
class DeviceDriver {
  public:
    DeviceDriver(const char *name);
    virtual ~DeviceDriver() {}

    virtual bool Init() {
        return true;
    }

    virtual void Restore() {}

    virtual FILEHANDLE Open(const char *name, int oflags, int *pParentFileHandle) = 0;

    virtual void Close(FILEHANDLE h) = 0;

    virtual uint32_t Read(FILEHANDLE h, void *buf, unsigned int bufsize, DeviceDriver *ddParent, FILEHANDLE ddFileHandle) = 0;

    virtual uint32 Write(FILEHANDLE h, const void *buf, unsigned int bufsize, DeviceDriver *ddParent, FILEHANDLE ddFileHandle) {
        return 0;
    }

    virtual uint64_t Seek(FILEHANDLE h, uint64_t offset, int whence, DeviceDriver *ddParent, FILEHANDLE ddFileHandle) = 0;

    virtual uint64_t Getsize(FILEHANDLE h) = 0;

    virtual uint64_t QueryLocation(FILEHANDLE h) {
        return 0;
    }

    virtual bool Remove(const char *name) {
        return false;
    }

    virtual uint64_t Getspace() {
        return 0;
    }

    virtual const char *GetName() {
        return this->mDeviceName;
    }

    virtual uint32_t GetOptimalReadSize() {
        return 0;
    }

  protected:
    char mDeviceName[16]; // offset 0x0, size 0x10
};

void AddDevice(DeviceDriver *device);
void AddSearchLocation(const char *location, bool recursive);

} // namespace RealFile

#endif
