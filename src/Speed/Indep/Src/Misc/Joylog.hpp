#ifndef MISC_JOYLOG_H
#define MISC_JOYLOG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

enum JoylogChannel {
    JOYLOG_CHANNEL_NONE = 0,
    JOYLOG_CHANNEL_CONFIG = 1,
    JOYLOG_CHANNEL_JOYEVENTS = 2,
    JOYLOG_CHANNEL_TIMESTEP = 3,
    JOYLOG_CHANNEL_CHECKSUM = 4,
    JOYLOG_CHANNEL_LOOP_COUNTER = 5,
    JOYLOG_CHANNEL_QUEUEDFILE_STATUS = 6,
    JOYLOG_CHANNEL_QUEUEDFILENAME = 7,
    JOYLOG_CHANNEL_MOVIE_PLAYER_STATUS = 8,
    JOYLOG_CHANNEL_MEMORY_CARD = 9,
    JOYLOG_CHANNEL_RANDOM = 10,
    JOYLOG_CHANNEL_PRINTF = 11,
    JOYLOG_CHANNEL_SOUND_LOADING = 12,
    JOYLOG_CHANNEL_PATHFINDER_TIMEOUT = 13,
    JOYLOG_NUM_CHANNELS = 14,
};

// total size: 0xC
struct JoylogBufferEntry {
    // Members
    int ChannelNumber; // offset 0x0, size 0x4
    int DataSize;      // offset 0x4, size 0x4
    uint32 Data;       // offset 0x8, size 0x4
};

// total size: 0x4118
struct JoylogBuffer {
    JoylogBuffer(const char *filename, int top_position);

    void SaveBuffer();

    void LoadBuffer(int position);

    void AddData(int data, int data_size, int channel_number);

    unsigned int GetData(int data_size, int channel_number);

    int AddEntry(JoylogBufferEntry *entry, int position);

    int GetEntry(JoylogBufferEntry *entry, int position);

    static int GetEntry(JoylogBufferEntry *entry, uint8 *pbuf);

    void PrintNearbyJoylogEntries(int error_pos);

    char *GetFilename() {}

    int GetTotalSize() {}

    int IsMoreData() {}

    int GetPosition() {}

    void SetPosition(int position) {}

    // Members
    int32 CurrentPosition;       // offset 0x0, size 0x4
    int32 NumBytesInBuffer;      // offset 0x4, size 0x4
    int32 CurrentLoadPosition;   // offset 0x8, size 0x4
    int32 BufferStartPosition;   // offset 0xC, size 0x4
    int32 TopPosition;           // offset 0x10, size 0x4
    char Filename[260];          // offset 0x14, size 0x104
    unsigned char Buffer[16384]; // offset 0x118, size 0x4000
};

// total size: 0x1
class Joylog {
  public:
    static void StopReplaying();

    static void SetChecksumError();

    static void PrintNearbyJoylogEntries(int error_pos);

    static int GetPosition();

    static void SetPosition(int position);

    static void LoadReadAheadBuffer();

    static int ReadAheadFromChannel(void *buf, int size, int channel_number);

    static void FreeReadAheadBuffer();

    static unsigned int GetData(int data_size, JoylogChannel channel_number);

    static int GetSignedData(int data_size, JoylogChannel channel_number);

    static void GetData(void *data, int data_size_bytes, JoylogChannel channel_number);

    static void AddData(int data, int data_size_bits, JoylogChannel channel_number);

    static void AddData(const void *data, int data_size_bytes, JoylogChannel channel_number);

    static unsigned int AddOrGetData(unsigned int data, int data_size, JoylogChannel channel_number);

    static int AddOrGetSignedData(int data, int data_size, JoylogChannel channel_number);

    static float AddOrGetData(float data, JoylogChannel channel_number);

    static void AddOrGetData(char *string, JoylogChannel channel_number);

    static void AddOrGetData(unsigned short *string, JoylogChannel channel_number);

    static void VerifyData(int data, int data_size, JoylogChannel channel_number);

    static void Init();

    static void Save();

    static void Suspend();

    static void Resume();

    static unsigned int IsCapturing();

    static int IsReplaying();

    static float GetData(JoylogChannel channel_number) {}

    static void AddData(float data, JoylogChannel channel_number) {}

    static void RewindReadAheadBuffer() {}

    static unsigned int IsJuiceReplay() {}

    static void SetJuiceReplay(int val) {}

  private:
    static int ReplayingFlag;              // size: 0x4
    static int CapturingFlag;              // size: 0x4
    static JoylogBuffer *pReplayingBuffer; // size: 0x4
    static JoylogBuffer *pCapturingBuffer; // size: 0x4
    static int32 ReadAheadBufferSize;      // size: 0x4
    static int32 ReadAheadBufferPos;       // size: 0x4
    static uint8 *ReadAheadBuffer;         // size: 0x4
    static int JuiceReplayFlag;            // size: 0x4
};

void InitJoylog();

#endif
