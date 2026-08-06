#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/AI/aireflectedtypes.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
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

char *MWExtension::GetScreenName() {
    static char screenName[128];
    bMemSet(screenName, '\0', 0x80);
    FEPackage *package = cFEng::Get()->FindPackageWithControl();
    if (package != nullptr) {
        package = cFEng::Get()->FindPackageWithControl();
        bStrCpy(screenName, *reinterpret_cast<char **>(reinterpret_cast<char *>(package) + 0xc));
    }
    return screenName;
}

char *MWExtension::GetCursorPos() {
    static char cursorPos[32];
    bMemSet(cursorPos, '\0', 0x20);
    FEPackage *package = cFEng::Get()->FindPackageWithControl();
    FEObject *button;
    if (package == nullptr) {
        goto no_button;
    }
    button = *reinterpret_cast<FEObject **>(reinterpret_cast<char *>(package) + 0x98);
    if (button == nullptr) {
        goto no_button;
    }
    bSPrintf(cursorPos, "%d", *reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(button) + 0x10));
    return cursorPos;
no_button:
    return "null";
}

char *MWExtension::GetPlayerPosition() {
    return GetPlayerPosition(IPlayer::First(PLAYER_LOCAL));
}

char *MWExtension::GetPlayerPosition(IPlayer *player) {
    UMath::Vector3 vecCoords;
    char *destString;
    if (player == nullptr) {
        if (IPlayer::First(PLAYER_LOCAL) == nullptr) {
            return "0.0;0.0;0.0";
        }
        vecCoords = IPlayer::First(PLAYER_LOCAL)->GetPosition();
        static char coords[128];
        destString = coords;
    } else {
        vecCoords = player->GetPosition();
        static char coords[128];
        destString = coords;
    }
    bSPrintf(destString, "%f;%f;%f", vecCoords.z, -vecCoords.x, vecCoords.y);
    return destString;
}

char *MWExtension::GetFormationStr(const FormationType &formation) {
    switch (formation) {
    case PIT:
        return "PIT";
    case BOX_IN:
        return "BOX_IN";
    case ROLLING_BLOCK:
        return "ROLLING_BLOCK";
    case FOLLOW:
        return "FOLLOW";
    case HELI_PURSUIT:
        return "HELI_PURSUIT";
    case HERD:
        return "HERD";
    default:
        return "NONE";
    }
}

char *MWExtension::GetCurrentRaceType() {
    GRace::Type raceType = GRaceStatus::Get().GetRaceType();
    switch (raceType + GRace::kRaceType_Circuit) {
    case 0:
        return "None";
    case 1:
        return "P2P";
    case 2:
        return "Circuit";
    case 3:
        return "Drag";
    case 4:
        return "Knockout";
    case 5:
        return "Tollbooth";
    case 6:
        return "SpeedTrap";
    case 7:
        return "Checkpoint";
    case 8:
        return "CashGrab";
    case 9:
        return "Challenge";
    default:
        return "Unknown";
    }
}

char *MWExtension::GetCurrentZoneName() {
    return TheTrackStreamer.GetCurrentZoneName();
}

void MWExtension::JuiceScreenshot(char *fileName) {
    GameHook::Instance(this, fileName)->LogText("[MW SS] - Calling Juice screenshot");
}

}
