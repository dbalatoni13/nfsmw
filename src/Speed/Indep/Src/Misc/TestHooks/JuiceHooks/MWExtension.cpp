#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/file/driver.h"
#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system/systask.h"

extern int BuildVersionChangelistNumber;
extern int ForceJuiceConnect;
extern char *ForceJuiceConnectIP;
extern int GetJoylogChannelRepeatCount(int channel);
extern int ASYNCFILE_getstatus(int handle);

namespace Juice {

char MWExtension::mJuiceBuildName[32];

static char changeList[32];
static char name[255];

char *MWExtension::GetTitleName() {
    return "NFS Most Wanted";
}

char *MWExtension::GetBuildDate() {
    return "Sep 20 2005";
}

char *MWExtension::GetChangeList() {
    bSPrintf(changeList, "%d", BuildVersionChangelistNumber);
    return changeList;
}

char *MWExtension::GetConsoleName() {
    bMemSet(name, '\0', 0xff);
    return "Default PS2";
}

int MWExtension::GetServerPort() {
    return 0x7919;
}

int MWExtension::GetReadyToReset() {
    g_pEAXSound->RestoreDriver();
    return 0;
}

void MWExtension::FileSyncUpdate() {
    SYNCTASK_run();
}

int MWExtension::HasFileLoaded(const void *fileHandle) {
    return static_cast<int>(ASYNCFILE_getstatus(reinterpret_cast<int>(fileHandle)) == 1);
}

char *MWExtension::GetServerIP() {
    char *result;
    if ((ForceJuiceConnect == 0) ||
        (result = ForceJuiceConnectIP, *ForceJuiceConnectIP == '\0')) {
        result = "10.10.229.117";
    }
    return result;
}

int MWExtension::GetBackupServerPort() {
    return 0x7919;
}

char *MWExtension::GetBackupServerIP() {
    return "10.10.229.117";
}

char *MWExtension::GetXboxPath() {
    return "xe:\\CDMW\\";
}

char *MWExtension::GetXboxExeName() {
    return "nfsMWMilestone.exe";
}

char *MWExtension::GetConsoleTarget() {
    return "nfsMWMilestone.elf";
}

unsigned int MWExtension::GetIntializationTimeOut() {
    return 30000;
}

unsigned int MWExtension::GetHeartBeatTimeOut() {
    return 20000;
}

float MWExtension::GetGameTime() {
    return 0.0f;
}

void MWExtension::InstantiateScriptExt() {}

int MWExtension::NetworkInitRetries() {
    return 100;
}

int MWExtension::InitialConnectionRetries() {
    return 100;
}

int MWExtension::WaitBetweenInitialConnectionRretries() {
    return 200;
}

int MWExtension::WaitBetweenNetworkInitRetries() {
    return 0x96;
}

char *MWExtension::GetPopupName() {
    return "null";
}

char *MWExtension::GetPopupCheck() {
    return "false";
}

int MWExtension::GetJuiceUpdateProcessor() {
    return 0;
}

int MWExtension::NumberOfRepeatedReplayEntries(int channel) {
    return GetJoylogChannelRepeatCount(channel);
}

char *MWExtension::GetBuildName() {
    return GetJuiceBuildName();
}

char *MWExtension::GetImageName() {
    return "NFS_MW_Map";
}

int MWExtension::HangTimeoutLength() {
    return 65000;
}

char *MWExtension::GetJuiceBuildName() {
    if (ForceJuiceConnect != 0) {
        return "TestBuild";
    }
    if (BuildRegion::IsPal()) {
        bSPrintf(mJuiceBuildName, "%d-PAL", BuildVersionChangelistNumber);
    } else {
        bSPrintf(mJuiceBuildName, "%d", BuildVersionChangelistNumber);
    }
    return mJuiceBuildName;
}

}
