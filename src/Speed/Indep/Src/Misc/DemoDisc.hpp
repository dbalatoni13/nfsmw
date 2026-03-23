#ifndef MISC_DEMODISC_H
#define MISC_DEMODISC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Timer.hpp"

enum DemoDiscEndReason {
    DEMO_DISC_ENDREASON_NONE = -1,
    DEMO_DISC_ENDREASON_ATTRACT_INTERRUPTED = 0,
    DEMO_DISC_ENDREASON_ATTRACT_COMPLETE = 1,
    DEMO_DISC_ENDREASON_PLAYABLE_INACTIVITY_TIMEOUT = 2,
    DEMO_DISC_ENDREASON_PLAYABLE_GAMEPLAY_TIMEOUT = 3,
    DEMO_DISC_ENDREASON_PLAYABLE_COMPLETE = 4,
    DEMO_DISC_ENDREASON_PLAYABLE_QUIT = 5,
};

// total size: 0x28
class DemoDiscManager {
  public:
    DemoDiscManager();
    void Init(int argc, char **argv);
    bool IsAttractMode();
    void SetEndReason(DemoDiscEndReason end_reason);
    void EndDemo();
    void AddExitJoyHandler();
    int GetDemoCarType();

    bool IsActive() {
        return false;
    }

    // inline bool IsStandAlone() {}

    // inline bool CanExitDemo() {}

    // inline void RegisterActivity() {}

    // inline bool HasAnyKeyBeenPressed() {}

    // inline void SetAnyKeyPressed() {}

    DemoDiscEndReason GetEndReason() {
        return nDemoDiscEndReason;
    }

    // inline bool HasPlayTimeExpired() {}

    void StartPlayTimer() {
        StartPlayTime = RealTimer;
        SuspendedPlayTime.UnSet();
    }

    // inline bool IsPlayTimerSuspended() {}

    // inline void InvalidateTimeouts() {}

    // inline int GetNumDemoLoops() {}

    // inline void IncrementNumDemoLoops() {}

    unsigned short GetMasterVolumeScale() {
        return MasterVolumeScale;
    }

    // inline int GetLanguage() {}

  private:
    Timer StartPlayTime;                       // offset 0x0, size 0x4
    Timer LastActivityTime;                    // offset 0x4, size 0x4
    Timer SuspendedPlayTime;                   // offset 0x8, size 0x4
    enum DemoDiscEndReason nDemoDiscEndReason; // offset 0xC, size 0x4
    int NumDemoLoops;                          // offset 0x10, size 0x4
    unsigned short Language;                   // offset 0x14, size 0x2
    unsigned short Aspect;                     // offset 0x16, size 0x2
    unsigned short PlayMode;                   // offset 0x18, size 0x2
    unsigned short InactiveTimeout;            // offset 0x1A, size 0x2
    unsigned short TotalTimeout;               // offset 0x1C, size 0x2
    unsigned short MasterVolumeScale;          // offset 0x1E, size 0x2
    bool bCalledSCEADemoStart;                 // offset 0x20, size 0x1
    bool bAnyKeyPressed;                       // offset 0x24, size 0x1
};

extern DemoDiscManager TheDemoDiscManager;

#endif
