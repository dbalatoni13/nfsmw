#ifndef BWARE_BFUNK_H
#define BWARE_BFUNK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "bList.hpp"
#include <types.h>

struct bFunkPacketHeader {
    // total size: 0x1C
    unsigned short TotalSize;         // offset 0x0, size 0x2
    unsigned char Type;               // offset 0x2, size 0x1
    unsigned char FunctionNum;        // offset 0x3, size 0x1
    unsigned int DestServer;          // offset 0x4, size 0x4
    unsigned int SourceServer;        // offset 0x8, size 0x4
    unsigned short PacketID;          // offset 0xC, size 0x2
    unsigned short Checksum;          // offset 0xE, size 0x2
    unsigned int ReturnBufferAddress; // offset 0x10, size 0x4
    unsigned int ReturnCodeAddress;   // offset 0x14, size 0x4
    short ReturnCode;                 // offset 0x18, size 0x2
    short MaxReturnSize;              // offset 0x1A, size 0x2

    int GetType() {}

    int GetTotalSize() {}

    int GetDataSize() {}

    unsigned char *GetData() {}

    unsigned char *GetData(int pos) {}

    void SetDataSize(int size) {}

    void SetTotalSize(int size) {}

    void InitReturnPacketHeader(const bFunkPacketHeader *sync_packet, int return_code) {}

    void SetChecksumParameters(int packet_id, unsigned short checksum) {}

    unsigned short CalculateChecksum() {}
};

// total size: 0x7FC
struct bFunkPacket : public bFunkPacketHeader {
    unsigned char Data[2016]; // offset 0x1C, size 0x7E0
};

struct bFunkServer : public bTNode<bFunkServer> {
    // total size: 0x2D0
    unsigned int NameHash;    // offset 0x8, size 0x4
    char Name[64];            // offset 0xC, size 0x40
    char FunctionTypes[128];  // offset 0x4C, size 0x80
    void *FunctionTable[128]; // offset 0xCC, size 0x200

    unsigned int GetNameHash() {}

    const char *GetName() {}

    void AddASync(int function_num, void (*function)(const void *, long)) {}

    void AddSync(int function_num, long (*function)(const void *, long, void *)) {}

    virtual int Service() {}
};

// total size: 0x2E0
struct bFunkServerPlatform : public bFunkServer {
    // Members
    int MaxReceivePackets;        // offset 0x2D0, size 0x4
    int NextReceivePacketNum;     // offset 0x2D4, size 0x4
    bool ProcessingPacket;        // offset 0x2D8, size 0x1
    bFunkPacket *pReceivePackets; // offset 0x2DC, size 0x4
};

struct bFileOpenPacket {
    char OpenFlags;          // offset 0x0, size 0x1
    char Obsolete1;          // offset 0x1, size 0x1
    char WarnIfCantOpen;     // offset 0x2, size 0x1
    char FutureExpansion;    // offset 0x3, size 0x1
    int FileID;              // offset 0x4, size 0x4
    int FilePositionAddress; // offset 0x8, size 0x4
    int CachedFileSize;      // offset 0xC, size 0x4
    int Obsolete2;           // offset 0x10, size 0x4
    char Filename[200];      // offset 0x14, size 0xC8
};

struct bCodeineDirEntry {
    unsigned int NameHash; // offset 0x0, size 0x4
    int FileSize;          // offset 0x4, size 0x4
};

struct bFileCacheDirsPacket {
    int DirEntryAddress; // offset 0x0, size 0x4
    int MaxEntries;      // offset 0x4, size 0x4
    int DirNamesSize;    // offset 0x8, size 0x4
    char DirNames[1024]; // offset 0xC, size 0x400
};

struct bFileReadPacket {
    int FileID;   // offset 0x0, size 0x4
    int Position; // offset 0x4, size 0x4
    int Address;  // offset 0x8, size 0x4
    int NumBytes; // offset 0xC, size 0x4
};

struct bFileWritePacket {
    int FileID;               // offset 0x0, size 0x4
    int Position;             // offset 0x4, size 0x4
    unsigned char Data[2008]; // offset 0x8, size 0x7D8
};

// total size: 0x4
struct bFileClosePacket {
    int FileID; // offset 0x0, size 0x4
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
