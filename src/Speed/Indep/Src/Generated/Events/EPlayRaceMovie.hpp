#ifndef EVENTS_EPLAYRACEMOVIE_H
#define EVENTS_EPLAYRACEMOVIE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EPlayRaceMovie : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        const char * fMovieName; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EPlayRaceMovie(const char * pMovieName);

    override virtual ~EPlayRaceMovie();

    override virtual const char *GetEventName() {
        return "EPlayRaceMovie";
    }

  private:
    const char * fMovieName; // offset: 0x8, size 0x4
};

void EPlayRaceMovie_MakeEvent_Callback(const void *staticData);

#endif
