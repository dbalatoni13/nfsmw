#ifndef WORLD_WTRIGGER_H
#define WORLD_WTRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/Event.h"
#include "Speed/Indep/Src/World/WTriggerList.h"

class WTrigger {
  public:
    bool HasEvent(unsigned int eventID, const EventStaticData **foundEvent) const;
};

// total size: 0x8
struct FireOnExitRec {
    class WTrigger &mTrigger; // offset 0x0, size 0x4
    HSIMABLE mhSimable;       // offset 0x4, size 0x4
};

// total size: 0x10
class FireOnExitList : public std::set<FireOnExitRec> {};

// total size: 0x10
class WTriggerManager {
  public:
    void Update(float dT);
    void GetIntersectingTriggers(const UMath::Vector3 &pt, float radius, WTriggerList *triggerList) const;

    static WTriggerManager &Get() {
        return *fgTriggerManager;
    }

  private:
    static WTriggerManager *fgTriggerManager;

    bool fSilencableEnabled;          // offset 0x0, size 0x1
    int fProcessingStimulus;          // offset 0x4, size 0x4
    FireOnExitList *fgFireOnExitList; // offset 0x8, size 0x4
    unsigned short fIterCount;        // offset 0xC, size 0x2
};

#endif
