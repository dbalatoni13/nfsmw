#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"

namespace Juice {

char *MWExtension::GetTitleName() {
    return "NFS Most Wanted";
}

char *MWExtension::GetBuildDate() {
    return "Sep 20 2005";
}

int MWExtension::GetServerPort() {
    return 0x7919;
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

char *MWExtension::GetImageName() {
    return "NFS_MW_Map";
}

int MWExtension::HangTimeoutLength() {
    return 65000;
}

}
