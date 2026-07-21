#ifndef ONLINE_MANAGER_HPP
#define ONLINE_MANAGER_HPP

// TODO check #if ONLINE_PLATFORM instead
#if defined(EA_PLATFORM_GAMECUBE)
#elif defined(EA_PLATFORM_XENON)
#define ONLINE_ENABLED
#elif defined(EA_PLATFORM_PLAYSTATION2)
#define ONLINE_ENABLED
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
    void InitQuantizers();
    void Disconnect(bool force);
    void Update(bool networkThread);
    uint32 GetMasterTime();
    void DriverLeft(int driver_number, bool he_quit);

    void EndSimFrame() {}

    void Initialize(int argc, char **argv) {}

    bool IsOnlineRace() {
#ifndef ONLINE_ENABLED
        return false;
#else
        // TODO
        return false;
#endif
    }

    void EndOnlineRace(bool bForced) {
        // TODO
    }

    void TrackLoaded() {
        // TODO
    }

    // TODO
    eOnlineState GetState() {
        return OLS_DISCONNECTED;
    }
};

extern OnlineManager TheOnlineManager;

#endif
