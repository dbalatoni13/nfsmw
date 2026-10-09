//
//
//
//
#ifndef BFILE_HPP
#define BFILE_HPP // Decl: 6

#include "Speed/Indep/bWare/Inc/bList.hpp"

// Decl: 14
enum bFileOpenMode {
    BOPEN_MODE_READONLY = 1,
    BOPEN_MODE_WRITE = 6,
    BOPEN_MODE_APPEND = 2,
};

#define BOPEN_FLAG_READONLY (0x01)     // Decl: 22
#define BOPEN_FLAG_WRITE (0x02 + 0x04) // Decl: 23
#define BOPEN_FLAG_APPEND (0x02)       // Decl: 24

#define MAX_DISCULATOR_GIANT_FILES 30 // Decl: 76

// total size: 0x18
// Decl: 81
struct bFileDirectoryEntry {
    uint32 Hash;             // offset 0x0, size 0x4
    int32 FileNumber;        // offset 0x4, size 0x4
    int32 LocalSectorOffset; // offset 0x8, size 0x4
    int32 TotalSectorOffset; // offset 0xC, size 0x4
    int32 Size;              // offset 0x10, size 0x4
    uint32 Checksum;         // offset 0x14, size 0x4
};

// total size: 0x14
// Decl: 105
struct MemoryFileEntry {
    uint32 Hash;      // offset 0x0, size 0x4
    int32 Offset;     // offset 0x4, size 0x4
    int32 FileSize;   // offset 0x8, size 0x4
    int32 MemorySize; // offset 0xC, size 0x4
    uint8 *Data;      // offset 0x10, size 0x4
};

#define MEMORY_FILE_MAGIC 0x53219999 // Decl: 114

// total size: 0x28010
// Decl: 118
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

bool bInitDisculatorDriver(const char *dir_filename, const char *data_filename);

#endif
