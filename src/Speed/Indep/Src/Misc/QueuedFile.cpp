#include "QueuedFile.hpp"
#include "Joylog.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Timer.hpp"
#include "bFile.hpp"
#include "Platform.h"

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
            if (Params.BlockSize < readSize) {
                readSize = Params.BlockSize;
            }
            int fileSize = bFileSize(f);
            int seekPos = FilePos + NumRead;
            if (fileSize < seekPos + readSize) {
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
    return after->Params.Priority <= before->Params.Priority;
}

void CheckQueuedFileCallbacks() {
    bServiceFileSystem();
    QueuedFile *qf = static_cast<QueuedFile *>(ReadingQueuedFileList.GetHead());
    if (QueuedFileNumReadsInProgress == 0) {
        return;
    }
    int status = qf->GetStatus();
    if (QueuedFileJoylogEnabled) {
        if (!Joylog::IsReplaying()) {
            Joylog::AddData(status, 4, JOYLOG_CHANNEL_QUEUEDFILE_STATUS);
        } else {
            status = Joylog::GetData(4, JOYLOG_CHANNEL_QUEUEDFILE_STATUS);
            if (status != QREADING) {
                while (qf->GetStatus() == QREADING) {
                    bThreadYield(8);
                    bServiceFileSystem();
                }
            }
        }
    }
    if (status == QDONE) {
        qf->Remove();
        if (qf->NumRead != qf->NumBytes) {
            QueuedFileNumReadsInProgress = QueuedFileNumReadsInProgress - 1;
            qf->Status = QWAITING;
            WaitingQueuedFileList.AddTail(qf);
            return;
        }
        QueuedFileNumReadsInProgress = QueuedFileNumReadsInProgress - 1;
        bStrCopy(LastQueuedFilename, qf->Filename);
        if (qf->CallbackModeUseParam2) {
            ((void (*)(void *, int, void *))qf->CallbackFunction)(qf->CallbackParam, 0, qf->CallbackParam2);
        } else {
            ((void (*)(void *, int))qf->CallbackFunction)(qf->CallbackParam, 0);
        }
        bFree(QueuedFileSlotPool, qf);
    } else if (status == QERROR) {
        qf->Remove();
        QueuedFileNumReadsInProgress = QueuedFileNumReadsInProgress - 1;
        if (qf->CallbackModeUseParam2) {
            ((void (*)(void *, int, void *))qf->CallbackFunction)(qf->CallbackParam, 1, qf->CallbackParam2);
        } else {
            ((void (*)(void *, int))qf->CallbackFunction)(qf->CallbackParam, 1);
        }
        bFree(QueuedFileSlotPool, qf);
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
        qf->Remove();
        ReadingQueuedFileList.AddTail(qf);
        QueuedFileNumReadsInProgress = QueuedFileNumReadsInProgress + 1;
        qf->BeginRead();
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
