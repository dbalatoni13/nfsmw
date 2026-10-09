#include "QueuedFile.hpp"
#include "Joylog.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Timer.hpp"
#include "bFile.hpp"
#include "Platform.h"

static const int PrintQueuedFiles = ENABLE_IN_DEBUG; // size: 0x4, Decl: 27
static const int PrintQueuedFileChecksum = 0;        // size: 0x4, Decl: 28

int QueuedFileDefaultPriority = 5;           // // Decl: 30
int QueuedFileMinPriority = 0;               // Decl: 31
int QueuedFileMinPriorityTimeoutCounter = 0; // Decl: 32

bool QueuedFileJoylogEnabled = true;   // Decl. 34
float QueuedFileDebugTimeStart = 0.0f; // Decl: 39

// STRIPPED
void ResetQueuedFileDebugTime() {}

// Decl: 46
float GetQueuedFileDebugTime() {
    return GetDebugRealTime() - QueuedFileDebugTimeStart;
}

bool IsQueuedFileJoyloggable(const char *filename) {
    return bMatchNameWithWildcard("Sound\\Speech\\CopSpeech*.big", filename) == 0;
}

void SetQueuedFileMinPriority(int priority) {
    QueuedFileMinPriority = priority;
}

SlotPool *QueuedFileSlotPool = nullptr; // Decl: 68

// Decl: 70
enum QueuedFileStatus {
    QWAITING = 0,
    QREADING = 1,
    QDONE = 2,
    QERROR = 3,
};

// total size: 0x48
// Decl: 79
class QueuedFile : public bTNode<QueuedFile> {
  public:
    QueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void *callback_function, void *callback_param, QueuedFileParams *params);
    ~QueuedFile();

    void *operator new(size_t size, void *ptr) {
        return ptr;
    }

    void operator delete(void *ptr) {
        bFree(QueuedFileSlotPool, ptr);
    }

    int GetHandle() {
        return this->Handle;
    }

    const char *GetFilename() {
        return this->Filename;
    }

    int GetFilePos() {
        return this->FilePos;
    }

    int GetNumBytes() {
        return this->NumBytes;
    }

    int GetNumRead() {
        return this->NumRead;
    }

    void *GetBuffer() {
        return this->pBuf;
    }

    float GetStartReadTime() {
        return this->StartReadTime;
    }

    int GetPriority() {
        return this->Params.Priority;
    }

    QueuedFileStatus GetStatus() {
        return this->Status;
    }

    void SetStatus(QueuedFileStatus status) {
        this->Status = status;
    }

    void BeginRead();

    int IsFinishedAllReading() {
        return static_cast<int>(this->NumRead == this->NumBytes);
    }

    uint32 CalculateChecksum() {
        return bCalculateCrc32(pBuf, this->NumBytes, 0xFFFFFFFF);
    }

    char *GetDebugPrintHeader();

    void *GetCallback() {
        return this->CallbackFunction;
    }

    void SetCallbackParam2(void *param) {
        this->CallbackParam2 = param;
        this->CallbackModeUseParam2 = 1;
    }

    void CallDoneCallback(int error_status) {
        if (this->CallbackFunction != nullptr) {
            if (this->CallbackModeUseParam2 != 0) {
                ((void (*)(void *, int, void *))this->CallbackFunction)(this->CallbackParam, error_status, this->CallbackParam2);
            } else {
                ((void (*)(void *, int))this->CallbackFunction)(this->CallbackParam, error_status);
            }
        }
    }

    static int SortByPriority(QueuedFile *before, QueuedFile *after) {
        return static_cast<int>(before->Params.Priority >= after->Params.Priority);
    }

    // static int SortBySeekPosition(QueuedFile *before, QueuedFile *after) {}

    // Decl: 126
    static void ServiceDecompression();

    // Decl: 128
    static int GetNumFilesDecompressing() {
        return DecompressionTableTop - DecompressionTableBot;
    }

    friend class QueuedFileBundle;

  private:
    // Decl: 131
    static void ReadDoneCallback(void *param) {
        static_cast<QueuedFile *>(param)->ReadDoneCallback();
    }
    void ReadDoneCallback(); // Decl: 132

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
    float StartReadTime;       // offset 0x44, size 0x4, Decl: 150

    static int CurrentHandle;
    static int DecompressionTableBot;
    static int DecompressionTableTop;
    static QueuedFile *DecompressionTable[32];
};

int QueuedFile::CurrentHandle = 1;              // Decl: 152
int QueuedFile::DecompressionTableBot = 0;      // Decl: 154
int QueuedFile::DecompressionTableTop = 0;      // Decl: 155
QueuedFile *QueuedFile::DecompressionTable[32]; // Decl: 156

QueuedFile::QueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void *callback_function, void *callback_param,
                       QueuedFileParams *params) {
    this->Params.Priority = QueuedFileDefaultPriority;
    this->Params.BlockSize = 0x7ffffff;
    this->pBuf = buf;
    this->FilePos = file_pos;
    this->NumBytes = num_bytes;
    this->Params.Compressed = false;
    this->Params.UncompressedSize = 0;
    this->NumRead = 0;

    if (params != nullptr) {
        this->Params = *params;
    }
    this->Filename = bAllocateSharedString(filename);
    this->CallbackFunction = callback_function;
    this->CallbackParam = callback_param;
    this->CallbackParam2 = nullptr;
    this->CallbackModeUseParam2 = 0;
    this->Handle = QueuedFile::CurrentHandle;
    this->Status = QWAITING;
    this->StartReadTime = 0.0f;
    if (++QueuedFile::CurrentHandle > 100000) {
        QueuedFile::CurrentHandle = 1;
    }
    if (Joylog::IsCapturing()) {
        int len = bStrLen(filename);
        Joylog::AddData(len + 1, 8, JOYLOG_CHANNEL_QUEUEDFILENAME);
        Joylog::AddData(filename, len + 1, JOYLOG_CHANNEL_QUEUEDFILENAME);
    } else {
        if (Joylog::IsReplaying()) {
            char temp_filename[128];
            int len = Joylog::GetData(8, JOYLOG_CHANNEL_QUEUEDFILENAME);
            Joylog::GetData(temp_filename, len, JOYLOG_CHANNEL_QUEUEDFILENAME);
            if (!bStrEqual(temp_filename, filename)) {
                bBreak();
            }
        }
    }
}

QueuedFile::~QueuedFile() {
    bFreeSharedString(this->Filename);
}

// STRIPPED
char *QueuedFile::GetDebugPrintHeader() {}

void QueuedFile::BeginRead() {
    ProfileNode profile_node("TODO", 0);
    this->StartReadTime = GetQueuedFileDebugTime();
    if (this->NumBytes == 0) {
        this->Status = QDONE;
        return;
    }

    bFile *f = bOpen(this->Filename, 1, 1);
    if (f == nullptr) {
        this->Status = QERROR;
    } else {
        int num_to_read = this->NumBytes - this->NumRead;
        if (num_to_read > this->Params.BlockSize) {
            num_to_read = this->Params.BlockSize;
        }
        int file_size = bFileSize(f);
        if (this->FilePos + this->NumRead + num_to_read > file_size) {
            this->Status = QERROR;
        } else {
            this->Status = QREADING;
            bSeek(f, this->FilePos + this->NumRead, 0);
            bReadAsync(f, static_cast<char *>(pBuf) + this->NumRead, num_to_read, &QueuedFile::ReadDoneCallback, this);
            this->NumRead += num_to_read;
        }
        bClose(f);
    }
}

void QueuedFile::ReadDoneCallback() {
    if (this->Params.Compressed) {
        int table_pos = this->DecompressionTableTop % 32;
        this->DecompressionTable[table_pos] = this;
        this->DecompressionTableTop++;
    } else {
        this->Status = QDONE;
    }
}

// total size: 0x3C
// Decl: 331
class QueuedFileBundle {
  public:
    QueuedFileBundle() {
        this->ReadBuffer = nullptr;
        this->ReadBufferBot = 0;
        this->ReadBufferTop = 0;
        this->NumBytesQueued = 0;
        this->NumQueuedFiles = 0;
        this->MemoryPoolNumber = 0;
    }

    ~QueuedFileBundle() {}

    void *operator new(size_t size) {
        return bOMalloc(QueuedFileSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(QueuedFileSlotPool, ptr);
    }

    const char *GetFilename() {
        return this->QueuedFiles[0]->GetFilename();
    }

    bool TestAddQueuedFile(QueuedFile *q);
    void BeginRead();
    void ReadCallback(int error_status);

    static void ReadCallbackBridge(void *param, int error_status) {
        static_cast<QueuedFileBundle *>(param)->ReadCallback(error_status);
        if (param != nullptr) {
            if (static_cast<QueuedFileBundle *>(param)->ReadBuffer != nullptr) {
                bFree(static_cast<QueuedFileBundle *>(param)->ReadBuffer);
            }
            delete static_cast<QueuedFileBundle *>(param);
        }
    }

    int8 *ReadBuffer;            // offset 0x0, size 0x4
    int ReadBufferBot;           // offset 0x4, size 0x4
    int ReadBufferTop;           // offset 0x8, size 0x4
    int NumBytesQueued;          // offset 0xC, size 0x4
    int16 MemoryPoolNumber;      // offset 0x10, size 0x2
    int16 NumQueuedFiles;        // offset 0x12, size 0x2
    QueuedFile *QueuedFiles[10]; // offset 0x14, size 0x28
};

static const int QueuedFileBundleMaxReadGap = 32 * 1024;   // Decl: 364
static const int QueuedFileBundleMaxReadSize = 256 * 1024; // Decl: 365

int EnableQueuedFileBundle = 1; // Decl: 367

// Decl: 370
bool QueuedFileBundle::TestAddQueuedFile(QueuedFile *q) {
    if (this->NumQueuedFiles >= 10) {
        return false;
    }
    if (q->GetNumRead() != 0) {
        return false;
    }
    if (q->Params.Compressed) {
        return false;
    }
    if (this->NumQueuedFiles > 0) {
        if (bStrCmp(q->GetFilename(), this->QueuedFiles[0]->GetFilename()) != 0) {
            return false;
        }
    }
    int read_buffer_bot = q->GetFilePos() & 0xFFFFF800;
    int read_buffer_top = q->GetFilePos() + q->GetNumBytes();
    int num_bytes_queued = q->GetNumBytes() + this->NumBytesQueued;
    if (this->NumQueuedFiles > 0) {
        if (static_cast<int>(read_buffer_bot) > this->ReadBufferBot) {
            read_buffer_bot = this->ReadBufferBot;
        }
        if (read_buffer_top < this->ReadBufferTop) {
            read_buffer_top = this->ReadBufferTop;
        }
    }
    int read_size = read_buffer_top - read_buffer_bot;
    if (read_size > QueuedFileBundleMaxReadSize) {
        return false;
    }
    int prev_read_size = this->ReadBufferTop - this->ReadBufferBot;
    if (read_size - prev_read_size - q->GetNumBytes() > QueuedFileBundleMaxReadGap) {
        return false;
    }
    // TODO dwarf (gap uses the wrong register)
    int gap = bLargestMalloc(0) - 0x10000;
    bool not_enough_memory = read_size * 2 > gap;
    int memory_pool = 0;
    if (not_enough_memory) {
        if (bStrNICmp(q->GetFilename(), "CARS\\", 5) == 0) {
            extern int CarLoaderMemoryPoolNumber;
            not_enough_memory = read_size > bLargestMalloc(CarLoaderMemoryPoolNumber);
            memory_pool = CarLoaderMemoryPoolNumber;
        }
    }
    bool joylog_not_enough_memory = Joylog::AddOrGetData(static_cast<uint32>(not_enough_memory), 1, JOYLOG_CHANNEL_QUEUEDFILE_STATUS) != 0;
    if (joylog_not_enough_memory != not_enough_memory) {
        not_enough_memory = joylog_not_enough_memory;
    }
    if (not_enough_memory) {
        return false;
    }
    this->MemoryPoolNumber = static_cast<short>(memory_pool);
    this->ReadBufferBot = read_buffer_bot;
    this->ReadBufferTop = read_buffer_top;
    this->NumBytesQueued = num_bytes_queued;
    this->QueuedFiles[this->NumQueuedFiles++] = q;
    return true;
}

void QueuedFileBundle::BeginRead() {
    int read_size = this->ReadBufferTop - this->ReadBufferBot;
    if (PrintQueuedFiles) {
        for (int n = 0; n < this->NumQueuedFiles; n++) {
            // TODO what to use instead of bPrintf?
            // bPrintf(" %d", QueuedFiles[n]->Handle);
        }
    }
    ReadBuffer = static_cast<signed char *>(bMalloc(read_size, "QueuedFileBundle", 0, (this->MemoryPoolNumber & 0xF) | 0x1040));
    AddQueuedFile(ReadBuffer, this->GetFilename(), ReadBufferBot, read_size, &ReadCallbackBridge, this, nullptr);
    extern bTList<QueuedFile> WaitingQueuedFileList; // Decl: 489

    QueuedFile *q = WaitingQueuedFileList.RemoveTail();
    WaitingQueuedFileList.AddHead(q);
}

void QueuedFileBundle::ReadCallback(int error_status) {
    int num_bytes_copied;
    for (int n = 0; n < this->NumQueuedFiles; n++) {
        QueuedFile *q = this->QueuedFiles[n];
        bMemCpy(q->GetBuffer(), this->ReadBuffer + (q->GetFilePos() - this->ReadBufferBot), q->GetNumBytes());
        num_bytes_copied += q->GetNumBytes();
        q->CallDoneCallback(error_status);
        delete q;
    }
}

int QueuedFileNumReadsInProgress = 0; // Decl: 522

static const int QueuedFileMaxReadsInProgress = 3; // Decl: 527

static const int QueuedFileDontBlock = 1; // Decl: 530

bTList<QueuedFile> WaitingQueuedFileList; // Decl: 532
bTList<QueuedFile> ReadingQueuedFileList; // Decl: 533

static const int QueuedFileBlockingDecompression = 0; // Decl: 535

char LastQueuedFilename[100]; // Decl: 537

void CheckQueuedFileCallbacks() {
    bServiceFileSystem();
    if (QueuedFileNumReadsInProgress) {
        QueuedFile *q = ReadingQueuedFileList.GetHead();
        QueuedFileStatus status = q->GetStatus();
        if (QueuedFileJoylogEnabled) {
            if (Joylog::IsReplaying()) {
                status = static_cast<QueuedFileStatus>(Joylog::GetData(4, JOYLOG_CHANNEL_QUEUEDFILE_STATUS));
                if (status != QREADING) {
                    ProfileNode profile_node("TODO", 0);
                    while (q->GetStatus() == QREADING) {
                        bThreadYield(8);
                        bServiceFileSystem();
                    }
                    q->GetStatus();
                }
            } else {
                Joylog::AddData(status, 4, JOYLOG_CHANNEL_QUEUEDFILE_STATUS);
            }
        }
        if (status == QDONE) {
            ReadingQueuedFileList.Remove(q);
            QueuedFileNumReadsInProgress--;
            if (q->IsFinishedAllReading()) {
                ProfileNode profile_node("TODO", 0);
                q->CallDoneCallback(0);
            } else {
                q->SetStatus(QWAITING);
                WaitingQueuedFileList.AddTail(q);
                return;
            }
        } else {
            if (status == QERROR) {
                ReadingQueuedFileList.Remove(q);
                QueuedFileNumReadsInProgress--;
                q->CallDoneCallback(1);
            } else {
                // hack
                do {
                } while (false);
                return;
            }
        }
        delete q;
        q = nullptr;
    }
}

// STRIPPED
void SortQueuedFiles(const char *filename) {}

void StartQueuedFileReading() {
    // TODO
    while (true) {
        if ((QueuedFileNumReadsInProgress - QueuedFile::GetNumFilesDecompressing() >= QueuedFileMaxReadsInProgress) ||
            WaitingQueuedFileList.IsEmpty()) {
            return;
        } else {
            WaitingQueuedFileList.Sort(&QueuedFile::SortByPriority);
            if (QueuedFileDontBlock) {
                QueuedFile *q = WaitingQueuedFileList.GetHead();
                if (QueuedFileNumReadsInProgress > 0) {
                    if (bStrCmp(q->GetFilename(), LastQueuedFilename) != 0) {
                        return;
                    }
                }
            }
            if (EnableQueuedFileBundle) {
                QueuedFileBundle *q_bundle = new QueuedFileBundle();

                for (QueuedFile *q = WaitingQueuedFileList.GetHead(); q != WaitingQueuedFileList.EndOfList(); q = q->GetNext()) {
                    if (!q_bundle->TestAddQueuedFile(q)) {
                        break;
                    }
                }
                if (q_bundle->NumQueuedFiles <= 1) {
                    if (q_bundle != nullptr) {
                        if (q_bundle->ReadBuffer != nullptr) {
                            bFree(q_bundle->ReadBuffer);
                        }
                        delete q_bundle;
                    }
                } else {
                    q_bundle->BeginRead();
                    for (int n = 0; n < q_bundle->NumQueuedFiles; n++) {
                        WaitingQueuedFileList.Remove(q_bundle->QueuedFiles[n]);
                    }
                }
            }
            QueuedFile *q = WaitingQueuedFileList.GetHead();
            if (q->GetPriority() < QueuedFileMinPriority) {
                if (QueuedFileNumReadsInProgress == 0) {
                    if (QueuedFileMinPriorityTimeoutCounter++ > 10) {
                        QueuedFileMinPriority = 0;
                    }
                }
                return;
            }
            QueuedFileMinPriorityTimeoutCounter = 0;
            bSafeStrCpy(LastQueuedFilename, q->GetFilename(), sizeof(LastQueuedFilename));
            q->BeginRead();
            WaitingQueuedFileList.Remove(q);
            ReadingQueuedFileList.AddTail(q);
            QueuedFileNumReadsInProgress++;
        }
    }
}

void ServiceQueuedFiles() {
    ProfileNode profile_node("TODO", 0);
    CheckQueuedFileCallbacks();
    StartQueuedFileReading();
}

bool IsQueuedFileBusy() {
    return QueuedFileNumReadsInProgress != 0 || (WaitingQueuedFileList.IsEmpty() == 0);
}

void BlockWhileQueuedFileBusy() {
    ServiceQueuedFiles();
    while (IsQueuedFileBusy()) {
        DVDErrorTask(nullptr, 0);
        bThreadYield(8);
        ServiceQueuedFiles();
    }
}

void InitQueuedFiles() {
    const int most_expected_queued_files = 0; // TODO
    QueuedFileSlotPool = bNewSlotPool(0x48, 200, "QueuedFileSlotPool", 0);
    if (!QueuedFileBlockingDecompression) {
        // TODO
    }
}

// STRIPPED
void CloseQueuedFiles() {}

int GetQueuedFileSize(const char *filename) {
    return bFileSize(filename);
}

// STRIPPED
void AddQueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(void *), void *callback_param,
                   QueuedFileParams *params) {}

void AddQueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(void *, int), void *callback_param,
                   QueuedFileParams *params) {
    QueuedFile *q;
    if (!IsQueuedFileJoyloggable(filename)) {
        bFile *f = bOpen(filename, 1, 1);
        bSeek(f, file_pos, 0);
        bReadAsync(f, buf, num_bytes, ((void (*)(void *))callback), callback_param);
        bClose(f);
    } else {
        q = new (bOMalloc(QueuedFileSlotPool)) QueuedFile(buf, filename, file_pos, num_bytes, ((void *)callback), callback_param, params);
        WaitingQueuedFileList.AddTail(q);
    }
}

void AddQueuedFile2(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(void *, int, void *), void *callback_param,
                    void *callback_param2, QueuedFileParams *params) {
    QueuedFile *q = new (bOMalloc(QueuedFileSlotPool)) QueuedFile(buf, filename, file_pos, num_bytes, ((void *)callback), callback_param, params);
    q->SetCallbackParam2(callback_param2);
    WaitingQueuedFileList.AddTail(q);
}

// STRIPPED
void QueuedFileSetJoylogEnabled(bool enabled) {}
