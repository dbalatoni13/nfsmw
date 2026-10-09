#ifndef CACHEDREALFILEHANDLE_HPP
#define CACHEDREALFILEHANDLE_HPP

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "realcore/file/driver.h"

typedef FILEHANDLE EAFileHandle;
extern SlotPool *bFileSlotPool;

// total size: 0x18
class CachedRealFileHandle : public bTNode<CachedRealFileHandle> {
  public:
    CachedRealFileHandle(const char *filename, EAFileHandle file_handle, int file_size) {
        this->NumInstances++;
        this->NumReferences = 0;
        this->FileHandle = file_handle;
        this->FileSize = file_size;
        this->Filename = bAllocateSharedString(filename);
    }

    ~CachedRealFileHandle() {}

    USE_SLOTALLOC(bFileSlotPool);

    EAFileHandle GetFileHandle() {
        return this->FileHandle;
    }

    int GetFileSize() {
        return this->FileSize;
    }

    void AddReference() {
        this->NumReferences++;
    }

    void RemoveReference() {
        this->NumReferences--;
    }

    static CachedRealFileHandle *FindHandle(const char *filename);
    static CachedRealFileHandle *AddHandle(const char *filename, EAFileHandle file_handle, int file_size);
    static bool RemoveUnusedHandle();
    static void FlushUnusedHandle(const char *filename);
    static void FlushUnusedHandles(bool print_warning);

    static int NumInstances;
    static bTList<CachedRealFileHandle> HandleList;

    int NumReferences;       // offset 0x8, size 0x4
    EAFileHandle FileHandle; // offset 0xC, size 0x4
    int FileSize;            // offset 0x10, size 0x4
    const char *Filename;    // offset 0x14, size 0x4
};

#endif
