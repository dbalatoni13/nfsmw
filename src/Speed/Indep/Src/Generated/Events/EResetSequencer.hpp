#ifndef EVENTS_ERESETSEQUENCER_H
#define EVENTS_ERESETSEQUENCER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0xc
class EResetSequencer : public Event {
  public:
    // total size: 0x4
    struct StaticData : public Event::StaticData {};

    // enum { kEventID = 0 };

    EResetSequencer(unsigned int pEventSeqEngine);

    ~EResetSequencer() override;

    const char *GetEventName() override;

  private:
    unsigned int fEventSeqEngine; // offset: 0xc, size 0x4
};

void EResetSequencer_MakeEvent_Callback(const void *staticData);

#endif
