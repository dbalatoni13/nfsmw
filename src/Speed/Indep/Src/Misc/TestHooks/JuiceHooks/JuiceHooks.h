#ifndef MISC_TEST_HOOKS_JUICE_HOOKS_H
#define MISC_TEST_HOOKS_JUICE_HOOKS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

class IExtension {
public:
    virtual ~IExtension() {}
};

namespace Juice {

class MWExtension : public IExtension {
public:
    char *GetTitleName();
    char *GetBuildDate();
    int GetReadyToReset();
    void FileSyncUpdate();
    char *GetServerIP();
    int GetServerPort();
    int GetBackupServerPort();
    char *GetBackupServerIP();
    char *GetXboxPath();
    char *GetXboxExeName();
    char *GetConsoleTarget();
    unsigned int GetIntializationTimeOut();
    unsigned int GetHeartBeatTimeOut();
    float GetGameTime();
    int IsOkToConnect();
    void InstantiateScriptExt();
    int NetworkInitRetries();
    int InitialConnectionRetries();
    int WaitBetweenInitialConnectionRretries();
    int WaitBetweenNetworkInitRetries();
    char *GetPopupName();
    char *GetPopupCheck();
    int GetJuiceUpdateProcessor();
    static char *GetImageName();
    static char *GetJuiceBuildName();
    int HangTimeoutLength();
    char *GetBuildName();
};

}

#endif
