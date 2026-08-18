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

class bFile;

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

bool bInitDisculatorDriver(const char *dir_filename, const char *data_filename);

#endif
