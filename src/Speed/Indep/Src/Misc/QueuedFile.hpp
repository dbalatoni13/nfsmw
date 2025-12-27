#ifndef MISC_QUEUED_FILE_H
#define MISC_QUEUED_FILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

void InitQueuedFiles();
void AddQueuedFile2(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(void *, int, void *), void *callback_param,
                    void *callback_param2, struct QueuedFileParams *params);

#endif
