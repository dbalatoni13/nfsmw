#ifndef MISC_GAME_FLOW_H
#define MISC_GAME_FLOW_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"

enum GameFlowState {
    GAMEFLOW_STATE_EXIT_DEMO_DISC = 9,
    GAMEFLOW_STATE_UNLOADING_REGION = 8,
    GAMEFLOW_STATE_UNLOADING_TRACK = 7,
    GAMEFLOW_STATE_RACING = 6,
    GAMEFLOW_STATE_LOADING_TRACK = 5,
    GAMEFLOW_STATE_LOADING_REGION = 4,
    GAMEFLOW_STATE_IN_FRONTEND = 3,
    GAMEFLOW_STATE_UNLOADING_FRONTEND = 2,
    GAMEFLOW_STATE_LOADING_FRONTEND = 1,
    GAMEFLOW_STATE_NONE = 0,
};

// total size: 0x24
class GameFlowManager {
  public:
    GameFlowManager();
    ~GameFlowManager() {}

    void SetSingleFunction(void (*function)(int), const char *debug_name, int param);
    void SetWaitingForCallback(const char *debug_name, int phase);
    void ClearWaitingForCallback();
    void Service();
    void SetState(GameFlowState state);
    void LoadFrontend();
    void UnloadFrontend();
    void LoadTrack();
    void ReloadTrack();
    void UnloadTrack();
    void CheckForDemoDiscTimeout();
    bool IsPaused();

    void SetSingleFunction(void (*function)(), const char *debug_name) {
        SetSingleFunction(reinterpret_cast<void (*)(int)>(function), debug_name, 0);
    }

    GameFlowState GetState() {
        return this->CurrentGameFlowState;
    }

    bool IsInFrontend() {
        return this->CurrentGameFlowState == GAMEFLOW_STATE_IN_FRONTEND;
    }

    bool IsInGame() {
        return this->CurrentGameFlowState == GAMEFLOW_STATE_RACING;
    }

    bool IsLoading() {
        return GetState() == GAMEFLOW_STATE_LOADING_REGION || GetState() == GAMEFLOW_STATE_LOADING_TRACK;
    }

  private:
    void (*pSingleFunction)(int);       // offset 0x0, size 0x4
    int SingleFunctionParam;            // offset 0x4, size 0x4
    const char *pSingleFunctionName;    // offset 0x8, size 0x4
    void (*pLoopingFunction)();         // offset 0xC, size 0x4
    const char *pLoopingFunctionName;   // offset 0x10, size 0x4
    bool WaitingForCallback;            // offset 0x14, size 0x1
    const char *pCallbackName;          // offset 0x18, size 0x4
    int CallbackPhase;                  // offset 0x1C, size 0x4
    GameFlowState CurrentGameFlowState; // offset 0x20, size 0x4
};

extern GameFlowManager TheGameFlowManager; // size: 0x24

inline bool IsGameFlowInFrontEnd() {
    return TheGameFlowManager.IsInFrontend();
}

inline bool IsGameFlowInGame() {
    return TheGameFlowManager.IsInGame();
}

inline void ResetCapturedLoadingTimes() {}

inline void CaptureLoadingTime(const char *name) {}

inline void PrintCapturedLoadingTime(const char *from_name, const char *display_name) {}

void LoadGlobalAChunks();
void LoadGlobalChunks();
void BootLoadingScreen();
void UnloadFrontEndVault();
void MaybeDoMemoryProfile();
void HandleTrackStreamerLoadingBar();
void CheckLeakDetector(const char *debug_name);

// total size: 0x20
class RegionLoader {
  public:
    void BeginLoading();
    void LoadHandler();
    void FinishedLoading();
    void Unload();

    static void LoadHandler(intptr_t object) {
        reinterpret_cast<RegionLoader *>(object)->LoadHandler();
    }

  private:
    int Phase;                                // offset 0x0, size 0x4
    ResourceFile *pResourceInGameA;           // offset 0x4, size 0x4
    ResourceFile *pResourceInGameB;           // offset 0x8, size 0x4
    ResourceFile *pResourceInGameSplitScreen; // offset 0xC, size 0x4
    ResourceFile *pResourceRegion;            // offset 0x10, size 0x4
    VMFile *pResourceGlobalB_VM;              // offset 0x14, size 0x4
    VMFile *pResourceInGameB_VM;              // offset 0x18, size 0x4
    VMFile *pResourceRegion_VM;               // offset 0x1C, size 0x4
};

// total size: 0x4
class TrackLoader {
  public:
    void BeginLoading();
    void LoadHandler();
    void FinishedLoading();
    void Unload();
    void InitTopologyAndSceneryGroups();
    void CloseTopologyAndSceneryGroups();

  private:
    int Phase; // offset 0x0, size 0x4
};

extern RegionLoader TheRegionLoader;
extern TrackLoader TheTrackLoader;

#endif
