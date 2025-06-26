#pragma once

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

struct GameFlowManager {
    // total size: 0x24
    void (*pSingleFunction)(int);       // offset 0x0, size 0x4
    int SingleFunctionParam;            // offset 0x4, size 0x4
    const char *pSingleFunctionName;    // offset 0x8, size 0x4
    void (*pLoopingFunction)();         // offset 0xC, size 0x4
    const char *pLoopingFunctionName;   // offset 0x10, size 0x4
    bool WaitingForCallback;            // offset 0x14, size 0x1
    const char *pCallbackName;          // offset 0x18, size 0x4
    int CallbackPhase;                  // offset 0x1C, size 0x4
    GameFlowState CurrentGameFlowState; // offset 0x20, size 0x4

    ~GameFlowManager() {}

    void SetSingleFunction(void (*function)(), const char *debug_name) {}

    GameFlowState GetState() {}

    bool IsInFrontend() {
        return this->CurrentGameFlowState == GAMEFLOW_STATE_IN_FRONTEND;
    }

    bool IsInGame() {}

    bool IsLoading() {}
};

extern GameFlowManager TheGameFlowManager; // size: 0x24

inline bool IsGameFlowInFrontEnd() {
    return TheGameFlowManager.IsInFrontend();
}
