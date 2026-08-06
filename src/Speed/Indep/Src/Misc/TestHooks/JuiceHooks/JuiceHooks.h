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

class ICommands {
public:
    virtual ~ICommands() {}
};

namespace Juice {

namespace Scripting {

struct VarArgs {
    int mNextType;
    int mNumberOfArgs;

    int GetInt();
    float GetFloat();
    char *GetString();
};

}

class MWCommands : public ICommands {
private:
    static unsigned int mFEngScreenLoading;

public:
    static void LoadingNewFEngPackage(unsigned int newPkg);
    int TurnDebugTextOn(Scripting::VarArgs &params);
    int TurnDebugTextOff(Scripting::VarArgs &params);
    int IsInFreeRoam(Scripting::VarArgs &params);
    int UnlockAllThings(Scripting::VarArgs &params);
    int InfRaceBreaker(Scripting::VarArgs &params);
    int IsSplitScreen(Scripting::VarArgs &params);
    int TurnPursuitOn(Scripting::VarArgs &params);
    int TurnPursuitOff(Scripting::VarArgs &params);
    int TurnPursuitForeverOn(Scripting::VarArgs &params);
    int TurnPursuitForeverOff(Scripting::VarArgs &params);
};

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
