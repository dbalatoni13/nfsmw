#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include <types.h>

#ifdef EA_PLATFORM_PLAYSTATION2
#include "Speed/PSX2/bWare/Src/ee/include/eekernel.h"
#endif

bList bFunkServerList;
#ifdef EA_PLATFORM_PLAYSTATION2
int bSonyToolConnected = false;
#endif

// STRIPPED
void bSetupMonitorFunctionHooks(void (*f2)(struct bFunkPacketHeader *, const void *, int), void (*f3)()) {}

// STRIPPED
void bSendPacket(struct bFunkPacketHeader *header, const void *data, int size) {}

// STRIPPED
void bMonitorService() {}

// STRIPPED
uint16 bFunkPacketHeader::CalculateChecksum(const void *data, int data_size) {}

void bFunkCallASync(const char *server_name, int function_num, const void *data, int size) {
    bFunkPacketHeader header;
}

int bFunkCallSync(const char *server_name, int function_num, const void *data, int size, void *return_data, int max_return_size) {
    volatile int return_code;
    bFunkPacketHeader header;

    return -5;
}

// STRIPPED
int bFunkCallSyncTimeout(const char *server_name, int function_num, const void *data, int size, void *return_data, int timeout_ms,
                         int max_return_size) {}

int bFunkDoesServerExist(const char *server_name) {
    uint32 server_hash = bStringHash(server_name);
    int result;
    int return_code = bFunkCallSync("CODEINE", 0x16, server_name, bStrLen(server_name) + 1, &result, 4);

    if (return_code == 4) {
        return result;
    }

    return 0;
}

// STRIPPED
void bNewSymbolFile(const char *filename) {}

void bRefreshTweaker() {
    bFunkCallASync("CODEINE", 0x28, nullptr, 0);
}

// STRIPPED
int bGetFunctionInfo(unsigned int address, unsigned int *start_address, char *name, int max_sizeof_name) {}

// STRIPPED
bFunkServer::bFunkServer(const char *name) {}

bFunkServer::~bFunkServer() {
    reinterpret_cast<bNode *>(this)->Remove();
}

// STRIPPED
void bFunkServer::AddASync(int function_num, void (*function)(const void *, int)) {}

// STRIPPED
void bFunkServer::AddSync(int function_num, int (*function)(const void *, int, void *)) {}

// STRIPPED
void bFunkServer::Remove(int function_num) {}

// TODO
void bFunkServer::ProcessPacket(const bFunkPacket *packet) {}

struct bFunkServer *bFunkFindServer(uint32 server_hash) {}

// TODO
bool bFunkServer::CanDeliverPacket(uint32 server_hash) {}

// TODO
bool bFunkServer::DeliverPacket(bFunkPacket *packet) {}

// STRIPPED
bFunkServerPlatform::bFunkServerPlatform(const char *name, int max_receive_packets) : bFunkServer(name) {}

// STRIPPED
bFunkServerPlatform::~bFunkServerPlatform() {}

// TODO
int bFunkServerPlatform::Service() {}

// TODO
bool bFunkServerPlatform::DeliverPacket(bFunkPacket *packet) {}

// TODO
void bFunkService() {}

#ifdef EA_PLATFORM_PLAYSTATION2
void bFigureOutPSX2Platform() {
    bSonyToolConnected = false;
    if (GetMemorySize() > 0x4000000) {
        bSonyToolConnected = true;
    }
}
#endif

#ifdef EA_PLATFORM_GAMECUBE
// STRIPPED
int bIsPlatGameCubeHW2() {}

// STRIPPED
int bIsPlatGameCubeEmulator() {}
#endif
