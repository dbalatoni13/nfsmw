#ifndef MISC_QUEUED_FILE_H
#define MISC_QUEUED_FILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x10
struct QueuedFileParams {
    QueuedFileParams() {}

    int Priority;         // offset 0x0, size 0x4
    int BlockSize;        // offset 0x4, size 0x4
    bool Compressed;      // offset 0x8, size 0x1
    int UncompressedSize; // offset 0xC, size 0x4
};

void InitQueuedFiles();
void ServiceQueuedFiles();
void AddQueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(void *, int), void *callback_param,
                   QueuedFileParams *params);
void AddQueuedFile2(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(void *, int, void *), void *callback_param,
                    void *callback_param2, struct QueuedFileParams *params);
int GetQueuedFileSize(const char *filename);

inline void AddQueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(int, int), int callback_param,
                          QueuedFileParams *params) {
    AddQueuedFile(buf, filename, file_pos, num_bytes, reinterpret_cast<void (*)(void *, int)>(callback), reinterpret_cast<void *>(callback_param),
                  params);
}

#endif
