#ifndef MISC_BFILE_H
#define MISC_BFILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern SlotPool *bFileSlotPool;

enum bFileOpenMode {
    BOPEN_MODE_APPEND = 2,
    BOPEN_MODE_WRITE = 6,
    BOPEN_MODE_READONLY = 1,
};

// total size: 0x14
struct MemoryFileEntry {
    unsigned int Hash;       // offset 0x0, size 0x4
    int Offset;              // offset 0x4, size 0x4
    int FileSize;            // offset 0x8, size 0x4
    int MemorySize;          // offset 0xC, size 0x4
    unsigned char *Data;     // offset 0x10, size 0x4
};

// total size: 0x28010
struct MemoryFile : public bTNode<MemoryFile> {
    unsigned int Magic;                      // offset 0x8, size 0x4
    int NumFileEntries;                      // offset 0xC, size 0x4
    MemoryFileEntry FileEntries[8192];       // offset 0x10, size 0x28000
};

// total size: 0x18
struct CachedRealFileHandle : public bTNode<CachedRealFileHandle> {
    static void *operator new(unsigned int size) {
        return CachedRealFileHandleSlotPool->Malloc();
    }

    static void operator delete(void *ptr) {
        CachedRealFileHandleSlotPool->Free(ptr);
    }

    CachedRealFileHandle(const char *filename, int file_handle, int file_size)
        : NumReferences(0), //
          FileSize(file_size), //
          FileHandle(file_handle) {}

    ~CachedRealFileHandle() {}

    int GetFileHandle() { return FileHandle; }
    int GetFileSize() { return FileSize; }
    void AddReference() { NumReferences++; }
    void RemoveReference() { NumReferences--; }

    static CachedRealFileHandle *FindHandle(const char *filename);
    static CachedRealFileHandle *AddHandle(const char *filename, int file_handle, int file_size);
    static bool RemoveUnusedHandle();
    static void FlushUnusedHandle(const char *filename);
    static void FlushUnusedHandles(bool print_warning);

    static int NumInstances;
    static bTList<CachedRealFileHandle> HandleList;
    static SlotPool *CachedRealFileHandleSlotPool;

    int NumReferences;       // offset 0x8, size 0x4
    int FileHandle;          // offset 0xC, size 0x4
    int FileSize;            // offset 0x10, size 0x4
    const char *Filename;    // offset 0x14, size 0x4
};

struct bFile : public bTNode<bFile> {
    // total size: 0x38
    bFile(const char *filename, bFileOpenMode open_mode);
    ~bFile();

    static void *operator new(unsigned int size) {
        return bFileSlotPool->Malloc();
    }

    static void operator delete(void *ptr) {
        bFileSlotPool->Free(ptr);
    }

    bool IsOpen() { return FileSize >= 0; }

    void OpenLowLevel();
    void MaybeAddCachedHandle();
    void Seek(int position, int mode);
    void ReadAsync(void *buf, int num_bytes, void (*callback)(void *), void *callback_param);
    void FlushWriteBuffer();
    void Write(const void *buf, int num_bytes);

    static void CallbackFunctionOpen(int fop, int status, void *userdata);
    static void CallbackFunctionRead(int fop, int status, void *userdata);
    static void HandleCompletedCallbacks();

    bFileOpenMode OpenMode;                             // offset 0x8, size 0x4
    int FileSize;                                       // offset 0xC, size 0x4
    int Position;                                       // offset 0x10, size 0x4
    int FileHandle;                                     // offset 0x14, size 0x4
    CachedRealFileHandle *pCachedRealFileHandle;        // offset 0x18, size 0x4
    int CloseAfterCallbacks;                            // offset 0x1C, size 0x4
    int NumPendingCallbacks;                            // offset 0x20, size 0x4
    const char *Filename;                               // offset 0x24, size 0x4
    int WriteBufferPos;                                 // offset 0x28, size 0x4
    int WriteBufferNumBytes;                            // offset 0x2C, size 0x4
    int WriteBufferSize;                                // offset 0x30, size 0x4
    unsigned char *WriteBuffer;                         // offset 0x34, size 0x4
};

bFile *bOpen(const char *filename, int open_mode, int warn_if_cant_open);
void bClose(bFile *f);
void bRead(bFile *f, void *buf, int numbytes);
void bSeek(bFile *f, int position, int mode);
int bFileSize(bFile *f);
int bFileSize(const char *filename);
void bAppendToFile(const char *filename, void *buf, int num_bytes);
void *bGetFile(const char *filename, int *size, int allocation_params);
int bFileExists(const char *f);
int bFPrintf(bFile *file, const char *fmt, ...);
void bFileFlushCachedFiles();
void bFileFlushCacheFile(const char *filename);
unsigned int bFileGetFilenameHash(const char *filename);
int GetRealFileOpenFlags(bFileOpenMode open_mode);
void AddMemoryFile(void *pmemory_file);
void RemoveMemoryFile(void *pmemory_file);
MemoryFileEntry *FindMemoryFileEntry(const char *filename);
void AsyncCloseFileCallback(int fop, int status, void *userdata);
void AsyncCloseFile(int file_handle);

void ServiceFileStats();
bool bIsMainThread();
void bThreadYield(int a);
void bSyncTaskRun();
void bFileRunTimingTest();
void bReadAsync(bFile *f, void *buf, int numbytes, void (*callback)(void *), void *param);
void bServiceFileSystem();
void bWrite(bFile *f, const void *buf, int num_bytes);
bool bIsAsyncDone(bFile *f);
void bWaitUntilAsyncDone(bFile *f);
void bInitFileSystem();

namespace RealFile {

// total size: 0x14
struct DeviceDriver {
    char mDeviceName[16];   // offset 0x0, size 0x10

    DeviceDriver(const char *name);
    virtual ~DeviceDriver() {}
    virtual bool Init() { return true; }
    virtual void Restore() {}
    virtual int Open(const char *name, int oflags, int *pParentFileHandle) { return 0; }
    virtual void Close(int h) {}
    virtual unsigned int Read(int h, void *buf, unsigned int bufsize, DeviceDriver *ddParent, int ddFileHandle) { return 0; }
    virtual unsigned int Write(int h, const void *buf, unsigned int bufsize, DeviceDriver *ddParent, int ddFileHandle) { return 0; }
    virtual unsigned long long Seek(int h, unsigned long long offset, int whence, DeviceDriver *ddParent, int ddFileHandle) { return 0; }
    virtual unsigned long long Getsize(int h) { return 0; }
    virtual unsigned long long QueryLocation(int h) { return 0; }
    virtual bool Remove(const char *name) { return false; }
    virtual unsigned long long Getspace() { return 0; }
    virtual const char *GetName() { return mDeviceName; }
    virtual unsigned int GetOptimalReadSize() { return 0; }
};

void AddDevice(DeviceDriver *device);
void AddSearchLocation(const char *location, bool recursive);

} // namespace RealFile

// total size: 0x18
struct bFileDirectoryEntry {
    unsigned int Hash;            // offset 0x0, size 0x4
    int FileNumber;               // offset 0x4, size 0x4
    int LocalSectorOffset;        // offset 0x8, size 0x4
    int TotalSectorOffset;        // offset 0xC, size 0x4
    int Size;                     // offset 0x10, size 0x4
    unsigned int Checksum;        // offset 0x14, size 0x4
};

// total size: 0x28
struct OpenDisculatorFile {
    static void *operator new(unsigned int size);
    static void operator delete(void *ptr);
    const char *filename;         // offset 0x0, size 0x4
    int nameHash;                 // offset 0x4, size 0x4
    int giantFileNum;             // offset 0x8, size 0x4
    int localSectorOffset;        // offset 0xC, size 0x4
    int totalSectorOffset;        // offset 0x10, size 0x4
    unsigned long long size;      // offset 0x18, size 0x8
    unsigned long long seekPos;   // offset 0x20, size 0x8
};

// total size: 0x81C
struct DisculatorDriver : public RealFile::DeviceDriver {
    DisculatorDriver() : DeviceDriver("DVDV") {}

    static DisculatorDriver *Get() { return sDisculatorDriver; }

    char *GetGiantDataFileName(int file_number) {
        return GiantDataFileName[file_number];
    }

    ~DisculatorDriver() override;
    static DisculatorDriver *Create(const char *dir_filename, const char *data_filename);
    bool Init() override;
    void Restore() override;
    int Open(const char *name, int oflags, int *pParentFileHandle) override;
    void Close(int h) override;
    unsigned int Read(int h, void *buf, unsigned int bufsize, RealFile::DeviceDriver *ddParent, int ddFileHandle) override;
    unsigned int Write(int h, const void *buf, unsigned int bufsize, RealFile::DeviceDriver *ddParent, int ddFileHandle) override;
    unsigned long long Seek(int h, unsigned long long offset, int whence, RealFile::DeviceDriver *ddParent, int ddFileHandle) override;
    unsigned long long Getsize(int h) override;
    unsigned long long QueryLocation(int h) override;
    bool Remove(const char *name) override;
    unsigned long long Getspace() override;

    bool LoadGiantFiles(const char *giant_dir_filename, const char *giant_data_filename_base);
    bFileDirectoryEntry *FindDirectoryEntry(const char *filename);

    static DisculatorDriver *sDisculatorDriver;

    bFileDirectoryEntry *pDirectoryEntryTable;     // offset 0x14, size 0x4
    int NumDirectoryEntries;                        // offset 0x18, size 0x4
    char GiantDataFileName[30][64];                 // offset 0x1C, size 0x780
    int GiantDataFileHandle[30];                    // offset 0x79C, size 0x78
    int CurrentSector;                              // offset 0x814, size 0x4
    int TotalDeltaSector;                           // offset 0x818, size 0x4
};

bool bInitDisculatorDriver(const char *dir_filename, const char *data_filename);
extern SlotPool *OpenDisculatorFileSlotPool;

#endif
