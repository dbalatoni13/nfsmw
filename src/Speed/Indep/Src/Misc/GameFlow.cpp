#include "GameFlow.hpp"
#include "AttribAlloc.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/TimeOfDay.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/World/World.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "ResourceLoader.hpp"
#include "bFile.hpp"

GameFlowManager TheGameFlowManager; // size: 0x24

void RenderTrackMarkers(eView *view) {}

// todo
extern int SkipFE;
extern int SkipFESplitScreen;

// UNSOLVED
Sim::eUserMode CalculateSimMode() {
    Sim::eUserMode mode = Sim::USER_SINGLE;
    if (SkipFE) {
        if (SkipFESplitScreen) {
            mode = Sim::USER_SPLIT_SCREEN;
        }
    } else {
        if (TheOnlineManager.IsOnlineRace()) {
            mode = Sim::USER_ONLINE;
        } else if (FEDatabase->IsSplitScreenMode() && FEDatabase->iNumPlayers == 2) {
            mode = Sim::USER_SPLIT_SCREEN;
        } else {
            mode = Sim::USER_SINGLE;
        }
    }
    return mode;
}

void CodeOverlayUnloadingFrontend() {}

void CheckLeakDetector(const char *) {}

void MaybeDoMemoryProfile() {}

void SetLeakDetector() {}

// Forward declarations
void BeginGameFlowLoadingFrontEnd();
void BeginGameFlowUnloadingFrontEnd();
void BeginGameFlowLoadRegion();
void BeginGameFlowUnloadTrack(int reload);
void LoadFrontEndVault(bool high_alloc);

GameFlowManager::GameFlowManager()
    : pSingleFunction(nullptr),    //
      SingleFunctionParam(0),      //
      pSingleFunctionName(nullptr), //
      pLoopingFunction(nullptr),   //
      pLoopingFunctionName(nullptr), //
      WaitingForCallback(false),   //
      pCallbackName(nullptr),      //
      CallbackPhase(0) {}

void GameFlowManager::SetSingleFunction(void (*function)(int), const char *debug_name, int param) {
    if (pSingleFunction != nullptr) {
        pSingleFunction = nullptr;
    }
    if (WaitingForCallback) {
        WaitingForCallback = false;
    }
    pSingleFunction = function;
    pSingleFunctionName = debug_name;
    SingleFunctionParam = param;
}

void GameFlowManager::SetWaitingForCallback(const char *name, int phase) {
    if (pSingleFunction != nullptr) {
        pSingleFunction = nullptr;
    }
    if (WaitingForCallback) {
        WaitingForCallback = false;
    }
    WaitingForCallback = true;
    pCallbackName = name;
    CallbackPhase = phase;
}

void GameFlowManager::ClearWaitingForCallback() {
    if (!WaitingForCallback) {
        return;
    }
    WaitingForCallback = false;
}

void GameFlowManager::Service() {
    void (*func)(int);
    void (*saved)(int);
    func = pSingleFunction;
    if (func != nullptr) {
        do {
            saved = pSingleFunction;
            int param = SingleFunctionParam;
            pSingleFunction = nullptr;
            SingleFunctionParam = 0;
            pSingleFunctionName = nullptr;
            saved(param);
            func = pSingleFunction;
        } while (func != saved && func != nullptr);
    }
    if (pLoopingFunction != nullptr) {
        pLoopingFunction();
    }
    CheckForDemoDiscTimeout();
}

void GameFlowManager::SetState(GameFlowState state) {
    CurrentGameFlowState = state;
}

void GameFlowManager::LoadFrontend() {
    SetLeakDetector();
    LoadFrontEndVault(false);
    SetSingleFunction(reinterpret_cast<void (*)(int)>(BeginGameFlowLoadingFrontEnd), "LoadFrontend", 0);
}

void GameFlowManager::UnloadFrontend() {
    SetSingleFunction(reinterpret_cast<void (*)(int)>(BeginGameFlowUnloadingFrontEnd), "UnloadFrontend", 0);
}

void GameFlowManager::LoadTrack() {
    SetSingleFunction(reinterpret_cast<void (*)(int)>(BeginGameFlowLoadRegion), "LoadTrack", 0);
}

void GameFlowManager::ReloadTrack() {
    BeginGameFlowUnloadTrack(1);
}

void GameFlowManager::UnloadTrack() {
    BeginGameFlowUnloadTrack(0);
}

void GameFlowManager::CheckForDemoDiscTimeout() {}

bool GameFlowManager::IsPaused() {
    return Sim::GetState() == Sim::STATE_IDLE;
}

// TODO
void GetBuildVersionName(char *build_version_name) {
#ifdef MILESTONE_OPT
    bStrCpy(build_version_name, "Milestone");
#else
    bStrCpy(build_version_name, "Release");
#endif
}
