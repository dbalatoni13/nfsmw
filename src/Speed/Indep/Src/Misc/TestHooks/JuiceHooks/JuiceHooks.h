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

struct ProtoAriesRefT;

namespace Juice {

struct tJuicePad {
    unsigned int mButtons;
    float mButtonVal[32];
};

class IExtension {
public:
    static IExtension *sCurrentExtension;
    IExtension();
    ~IExtension() {}
    virtual char *GetTitleName();
    virtual char *GetBuildName();
    virtual char *GetChangeList();
    virtual char *GetConsoleTarget();
    virtual char *GetXboxPath();
    virtual char *GetXboxExeName();
    virtual char *GetServerIP();
    virtual char *GetConsoleName();
    virtual int GetServerPort();
    virtual int GetReadyToReset();
    virtual void FileSyncUpdate();
    virtual void GamePadToJuicePad(void *gameInput, tJuicePad *juiceInput);
    virtual void JuicePadToGamePad(tJuicePad *juiceInput, void *gameInput);
    virtual int HasFileLoaded(const void *fileHandle);
    virtual char *GetBuildDate();
    virtual unsigned int GetInitializationTimeOut();
    virtual unsigned int GetHeartBeatTimeOut();
    virtual char *GetBackupServerIP();
    virtual int GetBackupServerPort();
    virtual int NetworkInitRetries();
    virtual int InitialConnectionRetries();
    virtual int WaitBetweenInitialConnectionRretries();
    virtual int WaitBetweenNetworkInitRetries();
    virtual void ThreadYield();
    virtual char *GetScreenName();
    virtual char *GetPopupName();
    virtual char *GetPopupCheck();
    virtual char *GetCursorPos();
    virtual float GetGameTime();
    virtual int IsOkToConnect();
    virtual void InstantiateScriptExt();
    virtual void ResetGamePad();
    virtual int HangTimeoutLength();
    virtual void UpdatePad();
    virtual void JuiceScreenshot();
    virtual int NumberOfRepeatedReplayEntries();
    virtual int GetJuiceThreadPriority();
    virtual int GetJuiceUpdateProcessor();
    virtual void PauseGame();
    virtual void UnPauseGame();
};

class INetwork {
public:
    ~INetwork();
    INetwork();
    virtual int Connect();
    virtual int IsConnected();
    virtual int Recv();
    virtual int PeekHdr(char *buffer);
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

class JuicePad {
public:
    static JuicePad *Instance();
    void SetButton(float value, int button);
    int IsButtonPressed(int button);
    int DidInputHappen();
    tJuicePad *GetMasterPad();
    void UpdateJuicePad();
    void ResetButtons();
};

class JuiceDirtyNet : public INetwork {
private:
    static JuiceDirtyNet *mInstance;
    ProtoAriesRefT *mAries;

public:
    static JuiceDirtyNet *Instance();
    JuiceDirtyNet();
    virtual ~JuiceDirtyNet();
    virtual int Connect(const char *ip, int port);
    virtual int IsConnected();
    virtual int Recv(char *buffer, int *size, int flags);
    virtual int PeekHdr(char *buffer);
    virtual int Send(int length, int channel, char *buffer);
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
    static GameHook *(*Instance)();
    int mEnabled;
    int mFrame;
    void LogText(const char *text);
    void DisableJuice();
    void Initialize();
    void InitTrapHandler();
    void GameEvent(char *world, char *event, char *outcome, int fpsAsInt, float fps,
                   char *zoneName, char *imageName, char *playerPosition);
    void LogStat(int stat, char *statType, char *raceType, int value);
    void AssetHit(char *assetName, char *assetValue);
    int GetFrame() {
        return mFrame;
    }
    unsigned int GetCurrentSystemTime();
    int GetTimeElapsed(unsigned int *startTime);
};

struct PadConfigManager {
    static PadConfigManager *Instance();
    bool IsFENavCapturing();
    void CaptureKeyAndState(int buttonMask);
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

class ICommands {
public:
    ICommands();
    virtual ~ICommands();
    virtual void AutoRegister();
    virtual char *GetNamespaceName() const;
    static void SendMethod(int type, const ICommands *obj, const char *methodName, int argSize,
                           const void *method);

    template <class T>
    static void SendMethod(int type, const ICommands *obj, const char *methodName, int argSize,
                           T method) {
        T m = method;
        SendMethod(type, obj, methodName, argSize, static_cast<const void *>(&m));
    }
};

}

class MWCommands : public Scripting::ICommands {
private:
    static unsigned int mFEngScreenLoading;

public:
    static MWCommands *Instance();
    MWCommands();
    virtual char *GetNamespaceName() const;
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
    int SpeedBoost(Scripting::VarArgs &params);
    int TurnAutoPilotOn(Scripting::VarArgs &params);
    int TurnAutoPilotOff(Scripting::VarArgs &params);
    int ScatterCops(Scripting::VarArgs &params);
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
    void SubmitStats(JuiceRaceType raceType);
    void SubmitCareerData(const char *profileName);
    void CompareOptions(OptionsSettings *compare);
    void ResetPerRaceStats();
    void IncrementPerRaceStat(JuicePerRaceStatType statType);
    void UpdateTimers();
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
    static void JuiceLowFrameRateLog(const float &fps);
    char *GetScreenName();
    char *GetCursorPos();
    void UpdatePad(void *input);
    void ResetGamePad();
    void GamePadToJuicePad(void *gameInput, tJuicePad *juiceInput);
    void JuicePadToGamePad(tJuicePad *juiceInput, void *gameInput);
    void JuiceScreenshot(char *fileName);
    static char *GetJuiceBuildName();
    int HangTimeoutLength();
    char *GetBuildName();
};

}

#endif
