#ifndef REALCORE_FILE_GC_DVD_DEVICE_H
#define REALCORE_FILE_GC_DVD_DEVICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "../../../include/common/realcore/file/driver.h"
#include "../../../include/common/realcore/std/list.h"

#include <dolphin/dvd.h>

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

struct DvdFileHandle : public ListNodeS {
    DVDFileInfo fileInfo;
    int offset;
    int size;
};

struct GcDvdFileDeviceDriver : public RealFile::DeviceDriver {
    GcDvdFileDeviceDriver(const char *pName) : RealFile::DeviceDriver(pName) {}
    virtual ~GcDvdFileDeviceDriver() {}
    DvdFileHandle *_AllocateDvdFileHandle();
    void _FreeDvdFileHandle(DvdFileHandle *dvd_fh);
    virtual bool Init();
    virtual void Restore();
    virtual EAFileHandle Open(const char *filename, int, int *);
    virtual void Close(EAFileHandle h);
    virtual unsigned int Read(EAFileHandle h, void *buf, unsigned int bufsize,
                              RealFile::DeviceDriver *, EAFileHandle);
    virtual unsigned long long Seek(EAFileHandle h, unsigned long long offset, int whence,
                                    RealFile::DeviceDriver *, EAFileHandle);
    virtual unsigned long long Getsize(EAFileHandle h);
    virtual unsigned long long QueryLocation(EAFileHandle h);

    DvdFileHandle *mFileHandleMemory;
    ListSingle<DvdFileHandle> mFreeHandleQueue;
};

#endif
