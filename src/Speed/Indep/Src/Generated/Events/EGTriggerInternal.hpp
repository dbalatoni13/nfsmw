#ifndef EVENTS_EGTRIGGERINTERNAL_H
#define EVENTS_EGTRIGGERINTERNAL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x14
class EGTriggerInternal : public Event {
  public:
    // total size: 0x8
    struct StaticData : public Event::StaticData {
        unsigned int fTriggerKey; // offset: 0x4, size 0x4
    };

    // enum { kEventID = 0 };

    EGTriggerInternal(unsigned int pTriggerKey, unsigned int phSimable, int pTriggerStimulus);

    override virtual ~EGTriggerInternal();

    override virtual const char *GetEventName() {
        return "EGTriggerInternal";
    }

  private:
    unsigned int fTriggerKey; // offset: 0x8, size 0x4

    unsigned int fhSimable; // offset: 0xc, size 0x4
    int fTriggerStimulus; // offset: 0x10, size 0x4
};

void EGTriggerInternal_MakeEvent_Callback(const void *staticData);

#endif
