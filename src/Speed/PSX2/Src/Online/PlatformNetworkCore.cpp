#include "Speed/Indep/Src/Misc/BuildRegion.hpp"

#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

extern int bSuperBenderConnected;

extern "C" {
int32 NetConnStatus(int32 kind, int32 data, void *buffer);
void NetConnStartup(const char *params);
void NetConnIdle();
void NetConnShutdown(int32 flags);
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

bool PlatformNetworkCore::Init() {
    LoadIRXs();
    NetConnStartup(bSuperBenderConnected ? "-coop" : "");
    NetConnIdle();
    return true;
}

void PlatformNetworkCore::Destroy() { NetConnShutdown(0); }

char *PlatformNetworkCore::FilenameFromSlus() {
    static char slus[16];
    bSPrintf(slus, BuildRegion::GetSlusCode());
    slus[10] = slus[9];
    slus[9] = slus[8];
    slus[8] = '.';
    slus[11] = '\0';
    return slus;
}
