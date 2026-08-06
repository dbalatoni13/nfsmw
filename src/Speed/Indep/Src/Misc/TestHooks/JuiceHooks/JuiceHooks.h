#ifndef MISC_TEST_HOOKS_JUICE_HOOKS_H
#define MISC_TEST_HOOKS_JUICE_HOOKS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

class IPlayer;
enum FormationType;
struct FEPackage;

class IExtension {
public:
    virtual ~IExtension() {}
};

class ICommands {
public:
    virtual ~ICommands() {}
};

namespace Juice {

struct GameHook {
    static GameHook *(*Instance)(void *, char *);
    void LogText(const char *text);
};

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
private:
    static char mJuiceBuildName[32];

public:
    char *GetTitleName();
    char *GetBuildDate();
    char *GetChangeList();
    char *GetConsoleName();
    int HasFileLoaded(const void *fileHandle);
    int NumberOfRepeatedReplayEntries(int channel);
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
    static char *GetPlayerPosition();
    static char *GetPlayerPosition(IPlayer *player);
    static char *GetFormationStr(const FormationType &formation);
    static char *GetCurrentRaceType();
    static char *GetCurrentZoneName();
    char *GetScreenName();
    char *GetCursorPos();
    void UpdatePad(void *input);
    void ResetGamePad();
    void JuiceScreenshot(char *fileName);
    static char *GetJuiceBuildName();
    int HangTimeoutLength();
    char *GetBuildName();
};

}

#endif
