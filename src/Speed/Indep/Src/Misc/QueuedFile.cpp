#include "QueuedFile.hpp"
#include "Joylog.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Timer.hpp"

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
