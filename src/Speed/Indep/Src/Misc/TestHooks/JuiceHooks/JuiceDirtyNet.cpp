#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"

extern char *NetConnMAC();
extern ProtoAriesRefT *ProtoAriesCreate(int memSize);
extern long SocketInTextGetAddr(const char *host);
extern int ProtoAriesConnect(ProtoAriesRefT *ref, int flags, long address, int port);
extern int ProtoAriesPeek(ProtoAriesRefT *ref, int *kind, int *code, char **data);
extern int ProtoAriesRecv(ProtoAriesRefT *ref, int *kind, int *channel, char *buf, int size);
extern long ProtoAriesStatus(void *ref, int selector);
extern int ProtoAriesUnconnect(void *ref);
extern void ProtoAriesDestroy(void *ref);
extern int NetConnStatus(int kind, int data, int flag);
extern void NetConnShutdown(int flags);
extern void NetConnIdle();
extern char *SocketInAddrGetText(long address);

namespace Juice {

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
    char *localData = data;
    int kind = 0;
    int code = 0;
    int result = ProtoAriesPeek(mAries, &kind, &code, &localData);
    return result == 0 ? 1 : result;
}

int JuiceDirtyNet::Recv(char *buf, int *channel, int size) {
    int dummyKind = 0;
    return ProtoAriesRecv(mAries, &dummyKind, channel, buf, size);
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
