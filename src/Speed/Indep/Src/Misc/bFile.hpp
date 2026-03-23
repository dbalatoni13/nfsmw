#ifndef MISC_BFILE_H
#define MISC_BFILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/file/driver.h"

extern SlotPool *bFileSlotPool;

enum bFileOpenMode {
    BOPEN_MODE_APPEND = 2,
    BOPEN_MODE_WRITE = 6,
    BOPEN_MODE_READONLY = 1,
};

// total size: 0x14
struct MemoryFileEntry {
    uint32 Hash;      // offset 0x0, size 0x4
    int32 Offset;     // offset 0x4, size 0x4
    int32 FileSize;   // offset 0x8, size 0x4
    int32 MemorySize; // offset 0xC, size 0x4
    uint8 *Data;      // offset 0x10, size 0x4
};

// total size: 0x28010
class MemoryFile : public bTNode<MemoryFile> {
  public:
    uint32 Magic;                      // offset 0x8, size 0x4
    int32 NumFileEntries;              // offset 0xC, size 0x4
    MemoryFileEntry FileEntries[8192]; // offset 0x10, size 0x28000
};

// total size: 0x18
class CachedRealFileHandle : public bTNode<CachedRealFileHandle> {
  public:
    void *operator new(size_t size) {
        return bOMalloc(bFileSlotPool);
    }

    // TODO bFree, why doesn't it get inlined in CachedRealFileHandle::RemoveUnusedHandle?
    void operator delete(void *ptr);

    CachedRealFileHandle(const char *filename, int file_handle, int file_size) {
        NumInstances++;
        NumReferences = 0;
        FileHandle = file_handle;
        FileSize = file_size;
        Filename = bAllocateSharedString(filename);
    }

    ~CachedRealFileHandle() {}

    int GetFileHandle() {
        return FileHandle;
    }

    int GetFileSize() {
        return FileSize;
    }

    void AddReference() {
        NumReferences++;
    }

    void RemoveReference() {
        NumReferences--;
    }

    static CachedRealFileHandle *FindHandle(const char *filename);
    static CachedRealFileHandle *AddHandle(const char *filename, int file_handle, int file_size);
    static bool RemoveUnusedHandle();
    static void FlushUnusedHandle(const char *filename);
    static void FlushUnusedHandles(bool print_warning);

    static int NumInstances;
    static bTList<CachedRealFileHandle> HandleList;

    int NumReferences;    // offset 0x8, size 0x4
    int FileHandle;       // offset 0xC, size 0x4
    int FileSize;         // offset 0x10, size 0x4
    const char *Filename; // offset 0x14, size 0x4
};

// total size: 0x24
class bFileCallbackEntry : public bTNode<bFileCallbackEntry> {
  public:
    void *operator new(size_t size) {
        return bOMalloc(bFileSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(bFileSlotPool, ptr);
    }

    bFileCallbackEntry(class bFile *file, void *buf, int position, int num_bytes, void (*callback)(void *), void *callback_param)
        : File(file),                    //
          Callback(callback),            //
          FileHandleToClose(0),          //
          Position(position),            //
          CallbackParam(callback_param), //
          Buf(buf),                      //
          NumBytes(num_bytes)            //
    {}

    bFile *File;                    // offset 0x8, size 0x4
    void *Buf;                      // offset 0xC, size 0x4
    int Position;                   // offset 0x10, size 0x4
    int NumBytes;                   // offset 0x14, size 0x4
    void (*Callback)(void *);       // offset 0x18, size 0x4
    void *CallbackParam;            // offset 0x1C, size 0x4
    EAFileHandle FileHandleToClose; // offset 0x20, size 0x4
};

// total size: 0x38
class bFile : public bTNode<bFile> {
  public:
    bFile(const char *filename, bFileOpenMode open_mode);
    ~bFile();

    void *operator new(size_t size) {
        return bOMalloc(bFileSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(bFileSlotPool, ptr);
    }

    bool IsOpen() {
        return FileSize >= 0;
    }

    int GetNumPendingCallbacks() {
        return NumPendingCallbacks;
    }

    void SetCloseAfterCallbacks() {
        CloseAfterCallbacks = true;
    }

    int GetFileSize() {
        return FileSize;
    }

    static int GetTotalNumPendingCallbacks() {
        return TotalNumPendingCallbacks;
    }

    void OpenLowLevel();
    void MaybeAddCachedHandle();
    void Seek(int position, int mode);
    void ReadAsync(void *buf, int num_bytes, void (*callback)(void *), void *callback_param);
    void FlushWriteBuffer();
    void Write(const void *buf, int num_bytes);

    static void CallbackFunctionOpen(int fop, int status, void *userdata);
    static void CallbackFunctionRead(int fop, int status, void *userdata);
    static void HandleCompletedCallbacks();

  private:
    bFileOpenMode OpenMode;                                  // offset 0x8, size 0x4
    int FileSize;                                            // offset 0xC, size 0x4
    int Position;                                            // offset 0x10, size 0x4
    EAFileHandle FileHandle;                                 // offset 0x14, size 0x4
    CachedRealFileHandle *pCachedRealFileHandle;             // offset 0x18, size 0x4
    int CloseAfterCallbacks;                                 // offset 0x1C, size 0x4
    int NumPendingCallbacks;                                 // offset 0x20, size 0x4
    const char *Filename;                                    // offset 0x24, size 0x4
    static int TotalNumPendingCallbacks;                     // size: 0x4, address: 0x8041EA98
    static bTList<bFileCallbackEntry> PendingCallbackList;   // size: 0x8, address: 0x8048001C
    static bTList<bFileCallbackEntry> CompletedCallbackList; // size: 0x8, address: 0x80480024
    int WriteBufferPos;                                      // offset 0x28, size 0x4
    int WriteBufferNumBytes;                                 // offset 0x2C, size 0x4
    int WriteBufferSize;                                     // offset 0x30, size 0x4
    uint8 *WriteBuffer;                                      // offset 0x34, size 0x4
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

// total size: 0x18
struct bFileDirectoryEntry {
    uint32 Hash;             // offset 0x0, size 0x4
    int32 FileNumber;        // offset 0x4, size 0x4
    int32 LocalSectorOffset; // offset 0x8, size 0x4
    int32 TotalSectorOffset; // offset 0xC, size 0x4
    int32 Size;              // offset 0x10, size 0x4
    uint32 Checksum;         // offset 0x14, size 0x4
};

extern SlotPool *OpenDisculatorFileSlotPool;

class FileStats {
  public:
    void AddStatEntry(const char *filename, int seek_sector, int read_size, void *read_buf) {
        // TODO based on Undercover
    }

    void CaptureTimings() {}
};

// TODO move?
// total size: 0x28
struct OpenDisculatorFile {
    void *operator new(size_t size) {
        return bOMalloc(OpenDisculatorFileSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(OpenDisculatorFileSlotPool, ptr);
    }

    OpenDisculatorFile(bFileDirectoryEntry &dirEntry, const char *_filename)
        : nameHash(dirEntry.Hash),                       //
          giantFileNum(dirEntry.FileNumber),             //
          localSectorOffset(dirEntry.LocalSectorOffset), //
          totalSectorOffset(dirEntry.TotalSectorOffset), //
          seekPos(0),                                    //
          size(dirEntry.Size),                           //
          filename(_filename) {}

    const char *filename;       // offset 0x0, size 0x4
    int nameHash;               // offset 0x4, size 0x4
    int giantFileNum;           // offset 0x8, size 0x4
    int localSectorOffset;      // offset 0xC, size 0x4
    int totalSectorOffset;      // offset 0x10, size 0x4
    unsigned long long size;    // offset 0x18, size 0x8
    unsigned long long seekPos; // offset 0x20, size 0x8
};

// total size: 0x81C
class DisculatorDriver : public RealFile::DeviceDriver {
  public:
    DisculatorDriver() : DeviceDriver("discu:") {}

    static DisculatorDriver *Get() {
        return sDisculatorDriver;
    }

    char *GetGiantDataFileName(int file_number) {
        return GiantDataFileName[file_number];
    }

    ~DisculatorDriver() override;
    static DisculatorDriver *Create(const char *dir_filename, const char *data_filename);
    bool Init() override;
    void Restore() override;
    EAFileHandle Open(const char *name, int oflags, int *pParentFileHandle) override;
    void Close(EAFileHandle h) override;
    uint32_t Read(EAFileHandle h, void *buf, unsigned int bufsize, RealFile::DeviceDriver *ddParent, EAFileHandle ddFileHandle) override;
    uint32_t Write(EAFileHandle h, const void *buf, unsigned int bufsize, RealFile::DeviceDriver *ddParent, EAFileHandle ddFileHandle) override;
    uint64_t Seek(EAFileHandle h, unsigned long long offset, int whence, RealFile::DeviceDriver *ddParent, EAFileHandle ddFileHandle) override;
    uint64_t Getsize(EAFileHandle h) override;
    uint64_t QueryLocation(EAFileHandle h) override;
    bool Remove(const char *name) override;
    uint64_t Getspace() override;

    bool LoadGiantFiles(const char *giant_dir_filename, const char *giant_data_filename_base);
    bFileDirectoryEntry *FindDirectoryEntry(const char *filename);

  private:
    static DisculatorDriver *sDisculatorDriver;

    bFileDirectoryEntry *pDirectoryEntryTable; // offset 0x14, size 0x4
    int NumDirectoryEntries;                   // offset 0x18, size 0x4
    char GiantDataFileName[30][64];            // offset 0x1C, size 0x780
    int GiantDataFileHandle[30];               // offset 0x79C, size 0x78
    int CurrentSector;                         // offset 0x814, size 0x4
    int TotalDeltaSector;                      // offset 0x818, size 0x4
};

bool bInitDisculatorDriver(const char *dir_filename, const char *data_filename);

#endif
