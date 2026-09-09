#include "dvd_device.h"

#include "types.h"
#include "../../../include/common/realcore/file/filesys.h"
#include "../../../include/common/realcore/std.h"

#include <dolphin/dvd.h>
#include <dolphin/os.h>
#include <dolphin/os/OSCache.h>

#include <cstring>

#include "../../../include/common/realcore/system/threads.h"

GcDvdFileDeviceDriver GcDvd_fdd("dvd:");
static OSMessageQueue ReadFileThreadMsgQ;
static void *ReadFileThreadMsgData[32];
static ReadStatus gCurRead;

static void AyncDVDCallback(s32, DVDFileInfo *);
static int AyncDVDRead(DVDFileInfo *FileInfo);
static void StartNonAlignedAyncRead(DVDFileInfo *FileInfo, void *MemPointer, s32 FileBase, s32 Size);

static void QEndOp() {
    gCurRead.CurState = DONE;
    AyncDVDCallback(0, nullptr);
}

static int AyncDVDRead(DVDFileInfo *FileInfo) {
    int nBytesToKeep;
    int nBytesToRead;

    nBytesToKeep = 0;
    if (gCurRead.CurState == ALIGN_THE_START) {
        nBytesToKeep = gCurRead.TA;
        MEM_copy(reinterpret_cast<void *>(gCurRead.MemBase), gCurRead.Data + gCurRead.SD, nBytesToKeep);
        gCurRead.FileBase += nBytesToKeep;
        gCurRead.MemBase += nBytesToKeep;
    } else if (gCurRead.CurState == ALIGN_READ) {
        nBytesToKeep = gCurRead.MEA - gCurRead.MemBase;
        DCInvalidateRange(reinterpret_cast<void *>(gCurRead.MemBase), nBytesToKeep);
        gCurRead.FileBase += nBytesToKeep;
        gCurRead.MemBase += nBytesToKeep;
    } else if (gCurRead.CurState == NONALIGN_READ) {
        nBytesToKeep = gCurRead.ME - gCurRead.MemBase;
        if (nBytesToKeep > 0x4000) {
            nBytesToKeep = 0x4000;
        }
        MEM_copy(reinterpret_cast<void *>(gCurRead.MemBase), gCurRead.Data, nBytesToKeep);
        gCurRead.FileBase += nBytesToKeep;
        gCurRead.MemBase += nBytesToKeep;
    } else {
        if (gCurRead.CurState == SMALL_FILE) {
            nBytesToKeep = gCurRead.Size;
            MEM_copy(reinterpret_cast<void *>(gCurRead.MemBase), gCurRead.Data + gCurRead.SD, nBytesToKeep);
            gCurRead.CurState = SMALL_FILE;
            QEndOp();
            return nBytesToKeep;
        }
    }


    if (gCurRead.MemBase == gCurRead.MSA && gCurRead.MemBase < gCurRead.MEA) {
        gCurRead.CurState = ALIGN_READ;
        gCurRead.ret = DVDReadAsyncPrio(FileInfo, reinterpret_cast<void *>(gCurRead.MemBase),
                                        gCurRead.MEA - gCurRead.MemBase, gCurRead.FileBase,
                                       AyncDVDCallback, 2);
        return nBytesToKeep;
    }
    if (gCurRead.ME > gCurRead.MemBase) {
        nBytesToRead = (gCurRead.ME - gCurRead.MemBase + 0x1f) & ~0x1f;
        if (nBytesToRead > 0x4000) {
            nBytesToRead = 0x4000;
        }
        gCurRead.CurState = NONALIGN_READ;
        gCurRead.ret += DVDReadAsyncPrio(FileInfo, gCurRead.Data, nBytesToRead, gCurRead.FileBase,
                                        AyncDVDCallback, 2);
        return nBytesToKeep;
    }
    QEndOp();
    return nBytesToKeep;
}

static void StartNonAlignedAyncRead(DVDFileInfo *FileInfo, void *MemPointer, s32 FileBase, s32 Size) {
    int readSize;

    if (Size == 0) {
        QEndOp();
        return;
    }

    gCurRead.Size = Size;
    gCurRead.FileBase = FileBase;
    gCurRead.MemBase = reinterpret_cast<int>(MemPointer);
    gCurRead.MBS = gCurRead.MemBase & ~0x1f;
    gCurRead.MSA = (gCurRead.MemBase + 0x1f) & ~0x1f;
    gCurRead.ME = gCurRead.MemBase + Size;
    gCurRead.MEA = gCurRead.ME & ~0x1f;
    gCurRead.FBS = FileBase & ~0x1f;
    gCurRead.FSA = (FileBase + 0x1f) & ~0x1f;
    gCurRead.FE = FileBase + Size;
    gCurRead.FEA = gCurRead.FE & ~0x1f;
    gCurRead.SD = FileBase - gCurRead.FBS;
    gCurRead.TA = 0x20 - gCurRead.SD;
    gCurRead.ret = 0;

    if (Size <= 0x1f) {
        int sizealigned;

        gCurRead.CurState = SMALL_FILE;
        sizealigned = gCurRead.FE - gCurRead.FBS;
        sizealigned = (sizealigned + 0x1f) & ~0x1f;
        gCurRead.ret = DVDReadAsyncPrio(FileInfo, gCurRead.Data, sizealigned, gCurRead.FBS,
                                        AyncDVDCallback, 2);
    } else if (gCurRead.SD != 0) {
        gCurRead.CurState = ALIGN_THE_START;
        readSize = (gCurRead.TA + 0x1f) & ~0x1f;
        gCurRead.ret += DVDReadAsyncPrio(FileInfo, gCurRead.Data, readSize, gCurRead.FBS,
                                         AyncDVDCallback, 2);
    } else {
        if (gCurRead.MemBase == gCurRead.MSA && gCurRead.MSA < gCurRead.MEA) {
            gCurRead.CurState = ALIGN_READ;
            readSize = gCurRead.MEA - gCurRead.MSA;
            gCurRead.ret = DVDReadAsyncPrio(FileInfo, reinterpret_cast<void *>(gCurRead.MSA), readSize,
                                            gCurRead.FileBase, AyncDVDCallback, 2);
            return;
        }
        if (gCurRead.ME <= gCurRead.MemBase) {
            return;
        }
        readSize = (gCurRead.ME - gCurRead.MemBase + 0x1f) & ~0x1f;
        if (readSize > 0x4000) {
            readSize = 0x4000;
        }
        gCurRead.CurState = NONALIGN_READ;
        gCurRead.ret += DVDReadAsyncPrio(FileInfo, gCurRead.Data, readSize, gCurRead.FileBase,
                                         AyncDVDCallback, 2);
    }
}

static void AyncDVDCallback(s32, DVDFileInfo *) {
    OSSendMessage(&ReadFileThreadMsgQ, reinterpret_cast<void *>(1), 1);
}

DvdFileHandle *GcDvdFileDeviceDriver::_AllocateDvdFileHandle() {
    DvdFileHandle *dvd_fh;

    dvd_fh = this->mFreeHandleQueue.Pop();
    return dvd_fh;
}

void GcDvdFileDeviceDriver::_FreeDvdFileHandle(DvdFileHandle *dvd_fh) {
    MEM_clear(dvd_fh, sizeof(DvdFileHandle));
    this->mFreeHandleQueue.Push(dvd_fh);
}

bool GcDvdFileDeviceDriver::Init() {
    DvdFileHandle *dvd_fh;

    this->mFileHandleMemory = static_cast<DvdFileHandle *>(gFileSysOpts.allocator->Alloc(
        gFileSysOpts.MaxOpenFiles * sizeof(DvdFileHandle),
        EA::TagValuePair(EA::Allocator::ATT_NAME, "DVD File Handles") +
        EA::TagValuePair(EA::Allocator::ATT_ALLOC_HIGH, 1)));
    MEM_clear(this->mFileHandleMemory, gFileSysOpts.MaxOpenFiles * sizeof(DvdFileHandle));
    {
        int iFiles;

        dvd_fh = this->mFileHandleMemory;
        iFiles = 0;
        while (iFiles < gFileSysOpts.MaxOpenFiles) {
            this->mFreeHandleQueue.Push(dvd_fh);
            iFiles++;
            dvd_fh++;
        }
    }
    OSInitMessageQueue(&ReadFileThreadMsgQ, ReadFileThreadMsgData, 0x20);
    return true;
}

void GcDvdFileDeviceDriver::Restore() {
    gFileSysOpts.allocator->Free(this->mFileHandleMemory, 0);
}

void GcDvdFileDeviceDriver::Close(EAFileHandle h) {
    DvdFileHandle *dvd_fh;

    dvd_fh = reinterpret_cast<DvdFileHandle *>(h);
    DVDClose(&dvd_fh->fileInfo);
    this->_FreeDvdFileHandle(dvd_fh);
}

unsigned long long GcDvdFileDeviceDriver::Getsize(EAFileHandle h) {
    return reinterpret_cast<DvdFileHandle *>(h)->size;
}

unsigned long long GcDvdFileDeviceDriver::QueryLocation(EAFileHandle h) {
    return 0;
}

EAFileHandle GcDvdFileDeviceDriver::Open(const char *filename, int, int *) {
    char newname[256];
    DvdFileHandle *dvd_fh;
    char *name;
    const char *namesrc;
    int ret;
    int entryNum;

    name = newname;
    dvd_fh = this->_AllocateDvdFileHandle();
    namesrc = filename;
    while (*namesrc != '\0') {
        if (*namesrc == '\\') {
            *name++ = '/';
        } else {
            *name++ = *namesrc;
        }
        namesrc++;
    }
    *name = '\0';
    name = newname;
    if (strncmp(name, "dvd:", 4) == 0) {
        name += 4;
    }
    ret = 0;
    entryNum = DVDConvertPathToEntrynum(name + 1);
    if (entryNum != -1) {
        ret = DVDFastOpen(entryNum, &dvd_fh->fileInfo);
    }
    if (ret == 0) {
        this->Close(reinterpret_cast<EAFileHandle>(dvd_fh));
        return -1;
    }
    dvd_fh->size = dvd_fh->fileInfo.length;
    return reinterpret_cast<EAFileHandle>(dvd_fh);
}

unsigned long long GcDvdFileDeviceDriver::Seek(EAFileHandle h, unsigned long long offset, int whence,
                                               RealFile::DeviceDriver *, EAFileHandle) {
    DvdFileHandle *dvd_fh;

    dvd_fh = reinterpret_cast<DvdFileHandle *>(h);
    switch (whence) {
    case 0:
        dvd_fh->offset = offset;
        break;
    case 1:
        dvd_fh->offset += offset;
        break;
    case 2:
        dvd_fh->offset = this->Getsize(h) - offset;
        break;
    }
    return offset;
}

unsigned int GcDvdFileDeviceDriver::Read(EAFileHandle h, void *buf, unsigned int bufsize,
                                         RealFile::DeviceDriver *, EAFileHandle) {
    DvdFileHandle *dvd_fh;
    void *msg;
    int nBytesKept;

    dvd_fh = reinterpret_cast<DvdFileHandle *>(h);
    if (static_cast<int>(bufsize) > dvd_fh->size - dvd_fh->offset) {
        bufsize = dvd_fh->size - dvd_fh->offset;
        if (dvd_fh->offset >= dvd_fh->size) {
            bufsize = 0;
        }
    }
    nBytesKept = 0;
    StartNonAlignedAyncRead(&dvd_fh->fileInfo, buf, dvd_fh->offset, bufsize);
    OSReceiveMessage(&ReadFileThreadMsgQ, &msg, 1);
    while (gCurRead.CurState != DONE) {
        nBytesKept += AyncDVDRead(&dvd_fh->fileInfo);
        OSReceiveMessage(&ReadFileThreadMsgQ, &msg, 1);
    }
    {
        int Err;

        while ((Err = DVDGetDriveStatus()) != 0 && Err != 8 && Err != 1) {
            THREAD_yield(100);
        }
    }
    dvd_fh->offset += nBytesKept;
    return nBytesKept;
}
