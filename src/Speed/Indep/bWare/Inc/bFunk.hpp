#ifndef BWARE_BFUNK_H
#define BWARE_BFUNK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "bList.hpp"

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

void bRefreshTweaker();
void bFunkCallASync(const char *server_name, int function_num, const void *data, int size);
int bFunkCallSync(const char *server_name, int function_num, const void *data, int size, void *return_data, int max_return_size);

#endif
