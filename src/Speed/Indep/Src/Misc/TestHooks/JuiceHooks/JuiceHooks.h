#ifndef MISC_TEST_HOOKS_JUICE_HOOKS_H
#define MISC_TEST_HOOKS_JUICE_HOOKS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Input/Action.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

class IPlayer;
enum FormationType;
struct FEPackage;
struct MoviePlayer {
    char *GetMovieFilename();
};
extern MoviePlayer *gMoviePlayer;

class ICountdown : public UTL::COM::IUnknown {
public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ICountdown(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}
    virtual ~ICountdown() {}
    virtual void BeginCountdown();
    virtual bool IsActive();
    virtual float GetSecondsBeforeRaceStart();
};

class IExtension {
public:
    virtual ~IExtension() {}
};

class ICommands {
public:
    virtual ~ICommands() {}
};

class INetwork {
public:
    virtual int Connect();
    virtual int IsConnected();
    virtual int Recv();
    virtual int PeekHdr();
    virtual int Send();
    virtual int IsEnabled();
    virtual int Initialize();
    virtual int Disconnect();
    virtual void MaintainConn();
    virtual void TearDown();
    virtual void FreeEverything();
    virtual char *GetMac();
    virtual char *GetLocalIpAddress();
};

namespace Juice {

class JuiceDirtyNet : public INetwork {
private:
    static JuiceDirtyNet *mInstance;

public:
    static JuiceDirtyNet *Instance();
    virtual int Connect(const char *ip, int port);
    virtual int IsConnected();
    virtual int Recv(char *buffer, int *size, int flags);
    virtual int PeekHdr(char *buffer);
    virtual int Send(int channel, int size, char *buffer);
    virtual int IsEnabled();
    virtual int Initialize();
    virtual int Disconnect();
    virtual void MaintainConn();
    virtual void TearDown();
    virtual void FreeEverything();
    virtual char *GetMac();
    virtual char *GetLocalIpAddress();
};

struct GameHook {
    static GameHook *(*Instance)(void *, char *);
    void LogText(const char *text);
};

namespace Scripting {

struct VarArgs {
    int mNextType;
    int mNumberOfArgs;

    int GetNumberOfRemainingArgs();
    int GetInt(int &result);
    float GetFloat(float &result);
    char *GetString(char *&result);
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
    int PassChallenge(Scripting::VarArgs &params);
    int PassSpeedtrap(Scripting::VarArgs &params);
    int AwardBounty(Scripting::VarArgs &params);
    int GetBountyValue(Scripting::VarArgs &params);
    char *GetTrackID(Scripting::VarArgs &params);
    int IsPursuit(Scripting::VarArgs &params);
    int InFrontEnd(Scripting::VarArgs &params);
    int InDriveMode(Scripting::VarArgs &params);
    int SetHeat(Scripting::VarArgs &params);
    int FinishRaceInPlace(Scripting::VarArgs &params);
    int TeleportToCoords(Scripting::VarArgs &params);
    int IsCarStuck(Scripting::VarArgs &params);
    char *DisplayFMVfilename(Scripting::VarArgs &params);
    int SetFrameRateThreshold(Scripting::VarArgs &params);
    int ScreenLoaded(Scripting::VarArgs &params);
    int TurnPursuitOn(Scripting::VarArgs &params);
    int TurnPursuitOff(Scripting::VarArgs &params);
    int TurnPursuitForeverOn(Scripting::VarArgs &params);
    int TurnPursuitForeverOff(Scripting::VarArgs &params);
};

}

enum MWJuicePadButtonType {
    JUICE_UP = 0,
    JUICE_DOWN,
    JUICE_LEFT,
    JUICE_RIGHT,
    JUICE_ACCEPT,
    JUICE_CANCEL,
    JUICE_Y,
    JUICE_X,
    JUICE_L1,
    JUICE_R1,
    JUICE_WHITE,
    JUICE_BLACK,
    JUICE_LSTICKBUTTON,
    JUICE_RSTICKBUTTON,
    JUICE_START,
    JUICE_BACK,
    JUICE_R3,
    JUICE_L3,
    JUICE_LEFTANALOG_LEFT,
    JUICE_LEFTANALOG_RIGHT,
    JUICE_LEFTANALOG_UP,
    JUICE_LEFTANALOG_DOWN,
    JUICE_RIGHTANALOG_LEFT,
    JUICE_RIGHTANALOG_RIGHT,
    JUICE_RIGHTANALOG_UP,
    JUICE_RIGHTANALOG_DOWN,
    JUICE_MAX_BUTTON
};

enum MWJPadReleaseType {
    JUICE_NORMAL = 0,
    JUICE_FORCED
};

struct MWJuicePadState {
    bool buttonState[26];
};

struct MWJuicePad {
private:
    static MWJuicePad *mInstance;
    MWJuicePadState mPadState[2];
    MWJuicePadState mTrackSegments[2];
    bool mIsInBE;
    bool mButtonPressedLastFrame;
    ActionID FEActionMapping[26];
    ActionID BEActionMapping[26];
    ActionQueue *mInputQueue;
    bool mIsLastFrame;
    bool mResetSegmentPresses;

public:
    static MWJuicePad *Instance();
    void Initialize();
    void PressButton(int port, MWJuicePadButtonType buttonType);
    void ReleaseAllButtons(MWJPadReleaseType type);
    void PollInput();
    void SetIsLastFrame(bool val);
    bool IsLastJoyFrame();
    void TrackButtonPress(int actionId, float data, int port);
    void ResetGamePad();
    void ReleaseSegmentPresses();

protected:
    MWJuicePad();

private:
    void SimulateFEButton(int port, int buttonID, float buttonData);
    void SimulateBEButton(int port, int buttonID, float buttonData);
    ActionQueue *GetActionQueue(const char *qName, int port);
    void TrackFEPresses(int actionID, float data, int port);
    void TrackBEPresses(int actionID, float data, int port);
    void ResetTrackedPresses();
};

enum JuicePerRaceStatType {
    JUICE_COP_CAR_SPAWN = 0,
    JUICE_COP_CAR_UNSPAWN,
    JUICE_COP_CHOPPER_SPAWN,
    JUICE_COP_CHOPPER_UNSPAWN,
    JUICE_TRAFFIC_SPAWN,
    JUICE_TRAFFIC_UNSPAWN,
    JUICE_CURRENT_HEAT,
    JUICE_MAX_HEAT,
    JUICE_CURRENT_CAMERA,
    JUICE_CURRENT_RACE,
    JUICE_MAX_PER_RACE_STAT
};

enum JuiceRaceType {
    JUICE_FREE_ROAM = 0,
    JUICE_CIRCUIT,
    JUICE_P2P,
    JUICE_OTHERRACE
};

enum CareerDataType {
    JUICE_REP_EARNED = 0,
    JUICE_CASH_EARNED,
    JUICE_PURSUIT_LENGTH,
    JUICE_MAX_CAREER_DATA
};

struct JuiceStatsDB {
private:
    static JuiceStatsDB *mInstance;
    int mPerRaceStatsDB[10];
    int mFrameOfLastInc[10];
    bool mShouldDumpStats;
    int mCareerIntDataDB[15][3];
    float mCareerFloatDataDB[15][3];
    bool mShouldDumpCareerData;
    float mTimeAtHeatLevel[11];
    float mTimeInCameraMode[6];
    float mTimeInRaceType[6];
    unsigned int mCurrentTicker;
    OptionsSettings mCurrentOptions;

public:
    static JuiceStatsDB *Instance();
    JuiceStatsDB();
    void ResetPerRaceStats();
    void ResetCareerData();
    void SetHeat(int heat);
    void SetCamera(int camID);
};

namespace Juice {

class MWExtension : public IExtension {
private:
    int mHasExecutedRPC;
    int mScreenShotHandle;
    static char mFileName[256];
    static char mJuiceBuildName[32];

public:
    static MWExtension *Instance();
    MWExtension();
    void ThreadYield(int dur);
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
