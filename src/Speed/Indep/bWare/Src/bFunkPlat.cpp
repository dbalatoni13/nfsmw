#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <types.h>

#ifdef EA_PLATFORM_PLAYSTATION2
#include "Speed/PSX2/bWare/Src/ee/include/eekernel.h"
#elif defined(EA_PLATFORM_GAMECUBE)
#include <dolphin.h>
#endif

bList bFunkServerList;
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
int bSonyToolConnected = false;
void (*SendPacketFunction)(bFunkPacketHeader *, const void *, int) = nullptr;
void (*ServiceMonitorFunction)() = nullptr;
#endif

void bSetupMonitorFunctionHooks(void (*f2)(struct bFunkPacketHeader *, const void *, int), void (*f3)()) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    SendPacketFunction = f2;
    ServiceMonitorFunction = f3;
#endif
}

void bSendPacket(struct bFunkPacketHeader *header, const void *data, int size) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    if (SendPacketFunction != nullptr) {
        SendPacketFunction(header, data, size);
    }
#endif
}

void bMonitorService() {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    if (ServiceMonitorFunction != nullptr) {
        ServiceMonitorFunction();
    }
#endif
}

uint16 bFunkPacketHeader::CalculateChecksum(const void *data, int data_size) {
    const uint8 *bytes = static_cast<const uint8 *>(data);
    uint16 checksum = 0;

    for (int i = 0; i < data_size; ++i) {
        checksum = static_cast<uint16>(checksum + bytes[i]);
    }

    return checksum;
}

void bFunkPacketHeader::InitReturnPacketHeader(const bFunkPacketHeader *sync_packet, int return_code) {
    this->Type = 2;
    this->FunctionNum = sync_packet->FunctionNum;
    this->DestServer = sync_packet->SourceServer;
    this->SourceServer = sync_packet->DestServer;
    this->ReturnBufferAddress = sync_packet->ReturnBufferAddress;
    this->ReturnCodeAddress = sync_packet->ReturnCodeAddress;
    this->ReturnCode = static_cast<int16>(return_code);
    this->MaxReturnSize = sync_packet->MaxReturnSize;
    if (return_code < 0) {
        this->TotalSize = sizeof(bFunkPacketHeader);
    } else {
        this->TotalSize = static_cast<uint16>(sizeof(bFunkPacketHeader) + return_code);
    }
}

void bFunkCallASync(const char *server_name, int function_num, const void *data, int size) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    if (!bIsCodeineConnected() || size > static_cast<int>(sizeof(((bFunkPacket *)0)->Data))) {
        return;
    }

    bFunkPacketHeader header;
    bMemSet(&header, 0, sizeof(header));
    header.SetDataSize(size);
    header.Type = 0;
    header.FunctionNum = static_cast<uint8>(function_num);
    header.DestServer = bStringHash(server_name);
    bMonitorService();
    bSendPacket(&header, data, size);
#endif
}

int bFunkCallSync(const char *server_name, int function_num, const void *data, int size, void *return_data, int max_return_size) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    if (!bIsCodeineConnected()) {
        return -5;
    }
    if (size > static_cast<int>(sizeof(((bFunkPacket *)0)->Data)) ||
        max_return_size > static_cast<int>(sizeof(((bFunkPacket *)0)->Data))) {
        return -6;
    }

    volatile int return_code;
    bFunkPacketHeader header;
    return_code = -10;
    bMemSet(&header, 0, sizeof(header));
    header.SetDataSize(size);
    header.Type = 1;
    header.FunctionNum = static_cast<uint8>(function_num);
    header.DestServer = bStringHash(server_name);
    header.ReturnBufferAddress = reinterpret_cast<uint32>(return_data);
    header.ReturnCodeAddress = reinterpret_cast<uint32>(&return_code);
    header.MaxReturnSize = static_cast<int16>(max_return_size);
    bSendPacket(&header, data, size);

    while (return_code == -10) {
        bMonitorService();
    }

    return return_code;
#else

    return -5;
#endif
}

// STRIPPED
int bFunkCallSyncTimeout(const char *server_name, int function_num, const void *data, int size, void *return_data, int timeout_ms,
                         int max_return_size) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    if (!bIsBFunkAvailable()) {
        return -5;
    }
    if (size > static_cast<int>(sizeof(((bFunkPacket *)0)->Data)) ||
        max_return_size > static_cast<int>(sizeof(((bFunkPacket *)0)->Data))) {
        return -6;
    }

    volatile int return_code = -10;
    bFunkPacketHeader header;
    bMemSet(&header, 0, sizeof(header));
    header.SetDataSize(size);
    header.Type = 1;
    header.FunctionNum = static_cast<uint8>(function_num);
    header.DestServer = bStringHash(server_name);
    header.ReturnBufferAddress = reinterpret_cast<uint32>(return_data);
    header.ReturnCodeAddress = reinterpret_cast<uint32>(&return_code);
    header.MaxReturnSize = static_cast<int16>(max_return_size);
    bSendPacket(&header, data, size);

    unsigned int start_tick = bGetTicker();
    while (return_code == -10) {
        bMonitorService();
        if (bGetTickerDifference(start_tick) >= static_cast<float>(timeout_ms)) {
            return -7;
        }
    }
    return return_code;
#else
    return -5;
#endif
}

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
void bNewSymbolFile(const char *filename) {
    bFunkCallASync("CODEINE", 0x26, filename, bStrLen(filename) + 1);
}

void bRefreshTweaker() {
    bFunkCallASync("CODEINE", 0x28, nullptr, 0);
}

// STRIPPED
int bGetFunctionInfo(unsigned int address, unsigned int *start_address, char *name, int max_sizeof_name) {
    struct FunctionInfoPacket {
        unsigned int StartAddress;
        char Name[128];
    } packet;

    int return_code = bFunkCallSync("CODEINE", 0x27, &address, sizeof(address), &packet, sizeof(packet));
    if (return_code != sizeof(packet)) {
        return false;
    }
    *start_address = packet.StartAddress;
    bStrNCpy(name, packet.Name, max_sizeof_name - 1);
    name[max_sizeof_name - 1] = '\0';
    return true;
}

bFunkServer::bFunkServer(const char *name) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    this->NameHash = bStringHash(name);
    bMemSet(this->Name, 0, sizeof(this->Name));
    bStrNCpy(this->Name, name, sizeof(this->Name) - 1);
    bMemSet(this->FunctionTypes, 0, sizeof(this->FunctionTypes));
    bMemSet(this->FunctionTable, 0, sizeof(this->FunctionTable));
    bFunkServerList.AddTail(this);
#endif
}

bFunkServer::~bFunkServer() {
    reinterpret_cast<bNode *>(this)->Remove();
}

void bFunkServer::AddASync(int function_num, void (*function)(const void *, int)) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    if (function_num >= 0 && function_num < 128) {
        this->FunctionTypes[function_num] = 0;
        this->FunctionTable[function_num] = (void *)function;
    }
#endif
}

void bFunkServer::AddSync(int function_num, int (*function)(const void *, int, void *)) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    if (function_num >= 0 && function_num < 128) {
        this->FunctionTypes[function_num] = 1;
        this->FunctionTable[function_num] = (void *)function;
    }
#endif
}

void bFunkServer::Remove(int function_num) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    if (function_num >= 0 && function_num < 128) {
        this->FunctionTable[function_num] = nullptr;
    }
#endif
}

void bFunkServer::ProcessPacket(const bFunkPacket *packet) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    if (packet->GetType() == 0) {
        int function_num = packet->FunctionNum;
        if ((this->FunctionTypes[function_num] == 0) && (this->FunctionTable[function_num] != nullptr)) {
            ((bFunkHandleASyncFunction *)this->FunctionTable[function_num])(packet->GetData(), packet->GetDataSize());
        } else if ((this->FunctionTypes[function_num] == 1) && (this->FunctionTable[function_num] != nullptr)) {
            ((bFunkHandleSyncFunction *)this->FunctionTable[function_num])(packet->GetData(), packet->GetDataSize(), nullptr);
        }
    } else if (packet->GetType() == 1) {
        bFunkPacket return_packet;
        int return_code;
        void *function = this->FunctionTable[packet->FunctionNum];

        if (function != nullptr) {
            if (this->FunctionTypes[packet->FunctionNum] == 0) {
                ((bFunkHandleASyncFunction *)function)(packet->GetData(), packet->GetDataSize());
                return_code = 0;
            } else {
                return_code = ((bFunkHandleSyncFunction *)function)(packet->GetData(), packet->GetDataSize(),
                                                                   return_packet.GetData());
            }
        } else {
            return_code = -4;
        }

        return_packet.InitReturnPacketHeader(packet, return_code);
        this->DeliverPacket(&return_packet);
    } else if (packet->GetType() == 2) {
        if (packet->ReturnCode > 0) {
            bMemCpy(reinterpret_cast<void *>(packet->ReturnBufferAddress), packet->GetData(), packet->ReturnCode);
        }
        *reinterpret_cast<int *>(packet->ReturnCodeAddress) = packet->ReturnCode;
    }
#endif
}

struct bFunkServer *bFunkFindServer(uint32 server_hash) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    for (bFunkServer *server = reinterpret_cast<bFunkServer *>(bFunkServerList.GetHead());
         server != reinterpret_cast<bFunkServer *>(bFunkServerList.EndOfList()); server = server->GetNext()) {
        if (server->GetNameHash() == server_hash) {
            return server;
        }
    }
#endif
    return nullptr;
}

bool bFunkServer::CanDeliverPacket(uint32 server_hash) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    return server_hash == this->NameHash || bFunkFindServer(server_hash) != nullptr;
#else
    return false;
#endif
}

bool bFunkServer::DeliverPacket(bFunkPacket *packet) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    bFunkServer *server = this;
    if (packet->DestServer != this->NameHash) {
        server = bFunkFindServer(packet->DestServer);
    }
    if (server != nullptr) {
        server->ProcessPacket(packet);
        return true;
    }
#endif
    return false;
}

bFunkServerPlatform::bFunkServerPlatform(const char *name, int max_receive_packets) : bFunkServer(name) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    int remaining_packets = max_receive_packets;
    this->MaxReceivePackets = remaining_packets;
    this->NextReceivePacketNum = 0;
    this->ProcessingPacket = false;
    this->pReceivePackets = new (__FILE__, __LINE__) bFunkPacket[remaining_packets];

    int offset = 0;
    if (remaining_packets != 0) {
        do {
            reinterpret_cast<bFunkPacket *>(reinterpret_cast<char *>(this->pReceivePackets) + offset)->SetTotalSize(0);
            offset += sizeof(bFunkPacket);
        } while (--remaining_packets != 0);
    }

    bFunkCodeineAddServerPacket packet;
    packet.pReceivePackets = this->pReceivePackets;
    packet.MaxReceivePackets = max_receive_packets;
    bStrNCpy(packet.Name, name, sizeof(packet.Name) - 1);
    packet.Name[sizeof(packet.Name) - 1] = '\0';
    bFunkCallASync("CODEINE", 20, &packet, sizeof(packet));
#endif
}

bFunkServerPlatform::~bFunkServerPlatform() {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    bFunkCallASync("CODEINE", 21, this->GetName(), bStrLen(this->GetName()) + 1);
#endif
}

int bFunkServerPlatform::Service() {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    int packets_processed = 0;

    if (!this->ProcessingPacket) {
        bFunkPacket *packet = &this->pReceivePackets[this->NextReceivePacketNum % this->MaxReceivePackets];
        while (packet->TotalSize != 0) {
            this->ProcessingPacket = true;
            ++this->NextReceivePacketNum;
            this->ProcessPacket(packet);
            ++packets_processed;
            packet->TotalSize = 0;
            this->ProcessingPacket = false;
            packet = &this->pReceivePackets[this->NextReceivePacketNum % this->MaxReceivePackets];
        }
    }

    return packets_processed;
#else
    return 0;
#endif
}

bool bFunkServerPlatform::DeliverPacket(bFunkPacket *packet) {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    if (!bIsCodeineConnected()) {
        return false;
    }
    if (!bFunkServer::DeliverPacket(packet)) {
        bSendPacket(packet, packet->GetData(), packet->GetDataSize());
    }
    return true;
#else
    return false;
#endif
}

void bFunkService() {
#if defined(EA_PLATFORM_PLAYSTATION2) || defined(EA_PLATFORM_WIN32)
    for (bFunkServer *server = reinterpret_cast<bFunkServer *>(bFunkServerList.GetHead());
         server != reinterpret_cast<bFunkServer *>(bFunkServerList.EndOfList()); server = server->GetNext()) {
        server->Service();
    }
#endif
}

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
int bIsPlatGameCubeHW2() {
    unsigned int console_type = OSGetConsoleType();
    return console_type == OS_CONSOLE_DEVHW2 || console_type == OS_CONSOLE_TDEVHW2;
}

// STRIPPED
int bIsPlatGameCubeEmulator() {
    unsigned int console_type = OSGetConsoleType();
    return console_type == OS_CONSOLE_EMULATOR || console_type == OS_CONSOLE_PC_EMULATOR;
}
#endif
