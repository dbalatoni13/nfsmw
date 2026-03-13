#include "Joylog.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "bFile.hpp"
#include <cstring>

#ifdef MILESTONE_OPT
int EnableJoylog = true;
#else
int EnableJoylog = false;
#endif
int SaveJoylog = false;
// static const int PrintJoylog;
// static const int JoylogScreenPrintX;
// static const int JoylogScreenPrintY;
int JoylogThrottleCounter[2];
int JoylogThrottleTicks[2];
int CurrentJoylogThrottleBuffer;
// static const int JoylogMaxThrottleCounter;
// static const float JoylogThrottleTime;
JoylogChannelInfo NFSJoylogChannelInfoTable[14];
// static const int DisableDumpJoylogPrint;

JoylogBuffer::JoylogBuffer(const char *filename, int top_position) {
    this->TopPosition = top_position;
    bStrCpy(this->Filename, filename);
    this->CurrentPosition = 0;
    this->NumBytesInBuffer = 0;
    this->CurrentLoadPosition = 0;
    this->BufferStartPosition = 0;
    bMemSet(this->Buffer, 0, sizeof(this->Buffer));
    if (bIsCodeineConnected()) {
        bFunkJoylogCaptureInitPacket packet;
        memset(&packet, 0, sizeof(packet));
        packet.BufferAddress = reinterpret_cast<intptr_t>(&this->Buffer);
        packet.BufferSizeAddress = reinterpret_cast<intptr_t>(&this->NumBytesInBuffer);

        bFunkCallASync("CODEINE", 100, &packet, 8);
    }
}

void JoylogBuffer::SaveBuffer() {
    ProfileNode profile_node("TODO", 0);

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
    ProfileNode profile_node("TODO", 0);

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

void JoylogBuffer::AddData(int data, int data_size, int channel_number) {
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
    this->CurrentPosition = this->TopPosition = AddEntry(&buffer_entry, this->CurrentPosition);
    this->NumBytesInBuffer = this->CurrentPosition - this->BufferStartPosition;
    int free_space = sizeof(this->Buffer) - this->NumBytesInBuffer;
    if (free_space < 256) {
        SaveBuffer();
    }
}

unsigned int JoylogBuffer::GetData(int data_size, int channel_number) {
    JoylogBufferEntry buffer_entry;
    int prev_position = CurrentPosition;

    do {
        if (CurrentLoadPosition - CurrentPosition < 0x100 && CurrentLoadPosition != TopPosition) {
            LoadBuffer(CurrentPosition);
        }
        CurrentPosition = GetEntry(&buffer_entry, CurrentPosition);
    } while (GetJoylogChannelInfo(buffer_entry.ChannelNumber)->ReadAheadOnly != 0);

    int lookahead_pos = CurrentPosition;
    while (IsMoreData()) {
        if (CurrentLoadPosition - CurrentPosition < 0x100 && CurrentLoadPosition != TopPosition) {
            LoadBuffer(CurrentPosition);
        }
        JoylogBufferEntry lookahead_buffer_entry;
        lookahead_pos = GetEntry(&lookahead_buffer_entry, lookahead_pos);
        if (GetJoylogChannelInfo(lookahead_buffer_entry.ChannelNumber)->ReadAheadOnly == 0) {
            break;
        }
        CurrentPosition = lookahead_pos;
    }

    if (buffer_entry.ChannelNumber != channel_number) {
        int percent_complete = CurrentPosition * 100 / GetTotalSize();
        PrintNearbyJoylogEntries(prev_position);
        bBreak();
        CurrentPosition = TopPosition;
    }

    return buffer_entry.Data;
}

int JoylogBuffer::AddEntry(JoylogBufferEntry *entry, int position) {
    int buffer_index = (position - BufferStartPosition) + 0x118;
    unsigned char *pbuf = reinterpret_cast<unsigned char *>(this) + buffer_index;
    pbuf[0] = static_cast<unsigned char>(entry->ChannelNumber + entry->DataSize * 16);
    unsigned int data = entry->Data;
    for (int byte_num = 0; byte_num < entry->DataSize; byte_num++) {
        pbuf[1 + byte_num] = static_cast<unsigned char>(data);
        data >>= 8;
    }
    return position + entry->DataSize + 1;
}

int JoylogBuffer::GetEntry(JoylogBufferEntry *entry, int position) {
    int buffer_index = (position - BufferStartPosition) + 0x118;
    unsigned char *pbuf = reinterpret_cast<unsigned char *>(this) + buffer_index;
    return position + GetEntry(entry, pbuf);
}

int JoylogBuffer::GetEntry(JoylogBufferEntry *entry, uint8 *pbuf) {
    entry->ChannelNumber = pbuf[0] & 0xF;
    entry->DataSize = pbuf[0] >> 4;
    unsigned int data = 0;
    for (int byte_num = 0; byte_num < entry->DataSize; byte_num++) {
        data |= static_cast<unsigned int>(pbuf[1 + byte_num]) << (byte_num * 8);
    }
    entry->Data = data;
    return entry->DataSize + 1;
}

void JoylogBuffer::PrintNearbyJoylogEntries(int error_pos) {
    const int range = 40;
    int pos = BufferStartPosition;
    if (pos < TopPosition && pos < CurrentPosition + range) {
        while (CurrentLoadPosition == 0 || pos < CurrentLoadPosition) {
            JoylogBufferEntry buffer_entry;
            int new_pos = GetEntry(&buffer_entry, pos);
            if (TopPosition <= new_pos) {
                return;
            }
            if (CurrentPosition + range <= new_pos) {
                return;
            }
            pos = new_pos;
        }
    }
}

int Joylog::ReplayingFlag;
int Joylog::CapturingFlag;
JoylogBuffer *Joylog::pReplayingBuffer;
JoylogBuffer *Joylog::pCapturingBuffer;
int32 Joylog::ReadAheadBufferSize;
int32 Joylog::ReadAheadBufferPos;
uint8 *Joylog::ReadAheadBuffer;
int Joylog::JuiceReplayFlag;

void Joylog::StopReplaying() {
    if (ReplayingFlag == 0) {
        return;
    }
    pReplayingBuffer = nullptr;
    ReplayingFlag = 0;
}

void Joylog::LoadReadAheadBuffer() {
    ReadAheadBuffer = static_cast<uint8 *>(bGetFile(pReplayingBuffer->GetFilename(), &ReadAheadBufferSize, 0x40));
}

int Joylog::ReadAheadFromChannel(void *buf, int size, int channel_number) {
    int num_read = 0;
    if (size > 0 && ReadAheadBufferPos < ReadAheadBufferSize) {
        do {
            JoylogBufferEntry entry;
            int entry_size = JoylogBuffer::GetEntry(&entry, ReadAheadBuffer + ReadAheadBufferPos);
            ReadAheadBufferPos += entry_size;
            if (entry.ChannelNumber == channel_number) {
                for (int byte_num = 0; byte_num < entry.DataSize; byte_num++) {
                    static_cast<unsigned char *>(buf)[num_read] =
                        static_cast<unsigned char>(entry.Data >> ((byte_num & 7) * 8));
                    num_read++;
                }
            }
        } while (num_read < size && ReadAheadBufferPos < ReadAheadBufferSize);
    }
    return num_read;
}

void Joylog::FreeReadAheadBuffer() {
    bFree(ReadAheadBuffer);
    ReadAheadBuffer = nullptr;
    ReadAheadBufferSize = 0;
}

unsigned int Joylog::GetData(int data_size, JoylogChannel channel_number) {
    if (IsReplaying() == 0) {
        return 0;
    }
    unsigned int data = pReplayingBuffer->GetData(data_size, channel_number);
    if (pReplayingBuffer->IsMoreData() == 0) {
        StopReplaying();
    }
    return data;
}

int Joylog::GetSignedData(int data_size, JoylogChannel channel_number) {
    int data = static_cast<int>(GetData(data_size, channel_number));
    return (data << (32 - data_size)) >> (32 - data_size);
}

void Joylog::GetData(void *data, int data_size_bytes, JoylogChannel channel_number) {
    for (int n = 0; n < data_size_bytes; n++) {
        static_cast<unsigned char *>(data)[n] = static_cast<unsigned char>(GetData(8, channel_number));
    }
}

void Joylog::AddData(int data, int data_size_bits, JoylogChannel channel_number) {
    if (CapturingFlag != 0) {
        pCapturingBuffer->AddData(data, data_size_bits, channel_number);
    }
}

void Joylog::AddData(const void *data, int data_size_bytes, JoylogChannel channel_number) {
    for (int n = 0; n < data_size_bytes; n++) {
        AddData(static_cast<int>(static_cast<const unsigned char *>(data)[n]), 8, channel_number);
    }
}

unsigned int Joylog::AddOrGetData(unsigned int data, int data_size, JoylogChannel channel_number) {
    if (IsReplaying() != 0) {
        data = GetData(data_size, channel_number);
    }
    if (IsCapturing() != 0) {
        AddData(static_cast<int>(data), data_size, channel_number);
    }
    return data;
}

float Joylog::AddOrGetData(float data, JoylogChannel channel_number) {
    if (IsReplaying() != 0) {
        data = Joylog::GetData(channel_number);
    }
    if (IsCapturing() != 0) {
        Joylog::AddData(data, channel_number);
    }
    return data;
}

void Joylog::AddOrGetData(char *string, JoylogChannel channel_number) {
    if (IsReplaying() != 0) {
        int len = static_cast<int>(GetData(16, channel_number));
        int n = 0;
        for (; n < len; n++) {
            string[n] = static_cast<char>(GetData(8, channel_number));
        }
        string[n] = 0;
    }
    if (IsCapturing() != 0) {
        int len = bStrLen(string);
        AddData(len, 16, channel_number);
        for (int n = 0; n < len; n++) {
            AddData(static_cast<int>(string[n]), 8, channel_number);
        }
    }
}

void Joylog::AddOrGetData(unsigned short *string, JoylogChannel channel_number) {
    if (IsReplaying() != 0) {
        int len = static_cast<int>(GetData(16, channel_number));
        int n = 0;
        for (; n < len; n++) {
            string[n] = static_cast<unsigned short>(GetData(16, channel_number));
        }
        string[n] = 0;
    }
    if (IsCapturing() != 0) {
        unsigned int len = bStrLen(string);
        AddData(static_cast<int>(len), 16, channel_number);
        for (int n = 0; n < static_cast<int>(len); n++) {
            AddData(static_cast<int>(string[n]), 16, channel_number);
        }
    }
}

unsigned int Joylog::IsCapturing() {
    return CapturingFlag;
}

int Joylog::IsReplaying() {
    return ReplayingFlag;
}

static int total_captured;

int JoylogPutStringFunction(int terminal_channel, const char *s) {
    if (bIsMainThread()) {
        int len = bStrLen(s);
        total_captured += len;
        if (len < 0x40000) {
            Joylog::AddData(s, len + 1, JOYLOG_CHANNEL_PRINTF);
        }
    }
    return 0;
}

void DumpJoylogPrint() {
    bool last_line_missing_linefeed;
    char string[512];
    Joylog::LoadReadAheadBuffer();
    int len = 0;
    while (Joylog::ReadAheadFromChannel(string + len, 1, JOYLOG_CHANNEL_PRINTF) != 0) {
        if (len == 510) {
            len = 511;
            string[511] = 0;
        }
        if (string[len] == 0) {
            len = 0;
        } else {
            len++;
        }
    }
    Joylog::FreeReadAheadBuffer();
}

extern const char *BuildVersionChangelistName;
extern char *bGetPlatformName();
extern int bStrICmp(const char *s1, const char *s2);
void bSetUserPutStringFunction(int (*func)(int, const char *));
void LoadConfigItems();
void SaveConfigItems();

void WriteJoylogFileHeader() {
    char changelist_name[512];
    char build_version_name[64];
    bStrCpy(changelist_name, BuildVersionChangelistName);
    Joylog::AddOrGetData(changelist_name, JOYLOG_CHANNEL_CONFIG);
    if (Joylog::IsReplaying()) {
        bReleasePrintf("Joylog CL: %s\n", changelist_name);
    }
    bReleasePrintf("Build CL: %s\n", BuildVersionChangelistName);
    bStrCpy(build_version_name, "NGC");
    bStrCat(build_version_name, build_version_name, "_RELEASE");
    bStrCat(build_version_name, build_version_name, "_USA");
    if (Joylog::IsCapturing()) {
        Joylog::AddOrGetData(build_version_name, JOYLOG_CHANNEL_CONFIG);
    } else if (Joylog::IsReplaying()) {
        char joylog_build_version_name[64];
        Joylog::AddOrGetData(joylog_build_version_name, JOYLOG_CHANNEL_CONFIG);
        bReleasePrintf("Joylog build: %s\n", joylog_build_version_name);
        if (bStrICmp(joylog_build_version_name, build_version_name) != 0) {
            bReleasePrintf("WARNING: Different builds %s %s\n", joylog_build_version_name, build_version_name);
            bBreak();
        }
    }
    bReleasePrintf("Current build: %s\n", build_version_name);
    if (Joylog::IsReplaying()) {
        LoadConfigItems();
    }
    if (Joylog::IsCapturing()) {
        SaveConfigItems();
    }
}

void Joylog::Init() {
    if (EnableJoylog != 0) {
        char filename[260];
        bStrCpy(filename, "joylog.bin");
        if (filename[0] != 0 && bFileExists(filename)) {
            bOpen(filename, 1, 1);
            int file_size = bFileSize(filename);
            pReplayingBuffer = new JoylogBuffer(filename, file_size);
            ReplayingFlag = 1;
            return;
        }
        pCapturingBuffer = new JoylogBuffer("joylog.bin", 0);
        CapturingFlag = 1;
        bFile *f = bOpen("joylog.bin", 6, 1);
        if (f == nullptr) {
            CapturingFlag = 0;
        } else {
            bClose(f);
        }
    }
}

void Joylog::Save() {
    if (pCapturingBuffer != nullptr) {
        pCapturingBuffer->SaveBuffer();
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

extern void bRefreshTweaker();

void ServiceJoylog() {
    if (SaveJoylog != 0) {
        Joylog::Save();
        SaveJoylog = 0;
        bRefreshTweaker();
    }
}
