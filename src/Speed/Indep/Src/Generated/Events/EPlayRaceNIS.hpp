#ifndef EVENTS_EPLAYRACENIS_H
#define EVENTS_EPLAYRACENIS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x24
class EPlayRaceNIS : public Event {
  public:
    // total size: 0x20
    struct StaticData : public Event::StaticData {
        GMarker *fStartMarker;  // offset: 0x4, size 0x4
        const char *fSceneName; // offset: 0x8, size 0x4
        const char *fSceneType; // offset: 0xc, size 0x4
        int fPlayerIdx;         // offset: 0x10, size 0x4
        int fCameraTrack;       // offset: 0x14, size 0x4
        const char *fPreMovie;  // offset: 0x18, size 0x4
        const char *fPostMovie; // offset: 0x1c, size 0x4
    };

    // enum { kEventID = 0 };

    EPlayRaceNIS(GMarker *pStartMarker, const char *pSceneName, const char *pSceneType, int pPlayerIdx, int pCameraTrack, const char *pPreMovie,
                 const char *pPostMovie);

    ~EPlayRaceNIS() override;

    const char *GetEventName() override;

  private:
    GMarker *fStartMarker;  // offset: 0x8, size 0x4
    const char *fSceneName; // offset: 0xc, size 0x4
    const char *fSceneType; // offset: 0x10, size 0x4
    int fPlayerIdx;         // offset: 0x14, size 0x4
    int fCameraTrack;       // offset: 0x18, size 0x4
    const char *fPreMovie;  // offset: 0x1c, size 0x4
    const char *fPostMovie; // offset: 0x20, size 0x4
};

void EPlayRaceNIS_MakeEvent_Callback(const void *staticData);

#endif
