#ifndef MISC_BFILE_H
#define MISC_BFILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

enum bFileOpenMode {
    BOPEN_MODE_APPEND = 2,
    BOPEN_MODE_WRITE = 6,
    BOPEN_MODE_READONLY = 1,
};

struct bFile : public bTNode<bFile> {
    // total size: 0x38
    bFileOpenMode OpenMode;                             // offset 0x8, size 0x4
    int FileSize;                                       // offset 0xC, size 0x4
    int Position;                                       // offset 0x10, size 0x4
    int FileHandle;                                     // offset 0x14, size 0x4
    struct CachedRealFileHandle *pCachedRealFileHandle; // offset 0x18, size 0x4
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
int bFileSize(bFile *f);
int bFileSize(const char *filename);

void *bGetFile(const char *filename, int *size, int allocation_params);
int bFileExists(const char *f);
int bFPrintf(bFile *file, const char *fmt, ...);
void bFileFlushCachedFiles();

void ServiceFileStats();

void bThreadYield(int a);

void bSyncTaskRun();

#endif
