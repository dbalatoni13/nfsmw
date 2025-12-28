#ifndef WORLD_ONLINEMANAGER_H
#define WORLD_ONLINEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

enum eOnlineState {
    OLS_DISCONNECTED = 0,
    OLS_LOBBY_IN_LOBBY = 1,
    OLS_RACE_DATA_SYNC = 2,
    OLS_RACE_LOAD_TRACK = 3,
    OLS_RACE_START_LINE = 4,
    OLS_RACING = 5,
    OLS_RACE_END = 6,
    NUM_OLS_STATES = 7,
};

class OnlineManager {
  public:
    void StartSimFrame();

    void EndSimFrame() {}

    void Initialize(int argc, char **argv) {}

    bool IsOnlineRace() {
#if !ONLINE_SUPPORT
        return false;
#else
        return false;
#endif
    }

    // TODO
    eOnlineState GetState() {
        return OLS_DISCONNECTED;
    }
};

extern OnlineManager TheOnlineManager;

#endif
