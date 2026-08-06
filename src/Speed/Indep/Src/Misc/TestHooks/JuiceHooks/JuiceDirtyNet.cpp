#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"
#include "Speed/PSX2/Src/Online/PS2Isp.hpp"

extern "C" void NetConnStartup(const char *params);
extern "C" long NetConnQuery(const char *name, NetConfigRecT *configs, int count);
extern "C" long NetConnConnect(NetConfigRecT *configs, int flags);
extern "C" int printf(const char *format, ...);
extern char *NetConnMAC();
extern ProtoAriesRefT *ProtoAriesCreate(int memSize);
extern long SocketInTextGetAddr(const char *host);
extern int ProtoAriesConnect(ProtoAriesRefT *ref, int flags, long address, int port);
extern int ProtoAriesPeek(ProtoAriesRefT *ref, int *kind, int *code, char **data);
extern int ProtoAriesRecv(ProtoAriesRefT *ref, int *kind, int *channel, char *buf, int size);
extern int ProtoAriesSend(ProtoAriesRefT *ref, int length, int channel, char *buf, int size);
extern long ProtoAriesStatus(void *ref, int selector);
extern int ProtoAriesUnconnect(void *ref);
extern void ProtoAriesDestroy(void *ref);
extern int NetConnStatus(int kind, int data, int flag);
extern void NetConnShutdown(int flags);
extern void NetConnIdle();
extern char *SocketInAddrGetText(long address);

namespace Juice {

IExtension *IExtension::sCurrentExtension;
JuiceDirtyNet *JuiceDirtyNet::mInstance;

JuiceDirtyNet *JuiceDirtyNet::Instance() {
    static JuiceDirtyNet dirty;
    return &dirty;
}

JuiceDirtyNet::JuiceDirtyNet() {}

JuiceDirtyNet::~JuiceDirtyNet() {}

int JuiceDirtyNet::Connect(const char *host, int port) {
    mAries = ProtoAriesCreate(0x1000);
    return ProtoAriesConnect(mAries, 0, SocketInTextGetAddr(host), port);
}

int JuiceDirtyNet::Initialize() {
    NetConfigRecT configs[4];
    int connected = 1;
    NetConnStartup("-nosecure");
    printf("Loading network configuration");
    long count = NetConnQuery("mc0:", configs, 4);
    if (count == 0) {
        printf("\nNo network configurations found on mc0\n");
    } else if (count > -1) {
        printf("\nConfiguration Loaded Starting Juice\n");
        count = NetConnConnect(configs, 0);
        printf("\nNetConnConnect returned: %d\n", count);
        if (count > -1) {
            goto done;
        }
    } else {
        printf("\nError occured loading network configurations\n");
    }
    connected = 0;
    reinterpret_cast<GameHook *(*)()>(GameHook::Instance)()->DisableJuice();
done:
    return connected;
}

char *JuiceDirtyNet::GetMac() {
    return NetConnMAC();
}

int JuiceDirtyNet::IsConnected() {
    PeekHdr(0);
    return ProtoAriesStatus(mAries, 0x73746174) == 3;
}

int JuiceDirtyNet::Disconnect() {
    return ProtoAriesUnconnect(mAries);
}

int JuiceDirtyNet::PeekHdr(char *data) {
    int kind = 0;
    int code = 0;
    int result = ProtoAriesPeek(mAries, &kind, &code, &data);
    return result == 0 ? 1 : result;
}

int JuiceDirtyNet::Recv(char *buf, int *channel, int size) {
    int dummyKind = 0;
    return ProtoAriesRecv(mAries, &dummyKind, channel, buf, size);
}

int JuiceDirtyNet::Send(int length, int channel, char *sendBuffer) {
    int ret_val;
    unsigned int timeDiff = 0;
    unsigned int startTime =
        reinterpret_cast<GameHook *(*)()>(GameHook::Instance)()->GetCurrentSystemTime();
    do {
        NetConnIdle();
        ret_val = ProtoAriesSend(mAries, length, channel, sendBuffer, length);
        if (ret_val < 0) {
            NetConnIdle();
            char *vtable = *reinterpret_cast<char **>(IExtension::sCurrentExtension);
            short offset = *reinterpret_cast<short *>(vtable + 0xc0);
            void (**notify)(char *, int) = reinterpret_cast<void (**)(char *, int)>(vtable + 0xc4);
            (*notify)(
                reinterpret_cast<char *>(IExtension::sCurrentExtension) + offset, 5);
            NetConnIdle();
        }
        NetConnIdle();
        timeDiff += reinterpret_cast<GameHook *(*)()>(GameHook::Instance)()->GetTimeElapsed(&startTime);
    } while (ret_val < 0 || timeDiff > 3000);
    return ret_val;
}

void JuiceDirtyNet::TearDown() {
    NetConnShutdown(0);
}

int JuiceDirtyNet::IsEnabled() {
    return NetConnStatus(0x6f6e6c6e, 0, 0);
}

void JuiceDirtyNet::MaintainConn() {
    NetConnIdle();
}

void JuiceDirtyNet::FreeEverything() {
    ProtoAriesDestroy(mAries);
}

char *JuiceDirtyNet::GetLocalIpAddress() {
    long address = 0;
    unsigned int elapsed = 0;
    unsigned int startTime = reinterpret_cast<GameHook *(*)()>(GameHook::Instance)()->GetCurrentSystemTime();
    if (NetConnStatus(0x706c7567, 0, 0) != 0) {
        while (address == 0) {
            address = NetConnStatus(0x61646472, 0, 0);
            if (NetConnStatus(0x706c7567, 0, 0) == 0 || elapsed > 3000) {
                break;
            }
            elapsed += reinterpret_cast<GameHook *(*)()>(GameHook::Instance)()->GetTimeElapsed(&startTime);
        }
    }
    return SocketInAddrGetText(address);
}

}
