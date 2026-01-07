#ifndef MISC_QUEUED_FILE_H
#define MISC_QUEUED_FILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

// total size: 0x10
struct QueuedFileParams {
    QueuedFileParams() {}

    int Priority;         // offset 0x0, size 0x4
    int BlockSize;        // offset 0x4, size 0x4
    bool Compressed;      // offset 0x8, size 0x1
    int UncompressedSize; // offset 0xC, size 0x4
};

enum QueuedFileStatus {
    QWAITING = 0,
    QREADING = 1,
    QDONE = 2,
    QERROR = 3,
};

// total size: 0x48
class QueuedFile : public bTNode<QueuedFile> {
  public:
    QueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void *callback_function, void *callback_param, QueuedFileParams *params);

    ~QueuedFile();

    char *GetDebugPrintHeader();

    void BeginRead();

    static void ServiceDecompression();

    void ReadDoneCallback();

    // void *operator new(size_t size) {}

    // void operator delete(void *ptr) {}

    // static int SortByPriority(QueuedFile *before, QueuedFile *after) {}

    // static int SortBySeekPosition(QueuedFile *before, QueuedFile *after) {}

    // static int GetNumFilesDecompressing() {}

    static void ReadDoneCallback(void *param) {}

    // int GetHandle() {}

    // const char *GetFilename() {}

    // int GetFilePos() {}

    // int GetNumBytes() {}

    // int GetNumRead() {}

    // void *GetBuffer() {}

    // float GetStartReadTime() {}

    // int GetPriority() {}

    // QueuedFileStatus GetStatus() {}

    void SetStatus(QueuedFileStatus status) {}

    // int IsFinishedAllReading() {}

    // unsigned int CalculateChecksum() {}

    // void *GetCallback() {}

    void SetCallbackParam2(void *param) {}

    void CallDoneCallback(int error_status) {}

  private:
    static int CurrentHandle;                  // size: 0x4, address: 0x8041EA64
    static int DecompressionTableBot;          // size: 0x4, address: 0x8041EA68
    static int DecompressionTableTop;          // size: 0x4, address: 0x8041EA6C
    static QueuedFile *DecompressionTable[32]; // size: 0x80, address: 0x804FE9CC

    void *pBuf;                // offset 0x8, size 0x4
    const char *Filename;      // offset 0xC, size 0x4
    int FilePos;               // offset 0x10, size 0x4
    int NumBytes;              // offset 0x14, size 0x4
    int NumRead;               // offset 0x18, size 0x4
    QueuedFileParams Params;   // offset 0x1C, size 0x10
    void *CallbackFunction;    // offset 0x2C, size 0x4
    void *CallbackParam;       // offset 0x30, size 0x4
    void *CallbackParam2;      // offset 0x34, size 0x4
    int CallbackModeUseParam2; // offset 0x38, size 0x4
    int Handle;                // offset 0x3C, size 0x4
    QueuedFileStatus Status;   // offset 0x40, size 0x4
    float StartReadTime;       // offset 0x44, size 0x4
};

void InitQueuedFiles();
void ServiceQueuedFiles();
void AddQueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(void *, int), void *callback_param,
                   QueuedFileParams *params);
void AddQueuedFile2(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(void *, int, void *), void *callback_param,
                    void *callback_param2, QueuedFileParams *params);
int GetQueuedFileSize(const char *filename);

inline void AddQueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(int, int), int callback_param,
                          QueuedFileParams *params) {
    AddQueuedFile(buf, filename, file_pos, num_bytes, reinterpret_cast<void (*)(void *, int)>(callback), reinterpret_cast<void *>(callback_param),
                  params);
}

#endif
