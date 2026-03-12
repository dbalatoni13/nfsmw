#include "./bFile.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <types.h>

extern bTList<bFile> bFileList;
extern int bFileNumInstances;
extern bTList<MemoryFile> MemoryFileList;

const char *bAllocateSharedString(const char *str);
void bFreeSharedString(const char *str);
void *bMalloc(int size, int allocation_params);
void bFree(void *ptr);
int bCountFreeMemory(int heap);
int bLargestMalloc(int heap);

void SYNCTASK_run();
void THREAD_yield(int a);
bool THREAD_iscurrent(void *thread);

int FILESYS_completeop(int fop);
int FILESYS_open(const char *filename, int flags, void (*callback)(int, int, void *), void *userdata);
int FILESYS_read(int fop, void *buf, int nbytes, void (*callback)(int, int, void *), void *userdata);

unsigned int bStringHash(const char *str);
void bEndianSwap32(void *value);
char bToUpper(char c);

int GetQueuedFileSize(const char *filename);

void bSyncTaskRun() {
    SYNCTASK_run();
}

void bThreadYield(int a) {
    THREAD_yield(a);
}

bool bIsMainThread() {
    return THREAD_iscurrent(nullptr);
}

unsigned int bFileGetFilenameHash(const char *filename) {
    char mangled_name[160];
    int len = bStrLen(filename);
    int n = 0;
    for (; n < len; n++) {
        char c = bToUpper(filename[n]);
        if (c == '/') {
            c = '\\';
        }
        mangled_name[n] = c;
    }
    mangled_name[n] = 0;
    return bStringHash(mangled_name);
}

void ServiceFileStats() {}

int GetRealFileOpenFlags(bFileOpenMode open_mode) {
    switch (open_mode) {
    case BOPEN_MODE_WRITE:
        return 6;
    case BOPEN_MODE_APPEND:
        return 2;
    default:
        return 1;
    }
}

void AddMemoryFile(void *pmemory_file) {
    MemoryFile *memory_file = static_cast<MemoryFile *>(pmemory_file);
    MemoryFileList.AddTail(memory_file);
    bEndianSwap32(&memory_file->Magic);
    bEndianSwap32(&memory_file->NumFileEntries);
    for (int n = 0; n < memory_file->NumFileEntries; n++) {
        MemoryFileEntry *memory_file_entry = &memory_file->FileEntries[n];
        bEndianSwap32(&memory_file_entry->Hash);
        bEndianSwap32(&memory_file_entry->Offset);
        bEndianSwap32(&memory_file_entry->FileSize);
        bEndianSwap32(&memory_file_entry->MemorySize);
        memory_file_entry->Data = reinterpret_cast<unsigned char *>(memory_file) + memory_file_entry->Offset;
    }
}

void RemoveMemoryFile(void *pmemory_file) {
    MemoryFile *memory_file = static_cast<MemoryFile *>(pmemory_file);
    MemoryFileList.Remove(memory_file);
}

MemoryFileEntry *FindMemoryFileEntry(const char *filename) {
    unsigned int name_hash = bFileGetFilenameHash(filename);
    for (MemoryFile *memory_file = MemoryFileList.GetHead(); memory_file != MemoryFileList.EndOfList();
         memory_file = memory_file->GetNext()) {
        for (int n = 0; n < memory_file->NumFileEntries; n++) {
            MemoryFileEntry *memory_file_entry = &memory_file->FileEntries[n];
            if (memory_file_entry->Hash == name_hash) {
                return memory_file_entry;
            }
        }
    }
    return nullptr;
}

void AsyncCloseFileCallback(int fop, int status, void *userdata) {
    FILESYS_completeop(fop);
}

void AsyncCloseFile(int file_handle) {
    int fop = FILESYS_open(nullptr, 0, nullptr, nullptr);
    FILESYS_read(fop, nullptr, 0, AsyncCloseFileCallback, nullptr);
}

void bFileFlushCachedFiles() {
    CachedRealFileHandle::FlushUnusedHandles(false);
}

void bFileFlushCacheFile(const char *filename) {
    CachedRealFileHandle::FlushUnusedHandle(filename);
}

bFile::bFile(const char *filename, bFileOpenMode open_mode) {
    bFileList.AddTail(this);
    bFileNumInstances++;
    Filename = bAllocateSharedString(filename);
    WriteBuffer = nullptr;
    FileHandle = -1;
    WriteBufferSize = 0;
    CloseAfterCallbacks = 0;
    OpenMode = open_mode;
    Position = 0;
    FileSize = 0;
    pCachedRealFileHandle = nullptr;
    NumPendingCallbacks = 0;
    WriteBufferPos = 0;
    WriteBufferNumBytes = 0;
    if (open_mode == BOPEN_MODE_WRITE || open_mode == BOPEN_MODE_APPEND) {
        bFileFlushCacheFile(filename);
        WriteBufferSize = 0x2000;
        WriteBuffer = static_cast<unsigned char *>(bMalloc(0x2000, 0));
    }
    CachedRealFileHandle *c = CachedRealFileHandle::FindHandle(filename);
    if (c != nullptr && OpenMode == BOPEN_MODE_READONLY) {
        c->AddReference();
        FileSize = c->GetFileSize();
        pCachedRealFileHandle = c;
        FileHandle = c->GetFileHandle();
    } else {
        MemoryFileEntry *entry = FindMemoryFileEntry(Filename);
        if (entry != nullptr) {
            FileHandle = entry->FileSize;
        } else {
            OpenLowLevel();
            if (FileHandle > -1) {
                if (OpenMode == BOPEN_MODE_APPEND) {
                    Position = FileHandle;
                }
                MaybeAddCachedHandle();
            }
        }
    }
}

void bFileRunTimingTest() {
    char filename[64];
}
