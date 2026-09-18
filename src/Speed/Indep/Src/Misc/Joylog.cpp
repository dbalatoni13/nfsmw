#include "Joylog.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "bFile.hpp"
#include "Config.h"
#include <cstring>

int EnableJoylog = ENABLE_IN_MILESTONE; // Decl: 110

int SaveJoylog = 0; // Decl: 120

static const int PrintJoylog = 0; // Decl: 123

// TODO use these
static const int JoylogScreenPrintX = -300; // Decl: 126
static const int JoylogScreenPrintY = 0;    // Decl: 127

int JoylogThrottleCounter[2];        // Decl: 130
int JoylogThrottleTicks[2];          // Decl: 131
int CurrentJoylogThrottleBuffer = 0; // Decl: 132

static const int JoylogMaxThrottleCounter = 2048; // Decl: 135
static const float JoylogThrottleTime = 30.0f;    // Decl: 136

// total size: 0xC
// Decl: 145
struct JoylogChannelInfo {
    JoylogChannel ChannelNumber; // offset 0x0, size 0x4, Decl: 146
    char *Name;                  // offset 0x4, size 0x4, Decl: 147
    int8 YieldRepeatCount;       // offset 0x8, size 0x1, Decl: 148
    int8 ReadAheadOnly;          // offset 0x9, size 0x1, Decl: 149
};

// Decl: 154
JoylogChannelInfo NFSJoylogChannelInfoTable[MAX_JOYLOG_CHANNELS] = {
    {JOYLOG_CHANNEL_NONE, "JOYLOG_CHANNEL_NONE", 0, 0},
    {JOYLOG_CHANNEL_CONFIG, "JOYLOG_CHANNEL_CONFIG", 0, 0},
    {JOYLOG_CHANNEL_JOYEVENTS, "JOYLOG_CHANNEL_JOYEVENTS", 0, 0},
    {JOYLOG_CHANNEL_TIMESTEP, "JOYLOG_CHANNEL_TIMESTEP", 16, 0},
    {JOYLOG_CHANNEL_CHECKSUM, "JOYLOG_CHANNEL_CHECKSUM", 0, 0},
    {JOYLOG_CHANNEL_LOOP_COUNTER, "JOYLOG_CHANNEL_LOOP_COUNTER", 0, 0},
    {JOYLOG_CHANNEL_QUEUEDFILE_STATUS, "JOYLOG_CHANNEL_QUEUEDFILE_STATUS", 16, 0},
    {JOYLOG_CHANNEL_QUEUEDFILENAME, "JOYLOG_CHANNEL_QUEUEDFILENAME", 0, 0},
    {JOYLOG_CHANNEL_MOVIE_PLAYER_STATUS, "JOYLOG_CHANNEL_MOVIE_PLAYER_STATUS", 0, 0},
    {JOYLOG_CHANNEL_MEMORY_CARD, "JOYLOG_CHANNEL_MEMORY_CARD", 0, 0},
    {JOYLOG_CHANNEL_RANDOM, "JOYLOG_CHANNEL_RANDOM", 0, 0},
    {JOYLOG_CHANNEL_PRINTF, "JOYLOG_CHANNEL_PRINTF", 0, 1},
    {JOYLOG_CHANNEL_SOUND_LOADING, "JOYLOG_CHANNEL_SOUND_LOADING", 0, 0},
    {JOYLOG_CHANNEL_PATHFINDER_TIMEOUT, "JOYLOG_CHANNEL_PATHFINDER_TIMEOUT", 0, 0},
};

// Decl: 174
inline JoylogChannelInfo *GetJoylogChannelInfo(int channel_number) {
    return &NFSJoylogChannelInfoTable[channel_number];
}

// Decl: 176
// STRIPPED: char *GetJoylogChannelName(int channel_number)

// Decl: 182
// STRIPPED: int GetJoylogChannelRepeatCount(int channel_number)

// total size: 0xC
// Decl: 200
struct JoylogBufferEntry {
    int ChannelNumber; // offset 0x0, size 0x4
    int DataSize;      // offset 0x4, size 0x4
    uint32 Data;       // offset 0x8, size 0x4
};

// total size: 0x4118
// Decl: 252
class JoylogBuffer {
  public:
    JoylogBuffer(const char *filename, int top_position);

    void SaveBuffer();

    void LoadBuffer(int position);

    void AddData(int32 data, int data_size, int channel_number);

    uint32 GetData(int data_size, int channel_number);

    int AddEntry(JoylogBufferEntry *entry, int position);

    int GetEntry(JoylogBufferEntry *entry, int position);

    static int GetEntry(JoylogBufferEntry *entry, uint8 *pbuf);

    void PrintNearbyJoylogEntries(int error_pos);

    char *GetFilename() {
        return Filename;
    }

    int GetTotalSize() {
        return TopPosition;
    }

    int IsMoreData() {
        return static_cast<int>(CurrentPosition < TopPosition);
    }

    int GetPosition() {
        return CurrentPosition;
    }

    void SetPosition(int position) {}

  private:
    int32 CurrentPosition;     // offset 0x0, size 0x4
    int32 NumBytesInBuffer;    // offset 0x4, size 0x4
    int32 CurrentLoadPosition; // offset 0x8, size 0x4
    int32 BufferStartPosition; // offset 0xC, size 0x4
    int32 TopPosition;         // offset 0x10, size 0x4
    char Filename[260];        // offset 0x14, size 0x104
    uint8 Buffer[16384];       // offset 0x118, size 0x4000
};

JoylogBuffer::JoylogBuffer(const char *filename, int top_position) {
    this->TopPosition = top_position;
    bStrCpy(this->Filename, filename);
    this->CurrentPosition = 0;
    this->NumBytesInBuffer = 0;
    this->CurrentLoadPosition = 0;
    this->BufferStartPosition = 0;
    bMemSet(this->Buffer, 0, sizeof(this->Buffer));
    if (bIsCodeineConnected()) {
        bFunkJoylogCaptureInitPacket packet = {};
        packet.BufferAddress = reinterpret_cast<intptr_t>(&this->Buffer);
        packet.BufferSizeAddress = reinterpret_cast<intptr_t>(&this->NumBytesInBuffer);

        bFunkCallASync("CODEINE", 100, &packet, 8);
    }
}

void JoylogBuffer::SaveBuffer() {
    ProfileNode profile_node;

    int flush_size = this->CurrentPosition - this->BufferStartPosition;
    if (flush_size > 0) {
        if (bIsCodeineConnected()) {
            int amount_flushed = 0;
            while (amount_flushed < flush_size) {
                int amount_to_flush = flush_size - amount_flushed;
                if (amount_to_flush > 0x7e0) {
                    amount_to_flush = 0x7e0;
                }
                unsigned char *pbuf = &this->Buffer[amount_flushed];
                bFunkCallASync("CODEINE", 101, pbuf, amount_to_flush);
                amount_flushed += amount_to_flush;
            }
        } else {
            bAppendToFile(this->Filename, this->Buffer, flush_size);
        }
        this->NumBytesInBuffer = 0;
        this->BufferStartPosition = this->CurrentPosition;
    }
}

void JoylogBuffer::LoadBuffer(int position) {
    ProfileNode profile_node;

    this->CurrentPosition = position;
    int amount_to_load = this->TopPosition - position;
    if (amount_to_load > 0x4000) {
        amount_to_load = 0x4000;
    }
    if (amount_to_load > 0) {
        bFile *f = bOpen(this->Filename, 1, 1);
        bSeek(f, this->CurrentPosition, 0);
        bRead(f, this->Buffer, amount_to_load);
        bClose(f);
        this->BufferStartPosition = this->CurrentPosition;
        this->CurrentLoadPosition = this->CurrentPosition + amount_to_load;
    }
}

void JoylogBuffer::AddData(int32 data, int data_size, int channel_number) {
    JoylogBufferEntry buffer_entry;
    buffer_entry.DataSize = (data_size + 7) >> 3;
    buffer_entry.Data = data;
    buffer_entry.ChannelNumber = channel_number;

    if ((JoylogThrottleCounter[CurrentJoylogThrottleBuffer] += buffer_entry.DataSize) > 0x800) {
        float time = bGetTickerDifference(JoylogThrottleTicks[CurrentJoylogThrottleBuffer]);
        if (time < 30.0f) {
            while (bGetTickerDifference(JoylogThrottleTicks[CurrentJoylogThrottleBuffer]) < 30.0f) {
                bThreadYield(1);
            }
        }
        CurrentJoylogThrottleBuffer ^= 1;
        JoylogThrottleCounter[CurrentJoylogThrottleBuffer] = 0;
        JoylogThrottleTicks[CurrentJoylogThrottleBuffer] = bGetTicker();
    }
    this->CurrentPosition = this->TopPosition = this->AddEntry(&buffer_entry, this->CurrentPosition);
    this->NumBytesInBuffer = this->CurrentPosition - this->BufferStartPosition;
    int free_space = sizeof(this->Buffer) - this->NumBytesInBuffer;
    if (free_space < 256) {
        this->SaveBuffer();
    }
}

uint32 JoylogBuffer::GetData(int data_size, int channel_number) {
    JoylogBufferEntry buffer_entry;
    int prev_position = CurrentPosition;

    do {
        if (CurrentLoadPosition - CurrentPosition < 0x100 && CurrentLoadPosition != TopPosition) {
            this->LoadBuffer(CurrentPosition);
        }
        CurrentPosition = this->GetEntry(&buffer_entry, CurrentPosition);
    } while (GetJoylogChannelInfo(buffer_entry.ChannelNumber)->ReadAheadOnly != 0);

    int lookahead_pos = CurrentPosition;
    while (this->IsMoreData()) {
        if (CurrentLoadPosition - CurrentPosition < 0x100 && CurrentLoadPosition != TopPosition) {
            this->LoadBuffer(CurrentPosition);
        }
        JoylogBufferEntry lookahead_buffer_entry;
        lookahead_pos = this->GetEntry(&lookahead_buffer_entry, lookahead_pos);
        if (GetJoylogChannelInfo(lookahead_buffer_entry.ChannelNumber)->ReadAheadOnly == 0) {
            break;
        }
        CurrentPosition = lookahead_pos;
    }

    int percent_complete = CurrentPosition * 100 / this->GetTotalSize();
    if (buffer_entry.ChannelNumber != channel_number) {
        this->PrintNearbyJoylogEntries(prev_position);
        bBreak();
        CurrentPosition = TopPosition;
    }

    return buffer_entry.Data;
}

int JoylogBuffer::AddEntry(JoylogBufferEntry *entry, int position) {
    int buffer_index = (position - BufferStartPosition) + 0x118;
    uint8 *pbuf = reinterpret_cast<uint8 *>(this) + buffer_index;
    pbuf[0] = static_cast<uint8>(entry->ChannelNumber + static_cast<uint8>(entry->DataSize) * 16);
    uint32 data = entry->Data;
    pbuf++;
    for (int byte_num = 0; byte_num < entry->DataSize; byte_num++) {
        pbuf[byte_num] = static_cast<uint8>(data);
        data >>= 8;
    }
    return 1 + position + entry->DataSize;
}

int JoylogBuffer::GetEntry(JoylogBufferEntry *entry, int position) {
    int buffer_index = (position - BufferStartPosition) + 0x118;
    uint8 *pbuf = reinterpret_cast<uint8 *>(this) + buffer_index;
    return position + GetEntry(entry, pbuf);
}

int JoylogBuffer::GetEntry(JoylogBufferEntry *entry, uint8 *pbuf) {
    entry->DataSize = pbuf[0] >> 4;
    entry->ChannelNumber = pbuf[0] & 0xF;
    pbuf++;
    uint32 data = 0;
    for (int byte_num = 0; byte_num < entry->DataSize; byte_num++) {
        data = (data << 8);
        data |= pbuf[entry->DataSize - byte_num - 1];
    }
    entry->Data = data;
    return entry->DataSize + 1;
}

void JoylogBuffer::PrintNearbyJoylogEntries(int error_pos) {
    int pos = this->BufferStartPosition;
    const int range = 40;
    while (pos < this->TopPosition && pos < this->CurrentPosition + range) {
        if (this->CurrentLoadPosition != 0 && pos >= this->CurrentLoadPosition) {
            break;
        }
        JoylogBufferEntry buffer_entry;
        int new_pos = this->GetEntry(&buffer_entry, pos);
        {
            const char *current_string = (pos <= this->CurrentPosition && this->CurrentPosition < new_pos) ? "***" : "   ";
        }
        pos = new_pos;
    }
}

int Joylog::ReplayingFlag = 0;                    // Decl: 609
int Joylog::CapturingFlag = 0;                    // Decl: 610
JoylogBuffer *Joylog::pReplayingBuffer = nullptr; // Decl: 611
JoylogBuffer *Joylog::pCapturingBuffer = nullptr; // Decl: 612

int32 Joylog::ReadAheadBufferSize = 0;    // Decl: 614
int32 Joylog::ReadAheadBufferPos = 0;     // Decl: 615
uint8 *Joylog::ReadAheadBuffer = nullptr; // Decl: 616

int Joylog::JuiceReplayFlag = 0; // Decl: 618

void Joylog::StopReplaying() {
    if (!ReplayingFlag) {
        return;
    }
    if (bIsCodeineConnected()) {
        // TODO magic
        bFunkCallASync("CODEINE", 0x68, nullptr, 0);
    }
    pReplayingBuffer = nullptr;
    ReplayingFlag = 0;
}

// STRIPPED: void Joylog::SetChecksumError()

// STRIPPED: void Joylog::PrintNearbyJoylogEntries(int error_pos)

// STRIPPED: int Joylog::GetPosition()

// STRIPPED: void Joylog::SetPosition(int position)

void Joylog::LoadReadAheadBuffer() {
    ReadAheadBuffer = static_cast<uint8 *>(bGetFile(pReplayingBuffer->GetFilename(), &ReadAheadBufferSize, 0x40));
}

int Joylog::ReadAheadFromChannel(void *buf, int size, int channel_number) {
    int num_read = 0;
    while (num_read < size && ReadAheadBufferPos < ReadAheadBufferSize) {
        JoylogBufferEntry entry;
        int size = JoylogBuffer::GetEntry(&entry, ReadAheadBuffer + ReadAheadBufferPos);
        ReadAheadBufferPos += size;
        if (entry.ChannelNumber == channel_number) {
            for (int byte_num = 0; byte_num < entry.DataSize; byte_num++) {
                static_cast<uint8 *>(buf)[num_read] = static_cast<uint8>(entry.Data >> (byte_num * 8));
                num_read++;
            }
        }
    }

    return num_read;
}

void Joylog::FreeReadAheadBuffer() {
    bFree(ReadAheadBuffer);
    ReadAheadBuffer = nullptr;
    ReadAheadBufferSize = 0;
}

uint32 Joylog::GetData(int data_size, JoylogChannel channel_number) {
    if (!IsReplaying()) {
        return 0;
    }
    uint32 data = pReplayingBuffer->GetData(data_size, channel_number);
    if (pReplayingBuffer->IsMoreData() == 0) {
        StopReplaying();
    }
    return data;
}

int32 Joylog::GetSignedData(int data_size, JoylogChannel channel_number) {
    return (static_cast<int32>(GetData(data_size, channel_number)) << (32 - data_size)) >> (32 - data_size);
}

void Joylog::GetData(void *data, int data_size_bytes, JoylogChannel channel_number) {
    for (int n = 0; n < data_size_bytes; n++) {
        static_cast<uint8 *>(data)[n] = static_cast<uint8>(GetData(8, channel_number));
    }
}

void Joylog::AddData(int32 data, int data_size_bits, JoylogChannel channel_number) {
    if (CapturingFlag) {
        pCapturingBuffer->AddData(data, data_size_bits, channel_number);
    }
}

void Joylog::AddData(const void *data, int data_size_bytes, JoylogChannel channel_number) {
    for (int n = 0; n < data_size_bytes; n++) {
        AddData(static_cast<int32>(static_cast<const char *>(data)[n]), 8, channel_number);
    }
}

uint32 Joylog::AddOrGetData(uint32 data, int data_size, JoylogChannel channel_number) {
    if (IsReplaying()) {
        data = GetData(data_size, channel_number);
    }
    if (IsCapturing()) {
        AddData(static_cast<int32>(data), data_size, channel_number);
    }
    return data;
}

// STRIPPED: int Joylog::AddOrGetSignedData(int32 data, int data_size, JoylogChannel channel_number)

float Joylog::AddOrGetData(float data, JoylogChannel channel_number) {
    if (IsReplaying()) {
        data = Joylog::GetData(channel_number);
    }
    if (IsCapturing()) {
        Joylog::AddData(data, channel_number);
    }
    return data;
}

void Joylog::AddOrGetData(char *string, JoylogChannel channel_number) {
    if (IsReplaying()) {
        int len = static_cast<int>(GetData(16, channel_number));
        int n;
        for (n = 0; n < len; n++) {
            string[n] = static_cast<char>(GetData(8, channel_number));
        }
        string[n] = 0;
    }
    if (IsCapturing()) {
        int len = bStrLen(string);
        AddData(len, 16, channel_number);
        for (int n = 0; n < len; n++) {
            AddData(static_cast<int>(string[n]), 8, channel_number);
        }
    }
}

void Joylog::AddOrGetData(uint16 *string, JoylogChannel channel_number) {
    if (IsReplaying()) {
        int len = static_cast<int>(GetData(16, channel_number));
        int n;
        for (n = 0; n < len; n++) {
            string[n] = static_cast<uint16>(GetData(16, channel_number));
        }
        string[n] = 0;
    }
    if (IsCapturing()) {
        int len = bStrLen(string);
        AddData(len, 16, channel_number);
        for (int n = 0; n < len; n++) {
            AddData(static_cast<int32>(string[n]), 16, channel_number);
        }
    }
}

// STRIPPED: void Joylog::VerifyData(int data, int data_size, JoylogChannel channel_number)

void Joylog::Init() {
    if (EnableJoylog) {
        char filename[260];
        bStrCpy(filename, "ReplayJoylog.jlg");
        if (filename[0] != '\0' && bFileExists(filename)) {
            bFile *f = bOpen(filename, 1, 1);
            int size = bFileSize(filename);
            JoylogBuffer *buffer = new ("JoylogBuffer (Replaying)", 0) JoylogBuffer(filename, size);
            pReplayingBuffer = buffer;
            ReplayingFlag = 1;
        } else {
            JoylogBuffer *buffer = new ("JoylogBuffer", 0) JoylogBuffer("CaptureJoylog.jlg", 0);
            pCapturingBuffer = buffer;
            CapturingFlag = 1;
            if (!bIsCodeineConnected()) {
                bFile *f = bOpen("CaptureJoylog.jlg", 6, 1);
                if (f != nullptr) {
                    bClose(f);
                } else {
                    CapturingFlag = 0;
                }
            }
        }
    }
}

void Joylog::Save() {
    // El DWARF da la local total_size y la expansion de GetTotalSize(); su uso
    // no llego al binario y no se conoce.
    int total_size;
    if (pCapturingBuffer != nullptr) {
        total_size = pCapturingBuffer->GetTotalSize();
        pCapturingBuffer->SaveBuffer();
    }
}

// STRIPPED: void Joylog::Suspend()

// STRIPPED: void Joylog::Resume()

uint32 Joylog::IsCapturing() {
    return CapturingFlag;
}

int Joylog::IsReplaying() {
    return ReplayingFlag;
}

// STRIPPED: void PrintInventory()

bool JoylogPutStringFunction(int terminal_channel, const char *s) {
    if (!bIsMainThread()) {
        return false;
    }
    int len = bStrLen(s);
    static int total_captured = 0;
    total_captured += len;
    if (len < 0x40000) {
        Joylog::AddData(s, len + 1, JOYLOG_CHANNEL_PRINTF);
    }
    return false;
}

static const int DisableDumpJoylogPrint = 0;

void DumpJoylogPrint() {
    bool last_line_missing_linefeed = false;
    Joylog::LoadReadAheadBuffer();
    int len = 0;
    char string[512];

    while (Joylog::ReadAheadFromChannel(string + len, 1, JOYLOG_CHANNEL_PRINTF) != 0) {
        if (len == 510) {
            len = 511;
            string[len] = 0;
        }

        if (string[len] != 0) {
            len++;
        } else {
            last_line_missing_linefeed = len > 0 && string[len - 1] != '\n';
            bPrintf("%s", string);
            len = 0;
        }
    }

    if (last_line_missing_linefeed) {
        bPrintf("\n");
    }

    Joylog::FreeReadAheadBuffer();
}

void WriteJoylogFileHeader() {
    char changelist_name[512];
    char build_version_name[64];
    bStrCpy(changelist_name, BuildVersionChangelistName);
    Joylog::AddOrGetData(changelist_name, JOYLOG_CHANNEL_CONFIG);
    if (Joylog::IsReplaying()) {
        bReleasePrintf("Joylog Changelist: %s\n", changelist_name);
    }
    bReleasePrintf("Changelist: %s\n", BuildVersionChangelistName);
    bStrCpy(build_version_name, bGetPlatformName());
#ifdef DEBUG
    bStrCat(build_version_name, build_version_name, " Debug");
#elif defined(MILESTONE_BUILD)
    bStrCat(build_version_name, build_version_name, " Milestone");
#else
    bStrCat(build_version_name, build_version_name, " Release");

#endif
    bStrCat(build_version_name, build_version_name, "Opt");
    if (Joylog::IsCapturing()) {
        Joylog::AddOrGetData(build_version_name, JOYLOG_CHANNEL_CONFIG);
    } else if (Joylog::IsReplaying()) {
        char joylog_build_version_name[64];
        Joylog::AddOrGetData(joylog_build_version_name, JOYLOG_CHANNEL_CONFIG);
        bReleasePrintf("Joylog Build Version: %s\n", joylog_build_version_name);
        if (!bStrEqual(joylog_build_version_name, build_version_name)) {
            bReleasePrintf("ERROR:  Joylog captured with %s, you\'re running %s\n", joylog_build_version_name, build_version_name);
            bBreak();
        }
    }
    bReleasePrintf("Build Type: %s\n", build_version_name);
    if (Joylog::IsReplaying()) {
        LoadConfigItems();
    }
    if (Joylog::IsCapturing()) {
        SaveConfigItems();
    }
}

void InitJoylog() {
    Joylog::Init();
    if (Joylog::IsCapturing()) {
        bSetUserPutStringFunction(JoylogPutStringFunction);
    } else if (Joylog::IsReplaying()) {
        DumpJoylogPrint();
    }
    WriteJoylogFileHeader();
}

void ServiceJoylog() {
    if (SaveJoylog) {
        Joylog::Save();
        SaveJoylog = 0;
        bRefreshTweaker();
    }
}

// STRIPPED: void EmergencySaveJoylog()
