#ifndef EVENTS_EAIENGINEREV_H
#define EVENTS_EAIENGINEREV_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class EAIEngineRev : public Event {
  public:
    // total size: 0xc
    struct StaticData : public Event::StaticData {
        unsigned int fCarID;      // offset: 0x4, size 0x4
        unsigned int fPatterPlay; // offset: 0x8, size 0x4
    };

    // enum { kEventID = 0 };

    EAIEngineRev(unsigned int pCarID, unsigned int pPatterPlay, unsigned int phSimable);

    ~EAIEngineRev() override;

    const char *GetEventName() override;

  private:
    unsigned int fCarID;      // offset: 0x8, size 0x4
    unsigned int fPatterPlay; // offset: 0xc, size 0x4

    unsigned int fhSimable; // offset: 0xc, size 0x4
};

void EAIEngineRev_MakeEvent_Callback(const void *staticData);

#endif
