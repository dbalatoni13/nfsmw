#include "Joylog.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "bFile.hpp"
#include <cstring>

#ifdef DEBUG
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
