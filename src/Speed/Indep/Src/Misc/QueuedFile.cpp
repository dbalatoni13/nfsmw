#include "QueuedFile.hpp"
#include "Joylog.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Timer.hpp"
#include "bFile.hpp"
#include "Platform.h"

// static const int PrintQueuedFiles;
// static const int PrintQueuedFileChecksum;
int QueuedFileDefaultPriority = 5; // size: 0x4
int QueuedFileMinPriority = 0;
int QueuedFileMinPriorityTimeoutCounter = 0;
bool QueuedFileJoylogEnabled = true;
float QueuedFileDebugTimeStart = 0.0f;
SlotPool *QueuedFileSlotPool = nullptr;

int QueuedFile::CurrentHandle = 1;
int QueuedFile::DecompressionTableBot = 0;
int QueuedFile::DecompressionTableTop = 0;
QueuedFile *QueuedFile::DecompressionTable[32];

// static const int QueuedFileBundleMaxReadGap;
// static const int QueuedFileBundleMaxReadSize;
int EnableQueuedFileBundle = true;
int QueuedFileNumReadsInProgress = 0;
// static const int QueuedFileMaxReadsInProgress;
// static const int QueuedFileDontBlock;

bTList<QueuedFile> WaitingQueuedFileList;
bTList<QueuedFile> ReadingQueuedFileList;
char LastQueuedFilename[100];

float GetQueuedFileDebugTime() {
    return GetDebugRealTime() - QueuedFileDebugTimeStart;
}

bool IsQueuedFileJoyloggable(const char *filename) {
    return bMatchNameWithWildcard("Sound\\Speech\\CopSpeech*.big", filename) == 0;
}

void SetQueuedFileMinPriority(int priority) {
    QueuedFileMinPriority = priority;
}

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

    if (params) {
        Params = *params;
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
    bFreeSharedString(Filename);
}

void QueuedFile::BeginRead() {
    ProfileNode profile_node("TODO", 0);
    StartReadTime = GetQueuedFileDebugTime();
    if (NumBytes == 0) {
        Status = QDONE;
        return;
    }

    bFile *f = bOpen(Filename, 1, 1);
    if (f == nullptr) {
        Status = QERROR;
    } else {
        int num_to_read = NumBytes - NumRead;
        if (num_to_read > Params.BlockSize) {
            num_to_read = Params.BlockSize;
        }
        int file_size = bFileSize(f);
        if (FilePos + NumRead + num_to_read > file_size) {
            Status = QERROR;
        } else {
            Status = QREADING;
            bSeek(f, FilePos + NumRead, 0);
            bReadAsync(f, static_cast<char *>(pBuf) + NumRead, num_to_read, &QueuedFile::ReadDoneCallback, this);
            NumRead = NumRead + num_to_read;
        }
        bClose(f);
    }
}

void QueuedFile::ReadDoneCallback() {
    if (Params.Compressed) {
        int table_pos = DecompressionTableTop % 32;
        DecompressionTable[table_pos] = this;
        DecompressionTableTop++;
    } else {
        Status = QDONE;
    }
}

// TODO move
extern int CarLoaderMemoryPoolNumber;

bool QueuedFileBundle::TestAddQueuedFile(QueuedFile *q) {
    if (NumQueuedFiles >= 10) {
        return false;
    }
    if (q->GetNumRead() != 0) {
        return false;
    }
    if (q->Params.Compressed != 0) {
        return false;
    }
    if (NumQueuedFiles >= 1) {
        if (bStrCmp(q->GetFilename(), QueuedFiles[0]->GetFilename()) != 0) {
            return false;
        }
    }
    int read_buffer_bot = q->GetFilePos() & 0xFFFFF800;
    int read_buffer_top = q->GetFilePos() + q->GetNumBytes();
    int num_bytes_queued = q->GetNumBytes() + NumBytesQueued;
    if (NumQueuedFiles > 0) {
        if (static_cast<int>(read_buffer_bot) > ReadBufferBot) {
            read_buffer_bot = ReadBufferBot;
        }
        if (read_buffer_top < ReadBufferTop) {
            read_buffer_top = ReadBufferTop;
        }
    }
    int read_size = read_buffer_top - read_buffer_bot;
    if (read_size >= 0x40001) {
        return false;
    }
    int prev_read_size = ReadBufferTop - ReadBufferBot;
    if (read_size - prev_read_size - q->GetNumBytes() > 0x8000) {
        return false;
    }
    // TODO dwarf (gap uses the wrong register)
    int gap = bLargestMalloc(0) - 0x10000;
    bool not_enough_memory = read_size * 2 > gap;
    int memory_pool = 0;
    if (not_enough_memory) {
        if (bStrNICmp(q->GetFilename(), "CARS\\", 5) == 0) {
            not_enough_memory = read_size > bLargestMalloc(CarLoaderMemoryPoolNumber);
            memory_pool = CarLoaderMemoryPoolNumber;
        }
    }
    bool joylog_not_enough_memory = Joylog::AddOrGetData(not_enough_memory, 1, JOYLOG_CHANNEL_QUEUEDFILE_STATUS) != 0;
    if (joylog_not_enough_memory != not_enough_memory) {
        not_enough_memory = joylog_not_enough_memory;
    }
    if (not_enough_memory) {
        return false;
    }
    MemoryPoolNumber = static_cast<short>(memory_pool);
    ReadBufferBot = read_buffer_bot;
    ReadBufferTop = read_buffer_top;
    NumBytesQueued = num_bytes_queued;
    QueuedFiles[NumQueuedFiles++] = q;
    return true;
}

static const int PrintQueuedFiles = ENABLE_IN_DEBUG;

void QueuedFileBundle::BeginRead() {
    int read_size = ReadBufferTop - ReadBufferBot;
    if (PrintQueuedFiles) {
        for (int n = 0; n < NumQueuedFiles; n++) {
            // TODO what to use instead of bPrintf?
            // bPrintf(" %d", QueuedFiles[n]->Handle);
        }
    }
    ReadBuffer = static_cast<signed char *>(bMalloc(read_size, "QueuedFileBundle", 0, (MemoryPoolNumber & 0xF) | 0x1040));
    AddQueuedFile(ReadBuffer, GetFilename(), ReadBufferBot, read_size, &ReadCallbackBridge, this, nullptr);
    QueuedFile *q = WaitingQueuedFileList.RemoveTail();
    WaitingQueuedFileList.AddHead(q);
}

void QueuedFileBundle::ReadCallback(int error_status) {
    int num_bytes_copied;
    for (int n = 0; n < NumQueuedFiles; n++) {
        QueuedFile *q = QueuedFiles[n];
        bMemCpy(q->GetBuffer(), ReadBuffer + (q->GetFilePos() - ReadBufferBot), q->GetNumBytes());
        num_bytes_copied += q->GetNumBytes();
        q->CallDoneCallback(error_status);
        delete q;
    }
}

// UNSOLVED and a GetStatus call is missing in the dwarf
void CheckQueuedFileCallbacks() {
    bServiceFileSystem();
    if (QueuedFileNumReadsInProgress) {
        QueuedFile *q = ReadingQueuedFileList.GetHead();
        QueuedFileStatus status = q->GetStatus();
        if (QueuedFileJoylogEnabled) {
            if (Joylog::IsReplaying()) {
                ProfileNode profile_node("TODO", 0);
                status = static_cast<QueuedFileStatus>(Joylog::GetData(4, JOYLOG_CHANNEL_QUEUEDFILE_STATUS));
                if (status != QREADING) {
                    while (q->GetStatus() == QREADING) {
                        bThreadYield(8);
                        bServiceFileSystem();
                    }
                }
            } else {
                Joylog::AddData(status, 4, JOYLOG_CHANNEL_QUEUEDFILE_STATUS);
            }
        }
        if (status == QDONE) {
            q->Remove();
            QueuedFileNumReadsInProgress--;
            if (q->IsFinishedAllReading()) {
                ProfileNode profile_node("TODO", 0);
                q->CallDoneCallback(0);
                delete q;
            } else {
                q->SetStatus(QWAITING);
                WaitingQueuedFileList.AddTail(q);
            }
        } else if (status == QERROR) {
            q->Remove();
            QueuedFileNumReadsInProgress--;
            q->CallDoneCallback(1);
            delete q;
        }
    }
}

void StartQueuedFileReading() {
    while (true) {
        // TODO this condition somehow has to go outside, or inside the while
        if ((QueuedFileNumReadsInProgress - QueuedFile::GetNumFilesDecompressing() > 2) || WaitingQueuedFileList.IsEmpty()) {
            return;
        }
        WaitingQueuedFileList.Sort(&QueuedFile::SortByPriority);
        if (QueuedFileNumReadsInProgress > 0) {
            QueuedFile *q = WaitingQueuedFileList.GetHead();
            if (bStrCmp(q->GetFilename(), LastQueuedFilename) != 0) {
                return;
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
                if (q_bundle) {
                    if (q_bundle->ReadBuffer) {
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
            break;
        }
        QueuedFileMinPriorityTimeoutCounter = 0;
        bSafeStrCpy(LastQueuedFilename, q->GetFilename(), sizeof(LastQueuedFilename));
        q->BeginRead();
        WaitingQueuedFileList.Remove(q);
        ReadingQueuedFileList.AddTail(q);
        QueuedFileNumReadsInProgress++;
    }
    if (QueuedFileNumReadsInProgress == 0) {
        if (QueuedFileMinPriorityTimeoutCounter++ > 10) {
            QueuedFileMinPriority = 0;
        }
    }
}

void ServiceQueuedFiles() {
    ProfileNode profile_node("TODO", 0);
    CheckQueuedFileCallbacks();
    StartQueuedFileReading();
}

bool IsQueuedFileBusy() {
    return QueuedFileNumReadsInProgress != 0 || !WaitingQueuedFileList.IsEmpty();
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
    QueuedFileSlotPool = bNewSlotPool(0x48, 200, "QueuedFileSlotPool", 0);
}

int GetQueuedFileSize(const char *filename) {
    return bFileSize(filename);
}

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
