#include "QueuedFile.hpp"
#include "Joylog.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Timer.hpp"
#include "bFile.hpp"
#include "Platform.h"

int bStrNICmp(const char *s1, const char *s2, int n);

// static const int PrintQueuedFiles;
// static const int PrintQueuedFileChecksum;
int QueuedFileMinPriority = 0;
int QueuedFileMinPriorityTimeoutCounter = 0;
bool QueuedFileJoylogEnabled = true;
float QueuedFileDebugTimeStart = 0.0f;
SlotPool *QueuedFileSlotPool = nullptr;
// static const int QueuedFileBundleMaxReadGap;
// static const int QueuedFileBundleMaxReadSize;
int QueuedFileNumReadsInProgress;
// static const int QueuedFileMaxReadsInProgress;
// static const int QueuedFileDontBlock;

// TODO correct place?
int QueuedFileDefaultPriority = 5; // size: 0x4
int EnableQueuedFileBundle = true; // size: 0x4

bTList<QueuedFile> WaitingQueuedFileList;
bTList<QueuedFile> ReadingQueuedFileList;
char LastQueuedFilename[128];

float GetQueuedFileDebugTime() {
    return GetDebugRealTime() - QueuedFileDebugTimeStart;
}

bool IsQueuedFileJoyloggable(const char *filename) {
    return bMatchNameWithWildcard("Sound\\Speech\\CopSpeech*.big", filename) == 0;
}

void SetQueuedFileMinPriority(int priority) {
    QueuedFileMinPriority = priority;
}

// UNSOLVED
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
        this->Params.UncompressedSize = params->UncompressedSize;
        this->Params.BlockSize = params->BlockSize;
        this->Params.Compressed = params->Compressed;
        this->Params.Priority = params->Priority;
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
    StartReadTime = GetQueuedFileDebugTime();
    if (NumBytes == 0) {
        Status = QDONE;
    } else {
        bFile *f = bOpen(Filename, 1, 1);
        if (f == nullptr) {
            Status = QERROR;
        } else {
            int readSize = NumBytes - NumRead;
            if (readSize > Params.BlockSize) {
                readSize = Params.BlockSize;
            }
            int fileSize = bFileSize(f);
            int seekPos = FilePos + NumRead;
            if (seekPos + readSize > fileSize) {
                Status = QERROR;
            } else {
                Status = QREADING;
                bSeek(f, seekPos, 0);
                bReadAsync(f, static_cast<char *>(pBuf) + NumRead, readSize, &QueuedFile::ReadDoneCallback, this);
                NumRead = NumRead + readSize;
            }
            bClose(f);
        }
    }
}

void QueuedFile::ReadDoneCallback() {
    if (Params.Compressed) {
        DecompressionTable[DecompressionTableTop % 32] = this;
        DecompressionTableTop = DecompressionTableTop + 1;
        return;
    }
    Status = QDONE;
}

int QueuedFile::SortByPriority(QueuedFile *before, QueuedFile *after) {
    return before->Params.Priority >= after->Params.Priority;
}

extern int CarLoaderMemoryPoolNumber;

bool QueuedFileBundle::TestAddQueuedFile(QueuedFile *q) {
    if (NumQueuedFiles >= 10) {
        return false;
    }
    if (q->NumRead != 0) {
        return false;
    }
    if (q->Params.Compressed != 0) {
        return false;
    }
    if (NumQueuedFiles >= 1) {
        if (bStrCmp(q->Filename, QueuedFiles[0]->Filename) != 0) {
            return false;
        }
    }
    int numBytes = q->NumBytes;
    int numBytesQueued = NumBytesQueued;
    int top = q->FilePos + numBytes;
    unsigned int bot = q->FilePos & 0xFFFFF800;
    if (NumQueuedFiles > 0) {
        if (static_cast<int>(ReadBufferBot) < static_cast<int>(bot)) {
            bot = ReadBufferBot;
        }
        if (top < ReadBufferTop) {
            top = ReadBufferTop;
        }
    }
    int totalSize = top - bot;
    if (totalSize >= 0x40001) {
        return false;
    }
    if (totalSize - (ReadBufferTop - ReadBufferBot) - numBytes > 0x8000) {
        return false;
    }
    int freeMem = bLargestMalloc(0);
    short poolNum = 0;
    bool tooBig = freeMem - 0x10000 < totalSize * 2;
    if (tooBig) {
        if (bStrNICmp(q->Filename, "Track", 5) == 0) {
            freeMem = bLargestMalloc(CarLoaderMemoryPoolNumber);
            tooBig = freeMem < totalSize;
            poolNum = static_cast<short>(CarLoaderMemoryPoolNumber);
        }
    }
    int joylogResult = Joylog::AddOrGetData(tooBig, 1, JOYLOG_CHANNEL_QUEUEDFILE_STATUS);
    if ((joylogResult != 0) != tooBig) {
        tooBig = joylogResult != 0;
    }
    if (!tooBig) {
        MemoryPoolNumber = poolNum;
        ReadBufferBot = bot;
        ReadBufferTop = top;
        NumBytesQueued = numBytes + numBytesQueued;
        QueuedFiles[NumQueuedFiles] = q;
        NumQueuedFiles = NumQueuedFiles + 1;
        return true;
    }
    return false;
}

void QueuedFileBundle::BeginRead() {
    int size = ReadBufferTop - ReadBufferBot;
    ReadBuffer = static_cast<signed char *>(bMalloc(size, MemoryPoolNumber | 0x1040));
    AddQueuedFile(ReadBuffer, QueuedFiles[0]->Filename, ReadBufferBot, size, ReadCallbackBridge, this, nullptr);
    QueuedFile *qf = static_cast<QueuedFile *>(WaitingQueuedFileList.GetTail());
    WaitingQueuedFileList.Remove(qf);
    ReadingQueuedFileList.AddTail(qf);
}

void QueuedFileBundle::ReadCallback(int error_status) {
    int i = 0;
    if (NumQueuedFiles > 0) {
        do {
            QueuedFile *qf = QueuedFiles[i];
            bMemCpy(qf->pBuf, ReadBuffer + (qf->FilePos - ReadBufferBot), qf->NumBytes);
            void *callback = qf->CallbackFunction;
            if (callback != nullptr) {
                if (qf->CallbackModeUseParam2 == 0) {
                    ((void (*)(void *, int))callback)(qf->CallbackParam, error_status);
                } else {
                    ((void (*)(void *, int, void *))callback)(qf->CallbackParam, error_status, qf->CallbackParam2);
                }
            }
            if (qf != nullptr) {
                delete qf;
            }
            i++;
        } while (i < NumQueuedFiles);
    }
}

void QueuedFileBundle::ReadCallbackBridge(void *param, int error_status) {
    QueuedFileBundle *bundle = static_cast<QueuedFileBundle *>(param);
    bundle->ReadCallback(error_status);
    if (bundle != nullptr) {
        if (bundle->ReadBuffer != nullptr) {
            bFree(bundle->ReadBuffer);
        }
        delete bundle;
    }
}

void CheckQueuedFileCallbacks() {
    bServiceFileSystem();
    if (QueuedFileNumReadsInProgress == 0) {
        return;
    }
    QueuedFile *qf = static_cast<QueuedFile *>(ReadingQueuedFileList.GetHead());
    QueuedFileStatus status = qf->GetStatus();
    if (QueuedFileJoylogEnabled) {
        if (Joylog::IsReplaying()) {
            status = static_cast<QueuedFileStatus>(Joylog::GetData(4, JOYLOG_CHANNEL_QUEUEDFILE_STATUS));
            if (status != QREADING) {
                while (qf->GetStatus() == QREADING) {
                    bThreadYield(8);
                    bServiceFileSystem();
                }
            }
        } else {
            Joylog::AddData(status, 4, JOYLOG_CHANNEL_QUEUEDFILE_STATUS);
        }
    }
    if (status == QDONE) {
        qf->Remove();
        QueuedFileNumReadsInProgress--;
        if (qf->NumRead == qf->NumBytes) {
            bStrCopy(LastQueuedFilename, qf->Filename);
            qf->CallDoneCallback(0);
            delete qf;
        } else {
            qf->SetStatus(QWAITING);
            WaitingQueuedFileList.AddTail(qf);
        }
    } else if (status == QERROR) {
        qf->Remove();
        QueuedFileNumReadsInProgress--;
        qf->CallDoneCallback(1);
        delete qf;
    }
}

void StartQueuedFileReading() {
    while (true) {
        if (QueuedFileNumReadsInProgress - (QueuedFile::DecompressionTableTop - QueuedFile::DecompressionTableBot) > 2) {
            return;
        }
        if (WaitingQueuedFileList.IsEmpty()) {
            return;
        }
        WaitingQueuedFileList.Sort(QueuedFile::SortByPriority);
        QueuedFile *qf = static_cast<QueuedFile *>(WaitingQueuedFileList.GetHead());
        if (QueuedFileNumReadsInProgress > 0 && bStrCmp(qf->Filename, LastQueuedFilename) != 0) {
            return;
        }
        if (EnableQueuedFileBundle != 0) {
            QueuedFileBundle *bundle = new QueuedFileBundle();
            bundle->ReadBuffer = 0;
            bundle->ReadBufferBot = 0;
            bundle->ReadBufferTop = 0;
            bundle->NumBytesQueued = 0;
            bundle->NumQueuedFiles = 0;
            bundle->MemoryPoolNumber = 0;
            QueuedFile *iter = static_cast<QueuedFile *>(WaitingQueuedFileList.GetHead());
            if (iter != reinterpret_cast<QueuedFile *>(&WaitingQueuedFileList)) {
                do {
                    if (!bundle->TestAddQueuedFile(iter)) {
                        break;
                    }
                    iter = static_cast<QueuedFile *>(iter->GetNext());
                } while (iter != reinterpret_cast<QueuedFile *>(&WaitingQueuedFileList));
            }
            if (bundle->NumQueuedFiles <= 1) {
                if (bundle != 0) {
                    if (bundle->ReadBuffer != 0) {
                        bFree(bundle->ReadBuffer);
                    }
                    delete bundle;
                }
            } else {
                bundle->BeginRead();
                for (int i = 0; i < bundle->NumQueuedFiles; i++) {
                    bundle->QueuedFiles[i]->Remove();
                }
            }
        }
        qf = static_cast<QueuedFile *>(WaitingQueuedFileList.GetHead());
        if (qf->Params.Priority < QueuedFileMinPriority) {
            break;
        }
        QueuedFileMinPriorityTimeoutCounter = 0;
        bSafeStrCpy(LastQueuedFilename, qf->Filename, 0x64);
        qf->BeginRead();
        qf->Remove();
        ReadingQueuedFileList.AddTail(qf);
        QueuedFileNumReadsInProgress = QueuedFileNumReadsInProgress + 1;
    }
    if (QueuedFileNumReadsInProgress == 0) {
        if (QueuedFileMinPriorityTimeoutCounter++ > 10) {
            QueuedFileMinPriority = 0;
        }
    }
}

void ServiceQueuedFiles() {
    CheckQueuedFileCallbacks();
    StartQueuedFileReading();
}

void InitQueuedFiles() {
    QueuedFileSlotPool = bNewSlotPool(0x48, 200, "QueuedFile", 0);
}

int IsQueuedFileBusy() {
    if (QueuedFileNumReadsInProgress == 0 && WaitingQueuedFileList.IsEmpty()) {
        return 0;
    }
    return 1;
}

void BlockWhileQueuedFileBusy() {
    ServiceQueuedFiles();
    while (IsQueuedFileBusy()) {
        DVDErrorTask(nullptr, 0);
        bThreadYield(8);
        ServiceQueuedFiles();
    }
}

int GetQueuedFileSize(const char *filename) {
    return bFileSize(filename);
}

void AddQueuedFile(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(void *, int), void *callback_param,
                   QueuedFileParams *params) {
    if (!IsQueuedFileJoyloggable(filename)) {
        bFile *f = bOpen(filename, 1, 1);
        bSeek(f, file_pos, 0);
        bReadAsync(f, buf, num_bytes, ((void (*)(void *))callback), callback_param);
        bClose(f);
    } else {
        QueuedFile *qf = new (bOMalloc(QueuedFileSlotPool)) QueuedFile(buf, filename, file_pos, num_bytes, ((void *)callback), callback_param, params);
        WaitingQueuedFileList.AddTail(qf);
    }
}

void AddQueuedFile2(void *buf, const char *filename, int file_pos, int num_bytes, void (*callback)(void *, int, void *), void *callback_param,
                    void *callback_param2, QueuedFileParams *params) {
    QueuedFile *qf = new (bOMalloc(QueuedFileSlotPool)) QueuedFile(buf, filename, file_pos, num_bytes, ((void *)callback), callback_param, params);
    qf->SetCallbackParam2(callback_param2);
    qf->CallbackModeUseParam2 = 1;
    WaitingQueuedFileList.AddTail(qf);
}
