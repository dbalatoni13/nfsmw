#ifndef REALCORE_SOURCE_FILE_GC_DVD_DEVICE_H
#define REALCORE_SOURCE_FILE_GC_DVD_DEVICE_H

#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/file/driver.h"
#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/std/list.h"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/dvd.h>
#endif

// total size: 0x48
struct DvdFileHandle : public ListNodeS {
    DVDFileInfo fileInfo; // offset 0x4, size 0x3C
    int offset;           // offset 0x40, size 0x4
    int size;             // offset 0x44, size 0x4
};

// Relleno: el ctor cae en la linea 51 y el dtor en la 53 del original.
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// total size: 0x24
class GcDvdFileDeviceDriver : public RealFile::DeviceDriver {
  public:
    GcDvdFileDeviceDriver(const char *pName) : DeviceDriver(pName) {}

    virtual ~GcDvdFileDeviceDriver() {}

    DvdFileHandle *_AllocateDvdFileHandle();
    void _FreeDvdFileHandle(DvdFileHandle *dvd_fh);

    virtual bool Init();
    virtual void Restore();
    virtual EAFileHandle Open(const char *filename, int oflags, int *pParentFileHandle);
    virtual void Close(EAFileHandle h);
    virtual uint32_t Read(EAFileHandle h, void *buf, unsigned int bufsize, DeviceDriver *ddParent,
                          EAFileHandle ddFileHandle);
    virtual uint64_t Seek(EAFileHandle h, uint64_t offset, int whence, DeviceDriver *ddParent,
                          EAFileHandle ddFileHandle);
    virtual uint64_t Getsize(EAFileHandle h);
    virtual uint64_t QueryLocation(EAFileHandle h);

    DvdFileHandle *mFileHandleMemory;           // offset 0x14, size 0x4
    ListSingle<DvdFileHandle> mFreeHandleQueue; // offset 0x18, size 0xC
};

extern GcDvdFileDeviceDriver GcDvd_fdd;

// Tipos que solo usa dvd_device.cpp. Van al FINAL a proposito: el relleno de arriba
// fija la linea del ctor (51) y del dtor (53) del original.
enum FileReadStateEnum {
    START = 0,
    SMALL_FILE = 1,
    ALIGN_THE_START = 2,
    ALIGN_READ = 3,
    NONALIGN_READ = 4,
    DONE = 5,
};

struct ReadStatus {
    FileReadStateEnum CurState;
    int MemBase;
    int FileBase;
    int Size;
    int MBS;
    int MSA;
    int ME;
    int MEA;
    int FBS;
    int FSA;
    int FE;
    int FEA;
    int SD;
    int TA;
    int ret;
    volatile void *MemPointer;
    volatile long parmFileBase;
    volatile long parmSize;
    char Data[32768] __attribute__((aligned(32)));
};

#endif
