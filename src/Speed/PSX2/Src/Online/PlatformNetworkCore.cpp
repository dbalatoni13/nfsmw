#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/PSX2/bWare/Src/ee/include/eekernel.h"

extern int bSuperBenderConnected;
extern int PowerOff_ThreadID;
extern int PowerOff_SemaID;
int bIsThereACDInTheDrive();
void ClosePlatform();
void LoadNetworkIRXModulesPart2();
void RemoveTimerInterrupt();
void RemoveVIFHandler();
void SoundPause(bool pause, eSNDPAUSE_REASON reason);

extern "C" {
int32 NetConnStatus(int32 kind, int32 data, void *buffer);
void NetConnSleep(int32 milliseconds);
void NetConnStartup(const char *params);
void NetConnIdle();
void NetConnShutdown(int32 flags);
void RpcDone();
int sceCdPOffCallback(void *callback, void *userdata);
int sceCdInit(int mode);
int sceDevctl(char *path, int command, void *arg, size_t arglen, void *buffer, size_t buflen);
int sceDmaReset(int mode);
int scePadEnd();
void sceSifExitCmd();
}

class PlatformNetworkCore {
  public:
    PlatformNetworkCore();
    virtual ~PlatformNetworkCore();

    bool IsOnline();
    void ExecNetGui();

  protected:
    bool Init();
    void Destroy();

  private:
    void LoadIRXs();
    void PrepareForExec();
    char *FilenameFromSlus();
};

PlatformNetworkCore::PlatformNetworkCore() {}

PlatformNetworkCore::~PlatformNetworkCore() {}

bool PlatformNetworkCore::IsOnline() {
    int status = NetConnStatus('onln', 0, 0);
    if (status != 1) {
        return false;
    }
    status = NetConnStatus('conn', 0, 0);
    return status == '+isp' || status == '+onl';
}

void PlatformNetworkCore::ExecNetGui() {
    char *pcNetConfigElf;
    char *pcNetConfigArgs[2];
    char buf[64];

    PrepareForExec();
    pcNetConfigElf = FilenameFromSlus();
    bSPrintf(buf, "cdrom0:\\%s;1", pcNetConfigElf);
    if (bIsThereACDInTheDrive()) {
        pcNetConfigElf = "cdrom0:\\NETGUI\\NTGUI.ELF;1";
        pcNetConfigArgs[0] = buf;
    } else {
        pcNetConfigElf = "host:NETGUI/ntgui_host_uc.elf";
        pcNetConfigArgs[0] = "host:SPEED.ELF";
    }
    LoadExecPS2(pcNetConfigElf, 1, pcNetConfigArgs);
}

bool PlatformNetworkCore::Init() {
    LoadIRXs();
    NetConnStartup(bSuperBenderConnected ? "-coop" : "");
    NetConnIdle();
    return true;
}

void PlatformNetworkCore::Destroy() { NetConnShutdown(0); }

void PlatformNetworkCore::LoadIRXs() {
    static bool firstTime = true;
    if (!firstTime) {
        return;
    }
    firstTime = false;

    while (TheCarLoader.IsLoadingInProgress()) {
        ServiceResourceLoading();
    }

    SoundPause(true, eSNDPAUSE_ONLINE);
    SetSoundControlState(true, SNDSTATE_PAUSE, "PS2 Online");
    g_pEAXSound->Update(0.1f);
    bWaitUntilAsyncDone(nullptr);
    LoadNetworkIRXModulesPart2();
    SoundPause(false, eSNDPAUSE_ONLINE);
    SetSoundControlState(false, SNDSTATE_PAUSE, "PS2 Online");
}

void PlatformNetworkCore::PrepareForExec() {
    int i = 2;

    Destroy();
    ClosePlatform();
    TheGameFlowManager.UnloadFrontend();
    sceCdPOffCallback(0, 0);
    TerminateThread(PowerOff_ThreadID);
    DeleteThread(PowerOff_ThreadID);
    DeleteSema(PowerOff_SemaID);
    RemoveTimerInterrupt();
    RemoveVIFHandler();
    sceDevctl("dev9x:", 0x4402, 0, 0, 0, 0);
    do {
        --i;
        RpcDone();
        NetConnSleep(50);
    } while (i > 0);
    sceDmaReset(1);
    scePadEnd();
    sceCdInit(5);
    sceSifExitCmd();
}

char *PlatformNetworkCore::FilenameFromSlus() {
    static char slus[16];
    bSPrintf(slus, BuildRegion::GetSlusCode());
    slus[10] = slus[9];
    slus[9] = slus[8];
    slus[8] = '.';
    slus[11] = '\0';
    return slus;
}
