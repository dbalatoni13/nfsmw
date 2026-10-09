//
//
//
//
//
//
//
//
#ifndef GAMEFLOW_HPP
#define GAMEFLOW_HPP

#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"

// Decl: 13
enum GameFlowState {
    GAMEFLOW_STATE_NONE = 0,
    GAMEFLOW_STATE_LOADING_FRONTEND = 1,
    GAMEFLOW_STATE_UNLOADING_FRONTEND = 2,
    GAMEFLOW_STATE_IN_FRONTEND = 3,
    GAMEFLOW_STATE_LOADING_REGION = 4,
    GAMEFLOW_STATE_LOADING_TRACK = 5,
    GAMEFLOW_STATE_RACING = 6,
    GAMEFLOW_STATE_UNLOADING_TRACK = 7,
    GAMEFLOW_STATE_UNLOADING_REGION = 8,
    GAMEFLOW_STATE_EXIT_DEMO_DISC = 9,
};

// total size: 0x24
// Decl: 29
class GameFlowManager {
  public:
    GameFlowManager();
    ~GameFlowManager() {}

    void SetSingleFunction(void (*function)(int32), const char *debug_name, int param);

    // Decl: 36
    void SetSingleFunction(void (*function)(), const char *debug_name) {
        this->SetSingleFunction(reinterpret_cast<void (*)(int32)>(function), debug_name, 0);
    }

    void SetWaitingForCallback(const char *debug_name, int debug_phase); // Decl: 43
    void ClearWaitingForCallback();                                      // Decl: 44

    void Service(); // Decl: 47

    void SetState(enum GameFlowState state); // Decl: 49
    // Decl: 50
    GameFlowState GetState() {
        return this->CurrentGameFlowState;
    }

    // Decl: 52
    bool IsInFrontend() {
        return this->CurrentGameFlowState == GAMEFLOW_STATE_IN_FRONTEND;
    }
    // Decl: 53
    bool IsInGame() {
        return this->CurrentGameFlowState == GAMEFLOW_STATE_RACING;
    }
    // Decl: 54
    bool IsLoading() {
        return this->GetState() == GAMEFLOW_STATE_LOADING_REGION || this->GetState() == GAMEFLOW_STATE_LOADING_TRACK;
    }

    bool IsPaused(); // Decl: 56

    void LoadFrontend();   // Decl: 59
    void UnloadFrontend(); // Decl: 60
    void LoadTrack();      // Decl: 61
    void UnloadTrack();    // Decl: 62

    void ReloadTrack(); // Decl: 64

    void CheckForDemoDiscTimeout(); // Decl: 66

  private:
    void (*pSingleFunction)(int32);  // offset 0x0, size 0x4, Decl: 70
    int32 SingleFunctionParam;       // offset 0x4, size 0x4, Decl: 71
    const char *pSingleFunctionName; // offset 0x8, size 0x4, Decl: 72

    void (*pLoopingFunction)();       // offset 0xC, size 0x4, Decl: 75
    const char *pLoopingFunctionName; // offset 0x10, size 0x4, Decl: 76

    bool WaitingForCallback;   // offset 0x14, size 0x1, Decl: 79
    const char *pCallbackName; // offset 0x18, size 0x4, Decl: 80
    int CallbackPhase;         // offset 0x1C, size 0x4, Decl: 81

    GameFlowState CurrentGameFlowState; // offset 0x20, size 0x4, Decl: 84
};

extern GameFlowManager TheGameFlowManager; // size: 0x24, Decl: 88

// Decl: 106
inline bool IsGameFlowInFrontEnd() {
    return TheGameFlowManager.IsInFrontend();
}
// Decl: 107
inline bool IsGameFlowInGame() {
    return TheGameFlowManager.IsInGame();
}
// Decl: 108
inline bool IsGameFlowLoadingGame() {}
// Decl: 109
inline bool IsGameFlowPaused() {}

// Decl: 144
inline void ResetCapturedLoadingTimes() {}
// Decl: 145
inline void CaptureLoadingTime(const char *name) {}
// Decl: 146
inline float GetCapturedLoadingTime() {}

inline float GetCapturedLoadingTime(const char * from_name) {}

// Decl: 148
inline void PrintCapturedLoadingTime(const char *from_name, const char *display_name) {}

void LoadGlobalAChunks();
void LoadGlobalChunks();
void BootLoadingScreen();
void UnloadFrontEndVault();
void MaybeDoMemoryProfile();
#ifndef EA_BUILD_A124
void HandleTrackStreamerLoadingBar();
#endif
void CheckLeakDetector(const char *debug_name);

void GameFlowLoadGarageScreen(ASYNCFILE_CALLBACK callback, intptr_t param);

#endif
