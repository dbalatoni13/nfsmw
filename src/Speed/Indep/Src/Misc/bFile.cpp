#include "./bFile.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
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
int FILESYS_opensync(const char *filename, unsigned int flags, int timeout);
void FILESYS_closesync(int handle, int timeout);
int FILESYS_writesync(int handle, int offset, void *buf, int nbytes, int timeout);
int FILESYS_waitop(int fop);
int FILESYS_size(int handle, int timeout, void *param);
int FILESYS_existssync(const char *filename, int timeout);
int FILESYS_opensync2(const char *filename, unsigned int flags, int timeout);
void FILESYS_callbackop(int fop, void (*callback)(int, int, void *));

int GetInfoFastByName(const char *filename, unsigned int flags, void *a, void *b);

unsigned int bStringHash(const char *str);
void bEndianSwap32(void *value);
char bToUpper(char c);

int GetQueuedFileSize(const char *filename);

extern SlotPool *bFileSlotPool;
extern int _5bFile_TotalNumPendingCallbacks;
extern bTList<void> _5bFile_PendingCallbackList;
extern bTList<void> _5bFile_CompletedCallbackList;

namespace RealFile {
struct Mutex {
    static void Create(Mutex *m);
    static void Lock(Mutex *m);
    static void Unlock(Mutex *m);
};
} // namespace RealFile

extern RealFile::Mutex bFileMutex;

void DVDErrorTask(void *a, int b);

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

CachedRealFileHandle *CachedRealFileHandle::FindHandle(const char *filename) {
    CachedRealFileHandle *handle = HandleList.GetHead();
    while (handle != HandleList.EndOfList()) {
        if (bStrCmp(handle->Filename, filename) == 0) {
            HandleList.Remove(handle);
            HandleList.AddTail(handle);
            return handle;
        }
        handle = handle->GetNext();
    }
    return nullptr;
}

CachedRealFileHandle *CachedRealFileHandle::AddHandle(const char *filename, int file_handle, int file_size) {
    do {
        if (NumInstances < 4) break;
    } while (RemoveUnusedHandle());
    CachedRealFileHandle *handle = new CachedRealFileHandle(filename, file_handle, file_size);
    NumInstances++;
    handle->Filename = bAllocateSharedString(filename);
    HandleList.AddTail(handle);
    return handle;
}

bool CachedRealFileHandle::RemoveUnusedHandle() {
    CachedRealFileHandle *handle = HandleList.GetHead();
    while (handle != HandleList.EndOfList()) {
        if (handle->NumReferences == 0) {
            HandleList.Remove(handle);
            if (handle != nullptr) {
                NumInstances--;
                AsyncCloseFile(handle->FileHandle);
                bFreeSharedString(handle->Filename);
                delete handle;
            }
            return true;
        }
        handle = handle->GetNext();
    }
    return false;
}

void CachedRealFileHandle::FlushUnusedHandle(const char *filename) {
    CachedRealFileHandle *handle = FindHandle(filename);
    if (handle != nullptr) {
        HandleList.Remove(handle);
        NumInstances--;
        AsyncCloseFile(handle->FileHandle);
        bFreeSharedString(handle->Filename);
        delete handle;
    }
}

void CachedRealFileHandle::FlushUnusedHandles(bool print_warning) {
    do {
    } while (RemoveUnusedHandle());
}

// DisculatorDriver
SlotPool *OpenDisculatorFileSlotPool;
DisculatorDriver *DisculatorDriver::sDisculatorDriver;

void *OpenDisculatorFile::operator new(unsigned int size) {
    return OpenDisculatorFileSlotPool->Malloc();
}

void OpenDisculatorFile::operator delete(void *ptr) {
    OpenDisculatorFileSlotPool->Free(ptr);
}

bool bInitDisculatorDriver(const char *dir_filename, const char *data_filename) {
    DisculatorDriver *driver = DisculatorDriver::Create(dir_filename, data_filename);
    if (driver != nullptr) {
        RealFile::AddDevice(driver);
        RealFile::AddSearchLocation("DVDV:\\", true);
        bFileRunTimingTest();
    }
    return driver != nullptr;
}

DisculatorDriver *DisculatorDriver::Create(const char *dir_filename, const char *data_filename) {
    if (sDisculatorDriver == nullptr) {
        DisculatorDriver *d = new DisculatorDriver();
        sDisculatorDriver = d;
        if (!d->LoadGiantFiles(dir_filename, data_filename)) {
            if (sDisculatorDriver != nullptr) {
                delete sDisculatorDriver;
            }
            sDisculatorDriver = nullptr;
        } else {
            return sDisculatorDriver;
        }
    }
    return nullptr;
}

bool DisculatorDriver::Init() {
    OpenDisculatorFileSlotPool = bNewSlotPool(0x28, 0x44, "OpenDisculatorFile", 0);
    CurrentSector = 0;
    TotalDeltaSector = 0;
    return true;
}

void DisculatorDriver::Restore() {}

int DisculatorDriver::Open(const char *name, int oflags, int *pParentFileHandle) {
    if (bStrNCmp(name, "DVDV:\\", 6) == 0) {
        name = name + 6;
    }
    char c = *name;
    while (c == '\\' || c == '/') {
        name++;
        c = *name;
    }
    bFileDirectoryEntry *entry = FindDirectoryEntry(name);
    if (entry == nullptr || GiantDataFileHandle[entry->FileNumber] == -1) {
        return -1;
    }
    OpenDisculatorFile *file = new OpenDisculatorFile();
    file->filename = bAllocateSharedString(name);
    file->nameHash = entry->Hash;
    file->giantFileNum = entry->FileNumber;
    file->localSectorOffset = entry->LocalSectorOffset;
    file->totalSectorOffset = entry->TotalSectorOffset;
    int sz = entry->Size;
    file->seekPos = 0;
    file->size = static_cast<unsigned long long>(sz);
    *pParentFileHandle = GiantDataFileHandle[entry->FileNumber];
    return reinterpret_cast<int>(file);
}

void DisculatorDriver::Close(int h) {
    OpenDisculatorFile *file = reinterpret_cast<OpenDisculatorFile *>(h);
    if (file != nullptr) {
        bFreeSharedString(file->filename);
        delete file;
    }
}

unsigned int DisculatorDriver::Read(int h, void *buf, unsigned int bufsize, RealFile::DeviceDriver *ddParent, int ddFileHandle) {
    OpenDisculatorFile *file = reinterpret_cast<OpenDisculatorFile *>(h);
    unsigned long long remaining = file->size - file->seekPos;
    if (bufsize > remaining) {
        bufsize = static_cast<unsigned int>(file->size - file->seekPos);
    }
    unsigned int seekLo = static_cast<unsigned int>(file->seekPos);
    int sector = file->totalSectorOffset + static_cast<int>(file->seekPos >> 11);
    int delta = sector - CurrentSector;
    if (delta < 0) delta = -delta;
    CurrentSector = file->totalSectorOffset + static_cast<int>((file->seekPos + bufsize) >> 11);
    TotalDeltaSector = TotalDeltaSector + delta;
    unsigned int sectorBase = static_cast<unsigned int>(file->localSectorOffset) * 0x800;
    // seek parent
    unsigned long long seekOffset = static_cast<unsigned long long>(static_cast<int>(sectorBase >> 21 | sectorBase << 11)) + file->seekPos;
    ddParent->Seek(ddFileHandle, seekOffset, 0, nullptr, 0);
    unsigned int bytesRead = ddParent->Read(ddFileHandle, buf, bufsize, nullptr, 0);
    file->seekPos = file->seekPos + bytesRead;
    return bytesRead;
}

unsigned int DisculatorDriver::Write(int h, const void *buf, unsigned int bufsize, RealFile::DeviceDriver *ddParent, int ddFileHandle) {
    return 0;
}

unsigned long long DisculatorDriver::Seek(int h, unsigned long long offset, int whence, RealFile::DeviceDriver *ddParent, int ddFileHandle) {
    OpenDisculatorFile *file = reinterpret_cast<OpenDisculatorFile *>(h);
    if (offset <= file->size) {
        file->seekPos = offset;
    }
    return file->seekPos;
}

unsigned long long DisculatorDriver::Getsize(int h) {
    OpenDisculatorFile *file = reinterpret_cast<OpenDisculatorFile *>(h);
    return file->size;
}

unsigned long long DisculatorDriver::QueryLocation(int h) {
    OpenDisculatorFile *file = reinterpret_cast<OpenDisculatorFile *>(h);
    return file->seekPos;
}

bool DisculatorDriver::LoadGiantFiles(const char *giant_dir_filename, const char *giant_data_filename_base) {
    if (!bFileExists(giant_dir_filename)) {
        return false;
    }
    bFile *f = bOpen(giant_dir_filename, 1, 1);
    unsigned int filesize = bFileSize(f);
    unsigned int numEntries = filesize / 0x18;
    bFileDirectoryEntry *table = new bFileDirectoryEntry[numEntries];
    bReadAsync(f, table, filesize, nullptr, nullptr);
    while (!bIsAsyncDone(f)) {
        DVDErrorTask(nullptr, 0);
        bThreadYield(8);
    }
    pDirectoryEntryTable = table;
    NumDirectoryEntries = numEntries;
    int i = 0;
    if (numEntries != 0) {
        do {
            int offset = i * 0x18;
            i++;
            bFileDirectoryEntry *e = reinterpret_cast<bFileDirectoryEntry *>(reinterpret_cast<char *>(pDirectoryEntryTable) + offset);
            bEndianSwap32(&e->Hash);
            bEndianSwap32(&e->FileNumber);
            bEndianSwap32(&e->LocalSectorOffset);
            bEndianSwap32(&e->TotalSectorOffset);
            bEndianSwap32(&e->Size);
            bEndianSwap32(&e->Checksum);
        } while (i < NumDirectoryEntries);
    }
    i = 0;
    do {
        bSPrintf(GiantDataFileName[i], "%s%d.bun", giant_data_filename_base, i);
        GiantDataFileHandle[i] = -1;
        i++;
    } while (i < 30);
    for (i = 0; i < 30; i++) {
        char *fn = GiantDataFileName[i];
        if (!FILESYS_existssync(fn, 100)) break;
        GiantDataFileHandle[i] = FILESYS_opensync(fn, 1, 100);
    }
    bClose(f);
    return true;
}

bFileDirectoryEntry *DisculatorDriver::FindDirectoryEntry(const char *filename) {
    if (NumDirectoryEntries == 0) {
        return nullptr;
    }
    unsigned int hash = bFileGetFilenameHash(filename);
    int high = NumDirectoryEntries;
    int result = -1;
    int low = 0;
    if (NumDirectoryEntries != 0 && high > -1) {
        do {
            result = (low + high) >> 1;
            unsigned int entryHash = pDirectoryEntryTable[result].Hash;
            if (entryHash == hash &&
                (result == 0 || pDirectoryEntryTable[result - 1].Hash != hash)) {
                break;
            }
            if (entryHash < hash) {
                low = result + 1;
            } else {
                high = result - 1;
            }
            result = -1;
        } while (low <= high);
    }
    if (result < 0) {
        return nullptr;
    }
    int bestIdx = result;
    int bestDist = 0x7fffffff;
    while (pDirectoryEntryTable[result].Hash == hash) {
        int dist = pDirectoryEntryTable[result].TotalSectorOffset - CurrentSector;
        if (dist < 0) dist = -dist;
        if (dist < bestDist) {
            bestIdx = result;
            bestDist = dist;
        }
        result++;
    }
    return &pDirectoryEntryTable[bestIdx];
}

void bFileFlushCachedFiles() {
    CachedRealFileHandle::FlushUnusedHandles(false);
}

void bFileFlushCacheFile(const char *filename) {
    CachedRealFileHandle::FlushUnusedHandle(filename);
}

DisculatorDriver::~DisculatorDriver() {}

bool DisculatorDriver::Remove(const char *name) {
    return false;
}

unsigned long long DisculatorDriver::Getspace() {
    return 0;
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

bFile::~bFile() {
    if (WriteBuffer != nullptr) {
        FlushWriteBuffer();
        bFree(WriteBuffer);
        WriteBuffer = nullptr;
    }
    bTNode<bFile>::Remove();
    bFileNumInstances--;
    if (FileHandle > -1) {
        if (pCachedRealFileHandle == nullptr) {
            if (FileHandle != 0) {
                AsyncCloseFile(FileHandle);
            }
        } else {
            pCachedRealFileHandle->RemoveReference();
        }
        FileHandle = -1;
    }
    bFreeSharedString(Filename);
}

void bFile::OpenLowLevel() {
    int flags = GetRealFileOpenFlags(OpenMode);
    if (flags == 2) {
        int handle = FILESYS_opensync(Filename, 1, 100);
        if (handle != 0) {
            flags = 0;
            FILESYS_closesync(handle, 100);
        }
    }
    if (flags == 1) {
        unsigned long long sizeOut;
        unsigned int dummy;
        int result = GetInfoFastByName(Filename, 1, &sizeOut, &dummy);
        if (result != 0) {
            FileSize = static_cast<int>(sizeOut);
        }
    } else {
        bGetTicker();
        int fop = FILESYS_opensync(Filename, flags, 100);
        int status = FILESYS_waitop(fop);
        int handle = FILESYS_completeop(fop);
        if (status == 1) {
            FileHandle = handle;
        }
    }
    if (FileHandle != 0) {
        bGetTicker();
        int fop = FILESYS_size(FileHandle, 100, nullptr);
        int status = FILESYS_waitop(fop);
        int sz = FILESYS_completeop(fop);
        if (status == 1) {
            FileSize = sz;
        }
    }
}

void bFile::MaybeAddCachedHandle() {
    if (FileHandle != 0 && pCachedRealFileHandle == nullptr && OpenMode == BOPEN_MODE_READONLY) {
        CachedRealFileHandle *handle = CachedRealFileHandle::AddHandle(Filename, FileHandle, FileSize);
        handle->AddReference();
        pCachedRealFileHandle = handle;
    }
}

void bFile::Seek(int position, int mode) {
    switch (mode) {
    case 0:
        Position = position;
        break;
    case 1:
        Position = FileSize - position;
        break;
    case 2:
        Position = Position + position;
        break;
    }
}

void bFile::ReadAsync(void *buf, int num_bytes, void (*callback)(void *), void *callback_param) {
    bGetTicker();
    if (FileSize < Position + num_bytes) {
        num_bytes = FileSize - Position;
    }
    MemoryFileEntry *entry = FindMemoryFileEntry(Filename);
    if (entry != nullptr) {
        if (Position < entry->FileSize) {
            int overflow = (Position + num_bytes) - entry->FileSize;
            if (overflow > 0) {
                bMemSet(static_cast<char *>(buf) + num_bytes - overflow, 0x21, overflow);
                num_bytes = num_bytes - overflow;
            }
            bMemCpy(buf, entry->Data + Position, num_bytes);
            Position = Position + num_bytes;
            if (callback == nullptr) {
                return;
            }
            callback(callback_param);
            return;
        }
    }
    RealFile::Mutex::Lock(&bFileMutex);
    int *cb = static_cast<int *>(bFileSlotPool->Malloc());
    cb[4] = Position;
    cb[6] = reinterpret_cast<int>(callback);
    cb[8] = 0;
    cb[7] = reinterpret_cast<int>(callback_param);
    cb[2] = reinterpret_cast<int>(this);
    cb[3] = reinterpret_cast<int>(buf);
    cb[5] = num_bytes;
    _5bFile_TotalNumPendingCallbacks++;
    _5bFile_PendingCallbackList.AddTail(reinterpret_cast<bTNode<void> *>(cb));
    NumPendingCallbacks++;
    RealFile::Mutex::Unlock(&bFileMutex);
    if (FileHandle == 0) {
        int flags = GetRealFileOpenFlags(OpenMode);
        int fop = FILESYS_open(Filename, flags, nullptr, cb);
        FILESYS_callbackop(fop, bFile::CallbackFunctionOpen);
        MaybeAddCachedHandle();
    } else {
        int fop = FILESYS_read(FileHandle, buf, num_bytes, nullptr, cb);
        FILESYS_callbackop(fop, bFile::CallbackFunctionRead);
    }
}

void bFile::FlushWriteBuffer() {
    if (WriteBufferNumBytes != 0) {
        FILESYS_writesync(FileHandle, WriteBufferPos, WriteBuffer, WriteBufferNumBytes, 100);
        WriteBufferNumBytes = 0;
        WriteBufferPos = Position;
    }
}

void bFile::Write(const void *buf, int num_bytes) {
    if (WriteBufferPos + WriteBufferNumBytes != Position) {
        FlushWriteBuffer();
    }
    if (WriteBufferNumBytes + num_bytes > WriteBufferSize) {
        FlushWriteBuffer();
    }
    if (num_bytes > WriteBufferSize) {
        FILESYS_writesync(FileHandle, Position, const_cast<void *>(buf), num_bytes, 100);
    } else {
        if (WriteBufferNumBytes == 0) {
            WriteBufferPos = Position;
        }
        bMemCpy(WriteBuffer + WriteBufferNumBytes, buf, num_bytes);
        WriteBufferNumBytes = WriteBufferNumBytes + num_bytes;
    }
    Position = Position + num_bytes;
    if (Position > FileSize) {
        FileSize = Position;
    }
}

void bFile::CallbackFunctionOpen(int fop, int status, void *userdata) {
    int *cb = static_cast<int *>(userdata);
    bGetTicker();
    int handle = FILESYS_completeop(fop);
    bFile *file = reinterpret_cast<bFile *>(cb[2]);
    if (file->FileHandle == 0) {
        file->FileHandle = handle;
    } else {
        cb[8] = handle;
    }
    if (file->FileHandle == 0) {
        RealFile::Mutex::Lock(&bFileMutex);
        _5bFile_PendingCallbackList.Remove(reinterpret_cast<bTNode<void> *>(cb));
        _5bFile_CompletedCallbackList.AddTail(reinterpret_cast<bTNode<void> *>(cb));
        RealFile::Mutex::Unlock(&bFileMutex);
    } else {
        int readFop = FILESYS_read(handle, reinterpret_cast<void *>(cb[3]), cb[5], nullptr, cb);
        FILESYS_callbackop(readFop, bFile::CallbackFunctionRead);
    }
}

void bFile::CallbackFunctionRead(int fop, int status, void *userdata) {
    int *cb = static_cast<int *>(userdata);
    bGetTicker();
    RealFile::Mutex::Lock(&bFileMutex);
    FILESYS_completeop(fop);
    _5bFile_PendingCallbackList.Remove(reinterpret_cast<bTNode<void> *>(cb));
    _5bFile_CompletedCallbackList.AddTail(reinterpret_cast<bTNode<void> *>(cb));
    if (cb[8] != 0) {
        AsyncCloseFile(cb[8]);
    }
    RealFile::Mutex::Unlock(&bFileMutex);
}

void bFile::HandleCompletedCallbacks() {
    RealFile::Mutex::Lock(&bFileMutex);
    while (reinterpret_cast<bTNode<void> *>(_5bFile_CompletedCallbackList.GetHead()) !=
           _5bFile_CompletedCallbackList.EndOfList()) {
        int *cb = reinterpret_cast<int *>(_5bFile_CompletedCallbackList.GetHead());
        bool shouldDelete = false;
        _5bFile_CompletedCallbackList.Remove(reinterpret_cast<bTNode<void> *>(cb));
        _5bFile_TotalNumPendingCallbacks--;
        bFile *file = reinterpret_cast<bFile *>(cb[2]);
        file->NumPendingCallbacks--;
        file->MaybeAddCachedHandle();
        if (file->CloseAfterCallbacks != 0) {
            shouldDelete = file->NumPendingCallbacks == 0;
        }
        if (cb[6] != 0) {
            RealFile::Mutex::Unlock(&bFileMutex);
            (reinterpret_cast<void (*)(void *)>(cb[6]))(reinterpret_cast<void *>(cb[7]));
            RealFile::Mutex::Lock(&bFileMutex);
        }
        if (cb != nullptr) {
            bFileSlotPool->Free(cb);
        }
        if (shouldDelete && file != nullptr) {
            file->~bFile();
            bFileSlotPool->Free(file);
        }
    }
    RealFile::Mutex::Unlock(&bFileMutex);
}

void bInitFileSystem() {
    if (bFileSlotPool == nullptr) {
        RealFile::Mutex::Create(&bFileMutex);
        bFileSlotPool = bNewSlotPool(0x40, 0x44, "bFile", 0);
    }
}

void bServiceFileSystem() {
    if (_5bFile_TotalNumPendingCallbacks != 0) {
        bSyncTaskRun();
        bFile::HandleCompletedCallbacks();
    }
}

void bWaitUntilAsyncDone(bFile *f) {
    while (!bIsAsyncDone(f)) {
        bThreadYield(8);
    }
}

bFile *bOpen(const char *filename, int open_mode, int warn_if_cant_open) {
    if (bFileSlotPool == nullptr) {
        bInitFileSystem();
    }
    bFile *f = new (bFileSlotPool->Malloc()) bFile(filename, static_cast<bFileOpenMode>(open_mode));
    if (f->FileSize < 0) {
        f->~bFile();
        bFileSlotPool->Free(f);
        f = nullptr;
    }
    return f;
}

void bClose(bFile *f) {
    if (f != nullptr) {
        if (f->NumPendingCallbacks < 1) {
            f->~bFile();
            bFileSlotPool->Free(f);
        } else {
            f->CloseAfterCallbacks = 1;
        }
    }
}

int bFileSize(bFile *f) {
    if (f == nullptr) return 0;
    if (f->FileSize < 0) return 0;
    return f->FileSize;
}

int bFileSize(const char *filename) {
    bFile *f = bOpen(filename, 1, 1);
    if (f == nullptr) return -1;
    int size = bFileSize(f);
    bClose(f);
    return size;
}

int bFileExists(const char *filename) {
    bGetTicker();
    bFile *f = bOpen(filename, 1, 0);
    int result = 0;
    if (f != nullptr) {
        result = f->FileSize + 1;
        bClose(f);
    }
    return result;
}

void bRead(bFile *f, void *buf, int numbytes) {
    if (f != nullptr && f->FileSize > -1) {
        f->ReadAsync(buf, numbytes, nullptr, nullptr);
        bWaitUntilAsyncDone(f);
    }
}

void bReadAsync(bFile *f, void *buf, int numbytes, void (*callback)(void *), void *param) {
    if (f != nullptr && f->FileSize >= 0) {
        f->ReadAsync(buf, numbytes, callback, param);
    }
}

void bSeek(bFile *f, int position, int mode) {
    if (f != nullptr) {
        f->Seek(position, mode);
    }
}

bool bIsAsyncDone(bFile *f) {
    bServiceFileSystem();
    int count = _5bFile_TotalNumPendingCallbacks;
    if (f != nullptr) {
        count = f->NumPendingCallbacks;
    }
    return count == 0;
}

void bWrite(bFile *f, const void *buf, int num_bytes) {
    f->Write(buf, num_bytes);
}

void *bGetFile(const char *filename, int *size_out, int flags) {
    bFile *f = bOpen(filename, 1, 1);
    if (f == nullptr) {
        return nullptr;
    }
    if (size_out != nullptr) {
        *size_out = bFileSize(f);
    }
    unsigned int pool = (flags >> 6) & 0x1ffc;
    if (pool == 0) {
        pool = 0x10;
    }
    if (pool == 0x10) {
        flags = flags | 0x2000;
    }
    void *buf = bMalloc(bFileSize(f), flags);
    bReadAsync(f, buf, bFileSize(f), nullptr, nullptr);
    while (!bIsAsyncDone(f)) {
        DVDErrorTask(nullptr, 0);
        bThreadYield(8);
    }
    bClose(f);
    return buf;
}

int bFPrintf(bFile *f, const char *fmt, ...) {
    int result;
    va_list ap;
    va_start(ap, fmt);
    if (f == nullptr) {
        result = bVPrintf(fmt, &ap);
    } else {
        char *buf = static_cast<char *>(bMalloc(0x2000, 0));
        result = bVSPrintf(buf, fmt, &ap);
        f->Write(buf, result);
        bFree(buf);
    }
    va_end(ap);
    return result;
}

void bAppendToFile(const char *filename, void *buf, int num_bytes) {
    bFile *f = bOpen(filename, 2, 1);
    if (f != nullptr) {
        f->Write(buf, num_bytes);
        bClose(f);
    }
}

void bFileRunTimingTest() {
    char filename[64];
}
