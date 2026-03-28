#ifndef MISC_QUEUED_FILE_H
#define MISC_QUEUED_FILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern SlotPool *QueuedFileSlotPool;
extern int QueuedFileDefaultPriority;

// total size: 0x10
struct QueuedFileParams {
    QueuedFileParams()
        : Priority(QueuedFileDefaultPriority), //
          BlockSize(0x7ffffff),                //
          Compressed(false),                   //
          UncompressedSize(0) {}

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

    static void *operator new(std::size_t size, void *ptr) {
        return ptr;
    }

    void operator delete(void *ptr) {
        bFree(QueuedFileSlotPool, ptr);
    }

    // static int SortBySeekPosition(QueuedFile *before, QueuedFile *after) {}

    static int SortByPriority(QueuedFile *before, QueuedFile *after) {
        return before->Params.Priority >= after->Params.Priority;
    }

    static int GetNumFilesDecompressing() {
        return DecompressionTableTop - DecompressionTableBot;
    }

    static void ReadDoneCallback(void *param) {
        static_cast<QueuedFile *>(param)->ReadDoneCallback();
    }

    int GetHandle() {
        return Handle;
    }

    const char *GetFilename() {
        return Filename;
    }

    int GetFilePos() {
        return FilePos;
    }

    int GetNumBytes() {
        return NumBytes;
    }

    int GetNumRead() {
        return NumRead;
    }

    void *GetBuffer() {
        return pBuf;
    }

    // float GetStartReadTime() {}

    int GetPriority() {
        return Params.Priority;
    }

    QueuedFileStatus GetStatus() {
        return Status;
    }

    void SetStatus(QueuedFileStatus status) {
        Status = status;
    }

    int IsFinishedAllReading() {
        return NumRead == NumBytes;
    }

    unsigned int CalculateChecksum() {
        return bCalculateCrc32(pBuf, NumBytes, 0xFFFFFFFF);
    }

    void *GetCallback() {
        return CallbackFunction;
    }

    void SetCallbackParam2(void *param) {
        CallbackParam2 = param;
        CallbackModeUseParam2 = true;
    }

    void CallDoneCallback(int error_status) {
        if (CallbackFunction) {
            if (CallbackModeUseParam2) {
                ((void (*)(void *, int, void *))CallbackFunction)(CallbackParam, error_status, CallbackParam2);
            } else {
                ((void (*)(void *, int))CallbackFunction)(CallbackParam, error_status);
            }
        }
    }

    friend class QueuedFileBundle;

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
bool IsQueuedFileBusy();
void BlockWhileQueuedFileBusy();
void SetQueuedFileMinPriority(int priority);

inline void AddQueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(intptr_t, int), intptr_t callback_param,
                          QueuedFileParams *params) {
    AddQueuedFile(buf, filename, file_pos, num_bytes, reinterpret_cast<void (*)(void *, int)>(callback), reinterpret_cast<void *>(callback_param),
                  params);
}

// total size: 0x3C
class QueuedFileBundle {
  public:
    QueuedFileBundle() {
        ReadBuffer = 0;
        ReadBufferBot = 0;
        ReadBufferTop = 0;
        NumBytesQueued = 0;
        NumQueuedFiles = 0;
        MemoryPoolNumber = 0;
    }

    ~QueuedFileBundle() {}

    void *operator new(std::size_t size) {
        return bOMalloc(QueuedFileSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(QueuedFileSlotPool, ptr);
    }

    const char *GetFilename() {
        return QueuedFiles[0]->GetFilename();
    }

    static void ReadCallbackBridge(void *param, int error_status) {
        QueuedFileBundle *bundle = static_cast<QueuedFileBundle *>(param);
        bundle->ReadCallback(error_status);
        if (bundle) {
            if (bundle->ReadBuffer) {
                bFree(bundle->ReadBuffer);
            }
            delete bundle;
        }
    }

    bool TestAddQueuedFile(QueuedFile *q);
    void BeginRead();
    void ReadCallback(int error_status);

    int8 *ReadBuffer;            // offset 0x0, size 0x4
    int ReadBufferBot;           // offset 0x4, size 0x4
    int ReadBufferTop;           // offset 0x8, size 0x4
    int NumBytesQueued;          // offset 0xC, size 0x4
    int16 MemoryPoolNumber;      // offset 0x10, size 0x2
    int16 NumQueuedFiles;        // offset 0x12, size 0x2
    QueuedFile *QueuedFiles[10]; // offset 0x14, size 0x28
};

#endif
