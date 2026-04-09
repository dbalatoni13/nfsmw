#ifndef BWARE_BFUNK_H
#define BWARE_BFUNK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "bList.hpp"
#include <types.h>

struct bFunkPacketHeader {
    // total size: 0x1C
    uint16 TotalSize;           // offset 0x0, size 0x2
    uint8 Type;                 // offset 0x2, size 0x1
    uint8 FunctionNum;          // offset 0x3, size 0x1
    uint32 DestServer;          // offset 0x4, size 0x4
    uint32 SourceServer;        // offset 0x8, size 0x4
    uint16 PacketID;            // offset 0xC, size 0x2
    uint16 Checksum;            // offset 0xE, size 0x2
    uint32 ReturnBufferAddress; // offset 0x10, size 0x4
    uint32 ReturnCodeAddress;   // offset 0x14, size 0x4
    int16 ReturnCode;           // offset 0x18, size 0x2
    int16 MaxReturnSize;        // offset 0x1A, size 0x2

    int GetType() {}

    int GetTotalSize() {}

    int GetDataSize() {}

    uint8 *GetData() {}

    uint8 *GetData(int pos) {}

    void SetDataSize(int size) {}

    void SetTotalSize(int size) {}

    void InitReturnPacketHeader(const bFunkPacketHeader *sync_packet, int return_code) {}

    void SetChecksumParameters(int packet_id, uint16 checksum) {}

    uint16 CalculateChecksum() {}

    static uint16 CalculateChecksum(void *data, int data_size);
};

// total size: 0x7FC
struct bFunkPacket : public bFunkPacketHeader {
    uint8 Data[2016]; // offset 0x1C, size 0x7E0
};

typedef void bFunkHandleASyncFunction(const void *, int32);
typedef int bFunkHandleSyncFunction(const void *, int32, void *);

class bFunkServer : public bTNode<bFunkServer> {
    // total size: 0x2D0
    uint32 NameHash;          // offset 0x8, size 0x4
    char Name[64];            // offset 0xC, size 0x40
    char FunctionTypes[128];  // offset 0x4C, size 0x80
    void *FunctionTable[128]; // offset 0xCC, size 0x200

  public:
    bFunkServer(const char *name) {}

    uint32 GetNameHash() {
        return NameHash;
    }

    const char *GetName() {
        return Name;
    }

    void AddASync(int function_num, bFunkHandleASyncFunction) {}

    void AddSync(int function_num, bFunkHandleSyncFunction) {}

    void Remove(int function_num);

    void ProcessPacket(bFunkPacket *packet);

    virtual ~bFunkServer();
    virtual bool CanDeliverPacket(uint32 server_hash);
    virtual bool DeliverPacket(bFunkPacket *packet);
    virtual int Service() {}
};

// total size: 0x2E0
class bFunkServerPlatform : public bFunkServer {
    // Members
    int MaxReceivePackets;        // offset 0x2D0, size 0x4
    int NextReceivePacketNum;     // offset 0x2D4, size 0x4
    bool ProcessingPacket;        // offset 0x2D8, size 0x1
    bFunkPacket *pReceivePackets; // offset 0x2DC, size 0x4
  public:
    bFunkServerPlatform();
    override ~bFunkServerPlatform();
    override bool DeliverPacket(bFunkPacket *packet);
    override int Service();
};

struct bFileOpenPacket {
    int8 OpenFlags;            // offset 0x0, size 0x1
    int8 Obsolete1;            // offset 0x1, size 0x1
    int8 WarnIfCantOpen;       // offset 0x2, size 0x1
    int8 FutureExpansion;      // offset 0x3, size 0x1
    int32 FileID;              // offset 0x4, size 0x4
    int32 FilePositionAddress; // offset 0x8, size 0x4
    int32 CachedFileSize;      // offset 0xC, size 0x4
    int32 Obsolete2;           // offset 0x10, size 0x4
    char Filename[200];        // offset 0x14, size 0xC8
};

struct bCodeineDirEntry {
    uint32 NameHash; // offset 0x0, size 0x4
    int32 FileSize;  // offset 0x4, size 0x4
};

struct bFileCacheDirsPacket {
    int32 DirEntryAddress; // offset 0x0, size 0x4
    int32 MaxEntries;      // offset 0x4, size 0x4
    int32 DirNamesSize;    // offset 0x8, size 0x4
    char DirNames[1024];   // offset 0xC, size 0x400
};

struct bFileReadPacket {
    int32 FileID;   // offset 0x0, size 0x4
    int32 Position; // offset 0x4, size 0x4
    int32 Address;  // offset 0x8, size 0x4
    int32 NumBytes; // offset 0xC, size 0x4
};

struct bFileWritePacket {
    int32 FileID;     // offset 0x0, size 0x4
    int32 Position;   // offset 0x4, size 0x4
    uint8 Data[2008]; // offset 0x8, size 0x7D8
};

// total size: 0x4
struct bFileClosePacket {
    int32 FileID; // offset 0x0, size 0x4
};

// total size: 0x88
struct bFunkCodeineAddServerPacket {
    struct bFunkPacket *pReceivePackets; // offset 0x0, size 0x4
    int MaxReceivePackets;               // offset 0x4, size 0x4
    char Name[128];                      // offset 0x8, size 0x80
};

// total size: 0xC
struct bMemoryTraceRemovePacket {
    int PoolID;        // offset 0x0, size 0x4
    int MemoryAddress; // offset 0x4, size 0x4
    int Size;          // offset 0x8, size 0x4
};

// total size: 0x8
struct bFunkJoylogCaptureInitPacket {
    intptr_t BufferAddress;     // offset 0x0, size 0x4
    intptr_t BufferSizeAddress; // offset 0x4, size 0x4
};

// total size: 0x58
struct bSymbolInfoPacket {
    int BaseType;      // offset 0x0, size 0x4
    int Size;          // offset 0x4, size 0x4
    int Address;       // offset 0x8, size 0x4
    char IsPointer;    // offset 0xC, size 0x1
    char IsEnum;       // offset 0xD, size 0x1
    char EnumName[64]; // offset 0xE, size 0x40
    union {
        long IntValue;     // offset 0x0, size 0x4
        double FloatValue; // offset 0x0, size 0x8
    } _value;              // offset 0x50, size 0x8
};

// total size: 0x48
struct bEnumInfoPacket {
    char EnumName[64];      // offset 0x0, size 0x40
    int TotalNumValues;     // offset 0x40, size 0x4
    int NumRetrievedValues; // offset 0x44, size 0x4
};

// total size: 0x44
struct bEnumValuePacket {
    char Name[64]; // offset 0x0, size 0x40
    int Value;     // offset 0x40, size 0x4
};

// total size: 0x7B8
struct bEnumRecordPacket {
    bEnumInfoPacket EnumInfo;        // offset 0x0, size 0x48
    bEnumValuePacket EnumValues[28]; // offset 0x48, size 0x770
};

void bRefreshTweaker();
void bFunkCallASync(const char *server_name, int function_num, const void *data, int size);
int bFunkCallSync(const char *server_name, int function_num, const void *data, int size, void *return_data, int max_return_size);

#endif
