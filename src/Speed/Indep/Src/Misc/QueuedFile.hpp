//
//
//
//
//
//
//
//
//
#ifndef QUEUEDFILE_HPP
#define QUEUEDFILE_HPP

#include "types.h"

extern int QueuedFileDefaultPriority; // Decl: 14

// total size: 0x10
// Decl: 17
struct QueuedFileParams {
    // Decl: 18
    QueuedFileParams()
        : Priority(QueuedFileDefaultPriority), //
          BlockSize(0x7ffffff),                //
          Compressed(false),                   //
          UncompressedSize(0) {}

    int Priority;         // offset 0x0, size 0x4, Decl: 27
    int BlockSize;        // offset 0x4, size 0x4, Decl: 28
    bool Compressed;      // offset 0x8, size 0x1, Decl: 29
    int UncompressedSize; // offset 0xC, size 0x4, Decl: 30
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

extern int EnableQueuedFileBundle; // size: 0x4, Decl: 75

#endif
