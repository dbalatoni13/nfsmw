#include "Speed/Indep/Libs/realcore/6.24.00/source/file/gc/dvd_device.h"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/os/OSCache.h>
#endif
#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/os/OSMessage.h>
#endif

#include <string.h>

void MEM_clear(void *ptr, int size);
void MEM_copy(void *dst, const void *src, int size);
void THREAD_yield(int ms);

// total size: 0xC
struct TagValuePair {
    unsigned int mTag; // offset 0x0, size 0x4
    union {
        int mInt;             // offset 0x0, size 0x4
        unsigned int mSize;   // offset 0x0, size 0x4
        float mFloat;         // offset 0x0, size 0x4
        const void *mPointer; // offset 0x0, size 0x4
    } mValue;                 // offset 0x4, size 0x4
    mutable const TagValuePair *mNext; // offset 0x8, size 0x4

    TagValuePair(unsigned int tag, int value) {
        mTag = tag;
        mValue.mInt = value;
        mNext = 0;
    }
    TagValuePair(unsigned int tag, unsigned int value) {
        mTag = tag;
        mValue.mSize = value;
        mNext = 0;
    }
    TagValuePair(unsigned int tag, float value) {
        mTag = tag;
        mValue.mFloat = value;
        mNext = 0;
    }
    TagValuePair(unsigned int tag, const void *value) {
        mTag = tag;
        mValue.mPointer = value;
        mNext = 0;
    }

    const TagValuePair &operator+(const TagValuePair &rhs) const {
        rhs.mNext = this;
        return rhs;
    }
};

// total size: 0x4
struct IAllocator {
    virtual void *Alloc(unsigned int size, const TagValuePair &flags) = 0;
    virtual void Free(void *pBlock, unsigned int size) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;

  protected:
    virtual ~IAllocator() {}
};

struct FILESYSOPTS {
    int size;                                // +0x00
    IAllocator *allocator;                   // +0x04
    int MaxOpenFiles;                        // +0x08
    int MaxFileOps;                          // +0x0C
    int nSearchLocs;                         // +0x10
    int nSearchPathLength;                   // +0x14
    int MaxDevices;                          // +0x18
    int ThreadStackSize;                     // +0x1C
    int (*decompresssize)(const void *);     // +0x20
    int (*decompress)(const void *, void *); // +0x24
    unsigned int LargeReadSliceSize;         // +0x28
    unsigned int AllocAlignBoundary;         // +0x2C
    int DiscType;                            // +0x30
    int mErrorRetryCount;                    // +0x34
};

extern FILESYSOPTS gFileSysOpts;

GcDvdFileDeviceDriver GcDvd_fdd("dvd:");

static OSMessageQueue ReadFileThreadMsgQ;
static void *ReadFileThreadMsgData[32];

// total size: 0x8060
struct CURREAD {
    int State;             // +0x00
    int MemPointer;        // +0x04
    int FileBase;          // +0x08
    int Size;              // +0x0C
    int MemStart;          // +0x10
    int MemStartAligned;   // +0x14
    int MemEnd;            // +0x18
    int MemEndAligned;     // +0x1C
    int FileStart;          // +0x20
    int FileStartAligned;   // +0x24
    int FileEnd;            // +0x28
    int FileEndAligned;     // +0x2C
    int PreBytes;           // +0x30
    int PreKeep;            // +0x34
    int BytesRead;          // +0x38
    char pad[0x24];      // +0x3C
    char Buffer[0x8000]; // +0x60
};

// El objetivo la alinea a 32: su .bss deja 28 B de relleno delante (0xC4 -> 0xE0),
// que es lo que pide DCInvalidateRange sobre Buffer. Sin esto el .bss sale 28 B corto.
static CURREAD gCurRead __attribute__((aligned(32)));

static void AyncDVDCallback(long, DVDFileInfo *);

static void QEndOp() {
    gCurRead.State = 5;
    AyncDVDCallback(0, 0);
}

static int AyncDVDRead(DVDFileInfo *FileInfo) {
    int nBytesToKeep;
    int nBytesToRead;

    nBytesToKeep = 0;

    if (gCurRead.State == 2) {

        nBytesToKeep = gCurRead.PreKeep;

        MEM_copy((void *)gCurRead.MemPointer, &gCurRead.Buffer[gCurRead.PreBytes], nBytesToKeep);
        gCurRead.FileBase += nBytesToKeep;
        gCurRead.MemPointer += nBytesToKeep;

    } else if (gCurRead.State == 3) {

        nBytesToKeep = gCurRead.MemEndAligned - gCurRead.MemPointer;

        DCInvalidateRange((void *)gCurRead.MemPointer, nBytesToKeep);
        gCurRead.FileBase += nBytesToKeep;
        gCurRead.MemPointer += nBytesToKeep;

    } else if (gCurRead.State == 4) {

        nBytesToKeep = gCurRead.MemEnd - gCurRead.MemPointer;
        if (nBytesToKeep > 0x4000) {
            nBytesToKeep = 0x4000;
        }
        MEM_copy((void *)gCurRead.MemPointer, gCurRead.Buffer, nBytesToKeep);
        gCurRead.FileBase += nBytesToKeep;
        gCurRead.MemPointer += nBytesToKeep;

    } else if (gCurRead.State == 1) {

        nBytesToKeep = gCurRead.Size;

        MEM_copy((void *)gCurRead.MemPointer, &gCurRead.Buffer[gCurRead.PreBytes], nBytesToKeep);
        gCurRead.State = 1;

        goto endop;
    }

    if (gCurRead.MemPointer == gCurRead.MemStartAligned &&
        gCurRead.MemPointer < gCurRead.MemEndAligned) {

        gCurRead.State = 3;

        gCurRead.BytesRead = DVDReadAsyncPrio(FileInfo, (void *)gCurRead.MemPointer,
                                              gCurRead.MemEndAligned - gCurRead.MemPointer,
                                              gCurRead.FileBase, AyncDVDCallback, 2);

    } else if (gCurRead.MemEnd > gCurRead.MemPointer) {

        nBytesToRead = ((gCurRead.MemEnd - gCurRead.MemPointer) + 31) & ~31;
        if (nBytesToRead > 0x4000) {
            nBytesToRead = 0x4000;
        }

        gCurRead.State = 4;

        gCurRead.BytesRead += DVDReadAsyncPrio(FileInfo, gCurRead.Buffer, nBytesToRead,
                                               gCurRead.FileBase, AyncDVDCallback, 2);

    } else {

    endop:
        QEndOp();
    }

    return nBytesToKeep;
}

/* RESUELTO r46 -- 440 B al 100 %. Faltaban SEIS `mr`: dos copias de entrada
   (`mr r28,r3` y `mr r7,r4`) y cuatro de argumento antes de las llamadas. El
   DWARF ya decia el reparto del original --FileInfo=r28, MemPointer=r7,
   FileBase=r12, Size=r6-- y r36e concluyo que la causa era mantener FileInfo
   vivo hasta el final; midio siete barreras sobre FileInfo y ninguna paso de
   436 B.

   La causa era la OTRA punta: **MemPointer**. Con `(int)MemPointer` repetido
   cinco veces GCC deja el parametro en r4 y usa r3 como temporal, asi que
   FileInfo se queda en r3 y no hacen falta copias. Cacheando el cast en una
   local, el parametro sale de r4 --a r7, como el original-- y a partir de ahi
   GCC pone FileInfo en r28 por su cuenta y emite las seis `mr`. Cero asm.

   Medido en esta ronda: cachear MemPointer con `register void * asm("r7")`
   deja 444 B / 98,618 % (el pin obliga una copia de mas por cruzar el `QEndOp`
   del retorno temprano); pinchar FileInfo a r28 solo, 424 B / 95,164 %; los
   dos pines a la vez, 432 B / 95,191 %; el pin en r8, 444 B / 98,300 %; el pin
   tras el retorno temprano, 444 B / 85,891 %. Sin pin: `void *mp = MemPointer`
   y `int memPointer = (int)MemPointer` dan las dos 440 B / 100 %. Reordenar el
   grupo `gCurRead.Mem*` o autoasignar el parametro no mueven nada (416 B).

   Nota de fidelidad: el DWARF del original solo nombra `readSize` y el bloque
   anonimo con `sizealigned`, asi que esta local es una RECONSTRUCCION --el
   original saco MemPointer de r4 de otra forma-- pero el objeto sale exacto. */
static void StartNonAlignedAyncRead(DVDFileInfo *FileInfo, void *MemPointer, long FileBase,
                                    long Size) {
    int readSize;
    int memPointer = (int)MemPointer;

    if (Size == 0) {

        QEndOp();

        return;
    }

    gCurRead.Size = Size;
    gCurRead.FileBase = FileBase;
    gCurRead.MemPointer = memPointer;

    gCurRead.MemStart = memPointer & ~31;
    gCurRead.MemStartAligned = (memPointer + 31) & ~31;
    gCurRead.MemEnd = memPointer + Size;
    gCurRead.MemEndAligned = (memPointer + Size) & ~31;

    gCurRead.FileStart = FileBase & ~31;
    gCurRead.FileStartAligned = (FileBase + 31) & ~31;
    gCurRead.FileEnd = FileBase + Size;
    gCurRead.FileEndAligned = (FileBase + Size) & ~31;

    gCurRead.BytesRead = 0;

    gCurRead.PreBytes = FileBase - gCurRead.FileStart;
    gCurRead.PreKeep = 32 - gCurRead.PreBytes;

    if (Size <= 31) {
        int sizealigned = ((gCurRead.FileEnd - gCurRead.FileStart) + 31) & ~31;

        gCurRead.State = 1;

        gCurRead.BytesRead =
            DVDReadAsyncPrio(FileInfo, gCurRead.Buffer, sizealigned, gCurRead.FileStart,
                             AyncDVDCallback, 2);

    } else if (gCurRead.PreBytes != 0) {

        gCurRead.State = 2;
        readSize = (gCurRead.PreKeep + 31) & ~31;

        gCurRead.BytesRead +=
            DVDReadAsyncPrio(FileInfo, gCurRead.Buffer, readSize, gCurRead.FileStart,
                             AyncDVDCallback, 2);

    } else if (gCurRead.MemPointer == gCurRead.MemStartAligned &&
               gCurRead.MemStartAligned < gCurRead.MemEndAligned) {

        gCurRead.State = 3;

        gCurRead.BytesRead = DVDReadAsyncPrio(FileInfo, (void *)gCurRead.MemStartAligned,
                                              gCurRead.MemEndAligned - gCurRead.MemStartAligned,
                                              gCurRead.FileBase, AyncDVDCallback, 2);

    } else if (gCurRead.MemEnd > gCurRead.MemPointer) {

        readSize = ((gCurRead.MemEnd - gCurRead.MemPointer) + 31) & ~31;
        if (readSize > 0x4000) {
            readSize = 0x4000;
        }

        gCurRead.State = 4;

        gCurRead.BytesRead += DVDReadAsyncPrio(FileInfo, gCurRead.Buffer, readSize,
                                               gCurRead.FileBase, AyncDVDCallback, 2);
    }
}

static void AyncDVDCallback(long, DVDFileInfo *) {
    OSSendMessage(&ReadFileThreadMsgQ, (void *)1, 1);
}

DvdFileHandle *GcDvdFileDeviceDriver::_AllocateDvdFileHandle() {
    DvdFileHandle *dvd_fh;

    dvd_fh = mFreeHandleQueue.Pop();

    return dvd_fh;
}

void GcDvdFileDeviceDriver::_FreeDvdFileHandle(DvdFileHandle *dvd_fh) {
    MEM_clear(dvd_fh, sizeof(DvdFileHandle));
    mFreeHandleQueue.Push(dvd_fh);
}

bool GcDvdFileDeviceDriver::Init() {
    DvdFileHandle *dvd_fh;

    mFileHandleMemory = (DvdFileHandle *)gFileSysOpts.allocator->Alloc(
        gFileSysOpts.MaxOpenFiles * sizeof(DvdFileHandle),
        TagValuePair(1, (const void *)"DVD File Handles") + TagValuePair(4, 1));

    MEM_clear(mFileHandleMemory, gFileSysOpts.MaxOpenFiles * sizeof(DvdFileHandle));

    dvd_fh = mFileHandleMemory;
    for (int iFiles = 0; iFiles < gFileSysOpts.MaxOpenFiles; iFiles++) {
        mFreeHandleQueue.Push(dvd_fh);

        dvd_fh++;
    }

    OSInitMessageQueue(&ReadFileThreadMsgQ, ReadFileThreadMsgData, 32);

    return true;
}

void GcDvdFileDeviceDriver::Restore() {
    gFileSysOpts.allocator->Free(mFileHandleMemory, 0);
}

EAFileHandle GcDvdFileDeviceDriver::Open(const char *filename, int, int *) {
    char newname[256];
    DvdFileHandle *dvd_fh;
    char *name;
    const char *namesrc;
    int ret;
    int entryNum;

    dvd_fh = _AllocateDvdFileHandle();

    name = newname;

    namesrc = filename;
    while (*namesrc != 0) {

        if (*namesrc == '\\') {

            *name++ = '/';

        } else {

            *name++ = *namesrc;
        }
        namesrc++;
    }
    *name = 0;
    name = newname;

    if (strncmp(name, "dvd:", 4) == 0) {
        name = &newname[4];
    }

    ret = 0;
    entryNum = DVDConvertPathToEntrynum(name + 1);
    if (entryNum != -1) {

        ret = DVDFastOpen(entryNum, &dvd_fh->fileInfo);
    }

    if (ret == 0) {
        Close((EAFileHandle)dvd_fh);

        return -1;
    }

    dvd_fh->size = dvd_fh->fileInfo.length;
    return (EAFileHandle)dvd_fh;
}

void GcDvdFileDeviceDriver::Close(EAFileHandle h) {
    DvdFileHandle *dvd_fh = (DvdFileHandle *)h;

    DVDClose(&dvd_fh->fileInfo);

    _FreeDvdFileHandle(dvd_fh);
}

uint32_t GcDvdFileDeviceDriver::Read(EAFileHandle h, void *buf, unsigned int bufsize,
                                     DeviceDriver *, EAFileHandle) {
    DvdFileHandle *dvd_fh = (DvdFileHandle *)h;
    void *msg;
    int nBytesKept;

    if ((int)bufsize > dvd_fh->size - dvd_fh->offset) {

        bufsize = dvd_fh->size - dvd_fh->offset;

        if (dvd_fh->offset >= dvd_fh->size) {

            bufsize = 0;
        }
    }

    StartNonAlignedAyncRead(&dvd_fh->fileInfo, buf, dvd_fh->offset, bufsize);

    OSReceiveMessage(&ReadFileThreadMsgQ, &msg, 1);

    nBytesKept = 0;
    while (gCurRead.State != 5) {

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

uint64_t GcDvdFileDeviceDriver::Seek(EAFileHandle h, uint64_t offset, int whence,
                                     DeviceDriver *ddParent, EAFileHandle ddFileHandle) {
    DvdFileHandle *dvd_fh = (DvdFileHandle *)h;

    switch (whence) {
    case 0:
        dvd_fh->offset = offset;
        break;

    case 1:
        dvd_fh->offset += offset;
        break;

    case 2:
        dvd_fh->offset = Getsize(h) - offset;
        break;
    }

    return offset;
}

uint64_t GcDvdFileDeviceDriver::Getsize(EAFileHandle h) {
    return ((DvdFileHandle *)h)->size;
}

uint64_t GcDvdFileDeviceDriver::QueryLocation(EAFileHandle h) {
    return 0;
}
