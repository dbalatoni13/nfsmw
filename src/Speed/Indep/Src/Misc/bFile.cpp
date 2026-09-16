#include "./bFile.hpp"
#include "./Platform.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "realcore/system.h"
#include "realcore/file/filesys_cc.h"
#include "realcore/file/driver.h"

#include <stdarg.h>

typedef FILEHANDLE EAFileHandle; // Decl: 56

typedef void *EADeviceDriverFileHandle; // Decl: 58

SlotPool *bFileSlotPool = nullptr; // Decl: 67

static const int bFileVerbose = 0;     // Decl: 69
static const int bFilePrintReads = 0;  // Decl: 70
static const int bFilePrintTiming = 0; // Decl: 71 TODO

static const int EnableCachedRealFileHandle = 1; // Decl: 74
static const int EnableNonBlockingRealFile = 1;  // Decl: 75

static const int DisableMiniDisculator = 0; // Decl: 77

static const int bVerifyGiantFileChecksums = 0; // Decl: 79

static const int bFileDisableAsync = 0; // Decl: 81

static const int bFilePrintOpenFiles = 0; // Decl: 83

static const int MaxCachedRealFileHandles = 4; // Decl: 88

MUTEX bFileMutex; // Decl: 92

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
    int n;
    int len = bStrLen(filename);
    for (n = 0; n < len; n++) {
        char c = bToUpper(filename[n]);
        if (c == '/') {
            c = '\\';
        }
        mangled_name[n] = c;
    }
    mangled_name[n] = 0;
    return bStringHash(mangled_name);
}

// STRIPPED
float bFileGetDebugTime() {}

// STRIPPED
float bFileGetDeltaDebugTime() {}

static int DisplayFileStats; // Decl: 158
static int PrintFileStats;   // Decl: 159

// Decl: 180
class FileStats {
  public:
    void AddStatEntry(const char *filename, int seek_sector, int read_size, void *read_buf) {
        // TODO based on Undercover
    }

    void CaptureTimings() {}
};

FileStats gFileStats; // Decl: 260

void ServiceFileStats() {}

int GetRealFileOpenFlags(bFileOpenMode open_mode) {
    if (open_mode == BOPEN_MODE_READONLY) {
        return BOPEN_FLAG_READONLY;
    } else if (open_mode == BOPEN_MODE_WRITE) {
        return BOPEN_FLAG_WRITE;
    } else if (open_mode == BOPEN_MODE_APPEND) {
        return BOPEN_FLAG_APPEND;
    } else {
        return BOPEN_FLAG_READONLY;
    }
}

static const int bFileMicroPauseScreenX = -300; // Decl: 550
static const int bFileMicroPauseScreenY = -30;  // Decl: 551

bTList<MemoryFile> MemoryFileList; // Decl: 576

void AddMemoryFile(void *pmemory_file) {
    MemoryFile *memory_file = static_cast<MemoryFile *>(pmemory_file);
    MemoryFileList.AddTail(memory_file);

    bPlatEndianSwap(&memory_file->Magic);
    bPlatEndianSwap(&memory_file->NumFileEntries);

    for (int n = 0; n < memory_file->NumFileEntries; n++) {
        MemoryFileEntry *memory_file_entry = &memory_file->FileEntries[n];
        bPlatEndianSwap(&memory_file_entry->Hash);
        bPlatEndianSwap(&memory_file_entry->Offset);
        bPlatEndianSwap(&memory_file_entry->FileSize);
        bPlatEndianSwap(&memory_file_entry->MemorySize);
        memory_file_entry->Data = reinterpret_cast<uint8 *>(memory_file) + memory_file_entry->Offset;
    }
}

void RemoveMemoryFile(void *pmemory_file) {
    MemoryFile *memory_file = static_cast<MemoryFile *>(pmemory_file);
    MemoryFileList.Remove(memory_file);
}

MemoryFileEntry *FindMemoryFileEntry(const char *filename) {
    unsigned int name_hash = bFileGetFilenameHash(filename);
    for (MemoryFile *memory_file = MemoryFileList.GetHead(); memory_file != MemoryFileList.EndOfList(); memory_file = memory_file->GetNext()) {
        for (int n = 0; n < memory_file->NumFileEntries; n++) {
            MemoryFileEntry *memory_file_entry = &memory_file->FileEntries[n];
            if (memory_file_entry->Hash == name_hash) {
                return memory_file_entry;
            }
        }
    }
    return nullptr;
}

// total size: 0x24
// Decl: 633
class bFileCallbackEntry : public bTNode<bFileCallbackEntry> {
  public:
    bFileCallbackEntry(bFile *file, void *buf, int position, int num_bytes, void (*callback)(void *), void *callback_param)
        : File(file),                    //
          Callback(callback),            //
          FileHandleToClose(0),          //
          Position(position),            //
          CallbackParam(callback_param), //
          Buf(buf),                      //
          NumBytes(num_bytes) {}

    USE_SLOTALLOC(bFileSlotPool);

    bFile *File;                    // offset 0x8, size 0x4
    void *Buf;                      // offset 0xC, size 0x4
    int Position;                   // offset 0x10, size 0x4
    int NumBytes;                   // offset 0x14, size 0x4
    void (*Callback)(void *);       // offset 0x18, size 0x4
    void *CallbackParam;            // offset 0x1C, size 0x4
    EAFileHandle FileHandleToClose; // offset 0x20, size 0x4
};

void AsyncCloseFileCallback(int fop, int status, void *userdata) {
    int result = FILESYS_completeop(fop);
}

void AsyncCloseFile(EAFileHandle file_handle) {
    int fop = FILESYS_close(file_handle, 100, nullptr);
    FILESYS_callbackop(fop, AsyncCloseFileCallback);
}

// total size: 0x18
// Decl: 725
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

bTList<CachedRealFileHandle> CachedRealFileHandle::HandleList;
int CachedRealFileHandle::NumInstances = 0;

CachedRealFileHandle *CachedRealFileHandle::FindHandle(const char *filename) {
    for (CachedRealFileHandle *c = HandleList.GetHead(); c != HandleList.EndOfList(); c = c->GetNext()) {
        if (bStrCmp(c->Filename, filename) == 0) {
            HandleList.Remove(c);
            HandleList.AddTail(c);
            return c;
        }
    }
    return nullptr;
}

CachedRealFileHandle *CachedRealFileHandle::AddHandle(const char *filename, int file_handle, int file_size) {
    while (NumInstances >= 4 && RemoveUnusedHandle()) {
        // nop
    }
    CachedRealFileHandle *c = new CachedRealFileHandle(filename, file_handle, file_size);
    HandleList.AddTail(c);
    return c;
}

bool CachedRealFileHandle::RemoveUnusedHandle() {
    for (CachedRealFileHandle *c = HandleList.GetHead(); c != HandleList.EndOfList(); c = c->GetNext()) {
        if (c->NumReferences == 0) {
            HandleList.Remove(c);
            if (c != nullptr) {
                NumInstances--;
                AsyncCloseFile(c->FileHandle);
                bFreeSharedString(c->Filename);
                delete c;
            }
            return true;
        }
    }
    return false;
}

void CachedRealFileHandle::FlushUnusedHandle(const char *filename) {
    CachedRealFileHandle *c = FindHandle(filename);
    if (c != nullptr) {
        HandleList.Remove(c);
        NumInstances--;
        AsyncCloseFile(c->FileHandle);
        bFreeSharedString(c->Filename);
        delete c;
    }
}

void CachedRealFileHandle::FlushUnusedHandles(bool print_warning) {
    HandleList.IsEmpty(); // ?
    while (RemoveUnusedHandle()) {
    }
}

void bFileFlushCachedFiles() {
    CachedRealFileHandle::FlushUnusedHandles(false);
}

void bFileFlushCacheFile(const char *filename) {
    CachedRealFileHandle::FlushUnusedHandle(filename);
}

SlotPool *OpenDisculatorFileSlotPool = nullptr; // Decl: 964

// total size: 0x28
struct OpenDisculatorFile {
    OpenDisculatorFile(bFileDirectoryEntry &dirEntry, const char *_filename)
        : nameHash(dirEntry.Hash),                       //
          giantFileNum(dirEntry.FileNumber),             //
          localSectorOffset(dirEntry.LocalSectorOffset), //
          totalSectorOffset(dirEntry.TotalSectorOffset), //
          seekPos(0),                                    //
          size(dirEntry.Size),                           //
          filename(_filename) {}

    ~OpenDisculatorFile() {}

    USE_SLOTALLOC(OpenDisculatorFileSlotPool);

    const char *filename;    // offset 0x0, size 0x4
    int32 nameHash;          // offset 0x4, size 0x4
    int32 giantFileNum;      // offset 0x8, size 0x4
    int32 localSectorOffset; // offset 0xC, size 0x4
    int32 totalSectorOffset; // offset 0x10, size 0x4
    uint64 size;             // offset 0x18, size 0x8
    uint64 seekPos;          // offset 0x20, size 0x8
};

// total size: 0x81C
// Decl: 1011
class DisculatorDriver : public RealFile::DeviceDriver {
  public:
    bool Init() override;
    void Restore() override;
    void Close(EAFileHandle h) override;
    EAFileHandle Open(const char *name, int oflags, int *pParentFileHandle) override;
    uint32_t Read(EAFileHandle h, void *buf, unsigned int bufsize, RealFile::DeviceDriver *ddParent, EAFileHandle ddFileHandle) override;
    uint32_t Write(EAFileHandle h, const void *buf, unsigned int bufsize, RealFile::DeviceDriver *ddParent, EAFileHandle ddFileHandle) override;
    uint64_t Seek(EAFileHandle h, uint64_t offset, int whence, RealFile::DeviceDriver *ddParent, EAFileHandle ddFileHandle) override;
    uint64_t Getsize(EAFileHandle h) override;
    uint64_t QueryLocation(EAFileHandle h) override;
    bool Remove(const char *name) override {
        return false;
    }
    uint64_t Getspace() override {
        return 0;
    }

    int GetFileSize(const char *filename);

    static DisculatorDriver *Get() {
        return sDisculatorDriver;
    }

    static DisculatorDriver *Create(const char *dir_filename, const char *data_filename);

    const char *GetDirectoryFileOffset(int *poffset, const char *filename);

  private:
    DisculatorDriver() : DeviceDriver("discu:") {}

    bool LoadGiantFiles(const char *giant_dir_filename, const char *giant_data_filename_base);

    char *GetGiantDataFileName(int file_number) {
        return GiantDataFileName[file_number];
    }

    bFileDirectoryEntry *FindDirectoryEntry(const char *filename);

    static DisculatorDriver *sDisculatorDriver;

    bFileDirectoryEntry *pDirectoryEntryTable;              // offset 0x14, size 0x4
    int NumDirectoryEntries;                                // offset 0x18, size 0x4
    char GiantDataFileName[MAX_DISCULATOR_GIANT_FILES][64]; // offset 0x1C, size 0x780
    int GiantDataFileHandle[MAX_DISCULATOR_GIANT_FILES];    // offset 0x79C, size 0x78
    int CurrentSector;                                      // offset 0x814, size 0x4
    int TotalDeltaSector;                                   // offset 0x818, size 0x4, Decl: 1075
};

DisculatorDriver *DisculatorDriver::sDisculatorDriver = nullptr; // Decl: 1078

static const int PrintCDSeeking = ENABLE_IN_DEBUG; // Decl: 1081

bool bInitDisculatorDriver(const char *dir_filename, const char *data_filename) {
    DisculatorDriver *driver = DisculatorDriver::Create(dir_filename, data_filename);
    if (driver != nullptr) {
        RealFile::AddDevice(driver);
        RealFile::AddSearchLocation("discu:", true);
        bFileRunTimingTest();
        return true;
    }
    return false;
}

DisculatorDriver *DisculatorDriver::Create(const char *dir_filename, const char *data_filename) {
    if (sDisculatorDriver == nullptr) {
        sDisculatorDriver = new DisculatorDriver();
        if (sDisculatorDriver->LoadGiantFiles(dir_filename, data_filename)) {
            return sDisculatorDriver;
        } else {
            delete sDisculatorDriver;
            sDisculatorDriver = nullptr;
        }
    }
    return nullptr;
}

bool DisculatorDriver::Init() {
    OpenDisculatorFileSlotPool = bNewSlotPool(0x28, MaxCachedRealFileHandles + 64, "OpenDisculatorFileSlotPool", 0);
    this->CurrentSector = 0;
    this->TotalDeltaSector = 0;
    return true;
}

void DisculatorDriver::Restore() {}

EAFileHandle DisculatorDriver::Open(const char *name, int oflags, int *pParentFileHandle) {
    if (bStrNCmp(name, "discu:", 6) == 0) {
        name = name + 6;
    }
    while (*name == '\\' || *name == '/') {
        name++;
    }
    // TODO scope
    {
        bFileDirectoryEntry *dirEntry = this->FindDirectoryEntry(name);
        if (dirEntry != nullptr) {
            if (GiantDataFileHandle[dirEntry->FileNumber] != -1) {
                OpenDisculatorFile *odf = new OpenDisculatorFile(*dirEntry, bAllocateSharedString(name));
                *pParentFileHandle = GiantDataFileHandle[dirEntry->FileNumber];
                return reinterpret_cast<int>(odf);
            } else {
                return -1;
            }
        } else {
            return -1;
        }
    }
}

void DisculatorDriver::Close(intptr_t h) {
    OpenDisculatorFile *odf = reinterpret_cast<OpenDisculatorFile *>(h);
    if (odf != nullptr) {
        bFreeSharedString(odf->filename);
        delete odf;
    }
}

uint32_t DisculatorDriver::Read(EAFileHandle h, void *buf, unsigned int bufsize, RealFile::DeviceDriver *ddParent, EAFileHandle ddFileHandle) {
    OpenDisculatorFile *odf = reinterpret_cast<OpenDisculatorFile *>(h);
    if (bufsize > odf->size - odf->seekPos) {
        bufsize = static_cast<unsigned int>(odf->size - odf->seekPos);
    }

    int new_start_sector = odf->totalSectorOffset + static_cast<int>(odf->seekPos >> 11);
    int new_end_sector = odf->totalSectorOffset + static_cast<int>((odf->seekPos + bufsize) >> 11);
    int delta_sector = bAbs(new_start_sector - this->CurrentSector);

    this->CurrentSector = new_end_sector;
    this->TotalDeltaSector += delta_sector;

    if (PrintCDSeeking) {
        // TODO using undercover
        float time;
        static float last_time = 0.0f;
        float delta_time;
        static bool first_time = true;
    }
    // TODO using undercover
    gFileStats.AddStatEntry(odf->filename, new_start_sector, bufsize, nullptr);

    ddParent->Seek(ddFileHandle, odf->seekPos + static_cast<uint64_t>(odf->localSectorOffset << 11), 0, nullptr, 0);
    int nread = ddParent->Read(ddFileHandle, buf, bufsize, nullptr, 0);
    odf->seekPos += nread;
    return nread;
}

uint32_t DisculatorDriver::Write(EAFileHandle h, const void *buf, unsigned int bufsize, RealFile::DeviceDriver *ddParent, EAFileHandle ddFileHandle) {
    return 0;
}

uint64_t DisculatorDriver::Seek(EAFileHandle h, uint64_t offset, int whence, RealFile::DeviceDriver *ddParent, EAFileHandle ddFileHandle) {
    OpenDisculatorFile *odf = reinterpret_cast<OpenDisculatorFile *>(h);
    if (offset <= odf->size) {
        odf->seekPos = offset;
    }
    return odf->seekPos;
}

uint64_t DisculatorDriver::Getsize(EAFileHandle h) {
    OpenDisculatorFile *odf = reinterpret_cast<OpenDisculatorFile *>(h);
    return odf->size;
}

uint64_t DisculatorDriver::QueryLocation(EAFileHandle h) {
    OpenDisculatorFile *odf = reinterpret_cast<OpenDisculatorFile *>(h);
    return odf->seekPos;
}

bool DisculatorDriver::LoadGiantFiles(const char *giant_dir_filename, const char *giant_data_filename_base) {
    if (bFileExists(giant_dir_filename)) {
        bFile *f = bOpen(giant_dir_filename, BOPEN_MODE_READONLY, 1);
        int size = bFileSize(f);
        int num_entries = size / sizeof(bFileDirectoryEntry);

        bFileDirectoryEntry *dir = new (giant_dir_filename, 0) bFileDirectoryEntry[num_entries];
        bReadAsync(f, dir, size, nullptr, nullptr);

        while (!bIsAsyncDone(f)) {
            void DVDErrorTask(void *, int);
            DVDErrorTask(nullptr, 0);
            bThreadYield(8);
        }

        this->pDirectoryEntryTable = dir;
        this->NumDirectoryEntries = num_entries;

        for (int n = 0; n < this->NumDirectoryEntries; n++) {
            bFileDirectoryEntry *dir = &this->pDirectoryEntryTable[n];
            bPlatEndianSwap(&dir->Hash);
            bPlatEndianSwap(&dir->FileNumber);
            bPlatEndianSwap(&dir->LocalSectorOffset);
            bPlatEndianSwap(&dir->TotalSectorOffset);
            bPlatEndianSwap(&dir->Size);
            bPlatEndianSwap(&dir->Checksum);
        }

        int file_number;
        for (file_number = 0; file_number < MAX_DISCULATOR_GIANT_FILES; file_number++) {
            bSPrintf(this->GiantDataFileName[file_number], "%s%d.BIN", giant_data_filename_base, file_number);
            *(this->GiantDataFileHandle + file_number) = -1;
        }

        for (file_number = 0; file_number < MAX_DISCULATOR_GIANT_FILES; file_number++) {
            if (FILESYS_existssync(this->GiantDataFileName[file_number], 100)) {
                this->GiantDataFileHandle[file_number] = FILESYS_opensync(this->GiantDataFileName[file_number], 1, 100);
            } else {
                break;
            }
        }

        bClose(f);
        return true;
    } else {
        return false;
    }
}

bFileDirectoryEntry *DisculatorDriver::FindDirectoryEntry(const char *filename) {
    if (NumDirectoryEntries == 0) {
        return nullptr;
    }
    unsigned int search_hash = bFileGetFilenameHash(filename);
    int found_position = -1;
    if (NumDirectoryEntries != 0) {
        int last = NumDirectoryEntries;
        int first = 0;
        if (first <= last) {
            do {
                int mid = (first + last) >> 1;
                unsigned int hash = pDirectoryEntryTable[mid].Hash;
                if (hash == search_hash && (mid == 0 || pDirectoryEntryTable[mid - 1].Hash != search_hash)) {
                    found_position = mid;
                    break;
                }
                if (hash >= search_hash) {
                    last = mid - 1;
                } else {
                    first = mid + 1;
                }
            } while (first <= last);
        }
    }
    if (found_position < 0) {
        return nullptr;
    }
    int closest_distance = 0x7fffffff;
    int best_found_position = found_position;

    while (pDirectoryEntryTable[found_position].Hash == search_hash) {
        int distance = pDirectoryEntryTable[found_position].TotalSectorOffset - CurrentSector;
        if (distance < 0)
            distance = -distance;
        if (distance < closest_distance) {
            best_found_position = found_position;
            closest_distance = distance;
        }
        found_position++;
    }
    bFileDirectoryEntry *best_directory_entry = &pDirectoryEntryTable[best_found_position];
    return best_directory_entry;
}

// STRIPPED
const char *DisculatorDriver::GetDirectoryFileOffset(int *poffset, const char *filename) {}

// STRIPPED
int DisculatorDriver::GetFileSize(const char *filename) {}

// STRIPPED
const char *bFileGetDisculatorOffset(int *poffset, const char *filename) {}

// total size: 0x38
// Decl: 1568
class bFile : public bTNode<bFile> {
  public:
    bFile(const char *filename, bFileOpenMode open_mode);
    ~bFile();

    USE_SLOTALLOC(bFileSlotPool);

    void OpenLowLevel();
    void MaybeAddCachedHandle();

    bool IsOpen() {
        return this->FileSize >= 0;
    }
    const char *GetFilename() {
        return this->Filename;
    }
    int GetFileSize() {
        return this->FileSize;
    }
    int GetPosition() {
        return this->Position;
    }
    int GetNumPendingCallbacks() {
        return this->NumPendingCallbacks;
    }
    void SetCloseAfterCallbacks() {
        this->CloseAfterCallbacks = 1;
    }
    static int GetTotalNumPendingCallbacks() {
        return TotalNumPendingCallbacks;
    }

    void Seek(int position, int mode);
    void ReadAsync(void *buf, int num_bytes, void (*callback)(void *), void *callback_param);
    void Write(const void *buf, int num_bytes);
    void FlushWriteBuffer();

    static void HandleCompletedCallbacks();
    static void CallbackFunctionOpen(int fop, int status, void *userdata);
    static void CallbackFunctionRead(int fop, int status, void *userdata);

  private:
    bFileOpenMode OpenMode;                      // offset 0x8, size 0x4
    int FileSize;                                // offset 0xC, size 0x4
    int Position;                                // offset 0x10, size 0x4
    EAFileHandle FileHandle;                     // offset 0x14, size 0x4
    CachedRealFileHandle *pCachedRealFileHandle; // offset 0x18, size 0x4
    int CloseAfterCallbacks;                     // offset 0x1C, size 0x4

    int NumPendingCallbacks; // offset 0x20, size 0x4
    const char *Filename;    // offset 0x24, size 0x4 // Decl: 1610

    static int TotalNumPendingCallbacks;                     // Decl: 1612
    static bTList<bFileCallbackEntry> PendingCallbackList;   // Decl: 1613
    static bTList<bFileCallbackEntry> CompletedCallbackList; // Decl: 1614

    int WriteBufferPos;      // offset 0x28, size 0x4, Decl: 1617
    int WriteBufferNumBytes; // offset 0x2C, size 0x4
    int WriteBufferSize;     // offset 0x30, size 0x4
    uint8 *WriteBuffer;      // offset 0x34, size 0x4
};

bTList<bFile> bFileList;   // Decl: 1623
int bFileNumInstances = 0; // Decl: 1624

int bFile::TotalNumPendingCallbacks = 0;
bTList<bFileCallbackEntry> bFile::PendingCallbackList;
bTList<bFileCallbackEntry> bFile::CompletedCallbackList;

bFile::bFile(const char *filename, bFileOpenMode open_mode) {
    bFileList.AddTail(this);
    bFileNumInstances++;
    this->Filename = bAllocateSharedString(filename);
    this->FileSize = -1;
    this->pCachedRealFileHandle = nullptr;
    this->OpenMode = open_mode;
    this->Position = 0;
    this->FileHandle = 0;
    this->CloseAfterCallbacks = 0;
    this->NumPendingCallbacks = 0;
    this->WriteBufferPos = 0;
    this->WriteBufferNumBytes = 0;
    this->WriteBufferSize = 0;
    this->WriteBuffer = nullptr;

    if (open_mode == BOPEN_MODE_WRITE || open_mode == BOPEN_MODE_APPEND) {
        bFileFlushCacheFile(Filename);
        this->WriteBufferSize = 0x2000;
        this->WriteBuffer = static_cast<uint8 *>(bMalloc(0x2000, "bFile WriteBuffer", 0, 0));
    }
    CachedRealFileHandle *c = CachedRealFileHandle::FindHandle(filename);
    if ((c != nullptr) && (OpenMode == BOPEN_MODE_READONLY)) {
        c->AddReference();
        this->FileHandle = c->GetFileHandle();
        this->FileSize = c->GetFileSize();
        this->pCachedRealFileHandle = c;
    } else {
        if (FindMemoryFileEntry(Filename) != nullptr) {
            this->FileSize = FindMemoryFileEntry(Filename)->FileSize;
        } else {
            this->OpenLowLevel();
            if (this->FileSize >= 0) {
                if (this->OpenMode == BOPEN_MODE_APPEND) {
                    this->Position = this->FileSize;
                }
                this->MaybeAddCachedHandle();
            }
        }
    }
}

bFile::~bFile() {
    if (this->WriteBuffer != nullptr) {
        this->FlushWriteBuffer();
        bFree(this->WriteBuffer);
        this->WriteBuffer = nullptr;
    }
    bFileList.Remove(this);
    bFileNumInstances--;
    if (this->IsOpen()) {
        if (bFileVerbose) {
            bPrintf("bFile: %-35s  Close()\n", this->Filename);
        }
        if (this->pCachedRealFileHandle != nullptr) {
            this->pCachedRealFileHandle->RemoveReference();
        } else if (FileHandle != 0) {
            AsyncCloseFile(FileHandle);
        }
        this->FileSize = -1;
    }
    bFreeSharedString(this->Filename);
}

// TODO maybe higher
inline void DetectMicropause(int start_tick, const char *text, const char *filename) {}

inline void CheckForFatalDiscError() {}

void bFile::OpenLowLevel() {
    int open_flags = GetRealFileOpenFlags(this->OpenMode);
    if (open_flags == BOPEN_FLAG_APPEND) {
        int file_handle = FILESYS_opensync(this->Filename, 1, 100);
        if (file_handle != 0) {
            open_flags = 0;
            FILESYS_closesync(file_handle, 100);
        }
    }
    if (open_flags == BOPEN_FLAG_READONLY) {
        uint64_t location_64;
        uint64_t filesize_64;
        if (RealFile::GetInfoFastByName(this->Filename, 1, location_64, filesize_64)) {
            this->FileSize = static_cast<int>(filesize_64);
        }
    } else {
        int ticks = bGetTicker();
        FILEOP fop = FILESYS_open(this->Filename, open_flags, 100, nullptr);
        int status = FILESYS_waitop(fop);
        int result = FILESYS_completeop(fop);
        if (status == 1) {
            this->FileHandle = result;
        }
        // TODO DetectMicropause from another game
    }
    if (this->FileHandle != 0) {
        int ticks = bGetTicker();
        FILEOP fop = FILESYS_size(this->FileHandle, 100, nullptr);
        int status = FILESYS_waitop(fop);
        int file_size = FILESYS_completeop(fop);
        if (status == 1) {
            this->FileSize = file_size;
        }
        // TODO DetectMicropause from another game
    }
}

void bFile::MaybeAddCachedHandle() {
    if ((this->FileHandle != 0) && (this->pCachedRealFileHandle == nullptr) && (this->OpenMode == BOPEN_MODE_READONLY) &&
        EnableCachedRealFileHandle) {
        CachedRealFileHandle *c = CachedRealFileHandle::AddHandle(this->Filename, this->FileHandle, this->FileSize);
        c->AddReference();
        pCachedRealFileHandle = c;
    }
}

void bFile::Seek(int position, int mode) {
    switch (mode) {
        case 0:
            this->Position = position;
            break;
        case 1:
            this->Position = this->FileSize - position;
            break;
        case 2:
            this->Position += position;
            break;
    }
}

void bFile::ReadAsync(void *buf, int num_bytes, void (*callback)(void *), void *callback_param) {
    int ticks = bGetTicker();
    if (this->Position + num_bytes > this->FileSize) {
        num_bytes = this->FileSize - this->Position;
    }
    MemoryFileEntry *memory_file_entry = FindMemoryFileEntry(Filename);
    if (memory_file_entry != nullptr) {
        if (this->Position < memory_file_entry->MemorySize) {
            if (bFilePrintReads) {
                bPrintf("bMemoryFileRead: %-40s  Pos =%7d   Size =%7d\n", this->Filename, this->Position, num_bytes);
            }
            int extra_bytes = (this->Position + num_bytes) - memory_file_entry->MemorySize;
            if (extra_bytes > 0) {
                bMemSet(static_cast<char *>(buf) + num_bytes - extra_bytes, 0x21, extra_bytes);
                num_bytes = num_bytes - extra_bytes;
            }
            bMemCpy(buf, memory_file_entry->Data + this->Position, num_bytes);
            this->Position += num_bytes;
            if (callback == nullptr) {
                return;
            }
            callback(callback_param);
            return;
        }
    }
    MUTEX_lock(&bFileMutex);
    bFileCallbackEntry *callback_entry = new bFileCallbackEntry(this, buf, this->Position, num_bytes, callback, callback_param);
    PendingCallbackList.AddTail(callback_entry);
    bFile::TotalNumPendingCallbacks++;
    this->NumPendingCallbacks++;
    MUTEX_unlock(&bFileMutex);

    bool file_needs_opening = this->FileHandle == 0;
    if (file_needs_opening) {
        FILEOP fop = FILESYS_open(this->Filename, GetRealFileOpenFlags(this->OpenMode), 100, reinterpret_cast<void *>(callback_entry));
        FILESYS_callbackop(fop, bFile::CallbackFunctionOpen);
        this->MaybeAddCachedHandle();
    } else {
        FILEOP fop = FILESYS_read(this->FileHandle, this->Position, buf, num_bytes, 0x64, reinterpret_cast<void *>(callback_entry));
        FILESYS_callbackop(fop, bFile::CallbackFunctionRead);
    }
    this->Position += num_bytes;
    DetectMicropause(ticks, "%s - %s", this->Filename);
}

void bFile::FlushWriteBuffer() {
    if (this->WriteBufferNumBytes != 0) {
        FILESYS_writesync(this->FileHandle, this->WriteBufferPos, this->WriteBuffer, this->WriteBufferNumBytes, 100);
        this->WriteBufferNumBytes = 0;
        this->WriteBufferPos = Position;
    }
}

void bFile::Write(const void *buf, int num_bytes) {
    if (bFileVerbose) {
        bPrintf("bFile: %-35s  Write()  Pos = %6d  Size = %6d\n", this->Filename, this->Position, num_bytes);
    }
    if (this->WriteBufferPos + this->WriteBufferNumBytes != this->Position) {
        this->FlushWriteBuffer();
    }
    if (this->WriteBufferNumBytes + num_bytes > this->WriteBufferSize) {
        this->FlushWriteBuffer();
    }
    if (num_bytes > this->WriteBufferSize) {
        FILESYS_writesync(this->FileHandle, this->Position, const_cast<void *>(buf), num_bytes, 100);
    } else {
        if (this->WriteBufferNumBytes == 0) {
            this->WriteBufferPos = this->Position;
        }
        bMemCpy(this->WriteBuffer + this->WriteBufferNumBytes, buf, num_bytes);
        this->WriteBufferNumBytes += num_bytes;
    }
    this->Position += num_bytes;
    if (this->Position > FileSize) {
        this->FileSize = this->Position;
    }
}

void bFile::CallbackFunctionOpen(int fop, int status, void *userdata) {
    int ticks = bGetTicker();
    bFileCallbackEntry *callback_entry = static_cast<bFileCallbackEntry *>(userdata);
    int file_handle = FILESYS_completeop(fop);
    bFile *file = callback_entry->File;
    if (file->FileHandle == 0) {
        file->FileHandle = file_handle;
    } else {
        callback_entry->FileHandleToClose = file_handle;
    }
    if (file->FileHandle != 0) {
        int readFop = FILESYS_read(file_handle, callback_entry->Position, callback_entry->Buf, callback_entry->NumBytes, 100, callback_entry);
        FILESYS_callbackop(readFop, bFile::CallbackFunctionRead);
    } else {
        MUTEX_lock(&bFileMutex);
        PendingCallbackList.Remove(callback_entry);
        CompletedCallbackList.AddTail(callback_entry);
        MUTEX_unlock(&bFileMutex);
        // TODO DetectMicropause
    }
}

void bFile::CallbackFunctionRead(int fop, int status, void *userdata) {
    gFileStats.CaptureTimings();
    int ticks = bGetTicker();
    bFileCallbackEntry *callback_entry = static_cast<bFileCallbackEntry *>(userdata);
    MUTEX_lock(&bFileMutex);
    int result = FILESYS_completeop(fop);
    PendingCallbackList.Remove(callback_entry);
    CompletedCallbackList.AddTail(callback_entry);
    if (callback_entry->FileHandleToClose != 0) {
        AsyncCloseFile(callback_entry->FileHandleToClose);
    }
    MUTEX_unlock(&bFileMutex);
    // TODO DetectMicropause
}

void bFile::HandleCompletedCallbacks() {
    MUTEX_lock(&bFileMutex);
    while (!CompletedCallbackList.IsEmpty()) {
        bFileCallbackEntry *callback_entry = CompletedCallbackList.RemoveHead();
        bFile *file = callback_entry->File;
        file->NumPendingCallbacks--;
        TotalNumPendingCallbacks--;
        file->MaybeAddCachedHandle();

        bool delete_file_after_callback = false;
        if (file->CloseAfterCallbacks) {
            delete_file_after_callback = file->NumPendingCallbacks == 0;
        }
        if (callback_entry->Callback != nullptr) {
            MUTEX_unlock(&bFileMutex);
            CheckForFatalDiscError();
            callback_entry->Callback(callback_entry->CallbackParam);
            MUTEX_lock(&bFileMutex);
        }
        delete callback_entry;
        if (delete_file_after_callback) {
            if (bFileVerbose) {
                bPrintf("bFile: %-35s  Close()   [All callbacks are now complete]\n", file->Filename);
            }
            delete file;
        }
    }
    MUTEX_unlock(&bFileMutex);
}

void bInitFileSystem() {
    if (bFileSlotPool == nullptr) {
        MUTEX_create(&bFileMutex);
        unsigned int slot_size = 64;
        bFileSlotPool = bNewSlotPool(slot_size, 68, "bFile System", 0);
    }
}

// STRIPPED
void bCloseFileSystem() {}

static const int DetectBusyLoopInServiceFileSystem = 0; // Decl: 2446

static const int bFileSlowReadCount = 0; // Decl: 2452

void bServiceFileSystem() {
    if (bFileSlowReadCount > 0) {
        static int slowdown_counter = 0;
        if (++slowdown_counter < bFileSlowReadCount) {
            slowdown_counter = 0;
        }
    }
    gFileStats.CaptureTimings();

    if (bFilePrintOpenFiles) {
        // TODO undercover
    }

    if (DetectBusyLoopInServiceFileSystem) {
        static int previous_ticks = 0;
        static int num_busy_loops = 0;
        if (bGetTickerDifference(previous_ticks) > 0.1f) {
            num_busy_loops = 0;
        } else if (num_busy_loops++ > 10) {
        }
    }
    if (bFile::GetTotalNumPendingCallbacks() != 0) {
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
    bFile *f = new bFile(filename, static_cast<bFileOpenMode>(open_mode));
    if (f->IsOpen()) {
        CheckForFatalDiscError();
    } else {
        delete f;
        f = nullptr;
    }
    return f;
}

void bClose(bFile *f) {
    if (f != nullptr) {
        if (f->GetNumPendingCallbacks() > 0) {
            f->SetCloseAfterCallbacks();
        } else {
            delete f;
        }
    }
    CheckForFatalDiscError();
}

int bFileSize(bFile *f) {
    if ((f != nullptr) && f->IsOpen()) {
        return f->GetFileSize();
    }
    return 0;
}

int bFileSize(const char *filename) {
    bFile *f = bOpen(filename, BOPEN_MODE_READONLY, 1);
    if (f != nullptr) {
        int file_size = bFileSize(f);
        bClose(f);
        return file_size;
    }
    return -1;
}

int bFileExists(const char *filename) {
    int ticks = bGetTicker();
    int warn_if_cant_open = 0;
    bFile *f = bOpen(filename, BOPEN_MODE_READONLY, warn_if_cant_open);
    int result = 0;
    if (f != nullptr) {
        int size = f->GetFileSize();
        bClose(f);
        DetectMicropause(ticks, "%s - %s", "bFileExists()");
        result = size + 1;
    }
    DetectMicropause(ticks, "%s - %s", "bFileExists()");
    return result;
}

// STRIPPED
int bFilePosition(bFile *f) {}

// STRIPPED
void bGetFilename(bFile *f, char *filename, int max_size) {}

void bReadAsync(bFile *f, void *buf, int numbytes, void (*callback)(void *), void *param) {
    if ((f != nullptr) && f->IsOpen()) {
        f->ReadAsync(buf, numbytes, callback, param);
        if (bFileDisableAsync) {
            bWaitUntilAsyncDone(f);
        }
    }
    CheckForFatalDiscError();
}

void bRead(bFile *f, void *buf, int numbytes) {
    bReadAsync(f, buf, numbytes, nullptr, nullptr);
    bWaitUntilAsyncDone(f);
}

void bSeek(bFile *f, int position, int mode) {
    if (f != nullptr) {
        f->Seek(position, mode);
    }
}

bool bIsAsyncDone(bFile *f) {
    bServiceFileSystem();
    if (f != nullptr) {
        return f->GetNumPendingCallbacks() == 0;
    } else {
        return bFile::GetTotalNumPendingCallbacks() == 0;
    }
}

void bWrite(bFile *f, const void *buf, int num_bytes) {
    f->Write(buf, num_bytes);
}

void *bGetFile(const char *filename, int32 *size_out, int flags) {
    bFile *f = bOpen(filename, BOPEN_MODE_READONLY, 1);
    if (f == nullptr) {
        return nullptr;
    }
    if (size_out != nullptr) {
        *size_out = bFileSize(f);
    }
    // TODO remove
    unsigned int hack = bMemoryGetAlignment(flags);
    if (hack == 0) {
        hack = 16;
    }
    if (hack == 16) {
        flags |= 0x2000;
    }
    void *buf = bMalloc(bFileSize(f), filename, 0, flags);
    unsigned int filesize = bFileSize(f);
    bReadAsync(f, buf, filesize, nullptr, nullptr);
    while (!bIsAsyncDone(f)) {
        DVDErrorTask(nullptr, 0);
        bThreadYield(8);
    }
    bClose(f);
    return buf;
}

int bFPrintf(bFile *f, const char *fmt, ...) {
    if (f == nullptr) {
        va_list arg_list;
        va_start(arg_list, fmt);
        int len = bVPrintf(fmt, arg_list);
        va_end(arg_list);
        return len;
    }
    char *buffer = new ("bFPrintf", 0) char[0x2000];
    va_list arg_list;
    va_start(arg_list, fmt);
    int len = bVSPrintf(buffer, fmt, arg_list);
    va_end(arg_list);
    bWrite(f, buffer, len);
    delete[] buffer;
    return len;
}

// STRIPPED
void bWriteToFile(const char *filename, void *buf, int num_bytes) {}

void bAppendToFile(const char *filename, void *buf, int num_bytes) {
    bFile *f = bOpen(filename, BOPEN_MODE_APPEND, 1);
    if (f != nullptr) {
        f->Write(buf, num_bytes);
        bClose(f);
    }
}

static const int bFileDoTimingTest = 0;               // Decl: 2809
static const int bFileTimingTestIncrement = 52428800; // Decl: 2810
static const int bFileTimingTestSize = 5242880;       // Decl: 2811

// TODO
void bFileRunTimingTest() {
    char filename[64];
}
