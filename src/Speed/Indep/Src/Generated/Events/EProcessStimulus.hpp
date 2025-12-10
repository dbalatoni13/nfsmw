#ifndef EVENTS_EPROCESSSTIMULUS_H
#define EVENTS_EPROCESSSTIMULUS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Main/Event.h"

// total size: 0x1c
class EProcessStimulus : public Event {
  public:
    // total size: 0x10
    struct StaticData : public Event::StaticData {
        UCrc32 fSystemName; // offset: 0x4, size 0x4
        UCrc32 fStimulus;   // offset: 0x8, size 0x4
        UCrc32 fQueueMode;  // offset: 0xc, size 0x4
    };

    // enum { kEventID = 0 };

    EProcessStimulus(UCrc32 pSystemName, UCrc32 pStimulus, UCrc32 pQueueMode, unsigned int pEventSeqEngine, unsigned int pEventSeqSystem);

    ~EProcessStimulus() override;

    const char *GetEventName() override;

  private:
    UCrc32 fSystemName; // offset: 0x8, size 0x4
    UCrc32 fStimulus;   // offset: 0xc, size 0x4
    UCrc32 fQueueMode;  // offset: 0x10, size 0x4

    unsigned int fEventSeqEngine; // offset: 0x18, size 0x4
    unsigned int fEventSeqSystem; // offset: 0x1c, size 0x4
};

void EProcessStimulus_MakeEvent_Callback(const void *staticData);

#endif
