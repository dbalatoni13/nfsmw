#ifndef WORLD_WTRIGGER_H
#define WORLD_WTRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

struct EventList;
struct EventStaticData;

// total size: 0x40
struct Trigger {
    bool ValidateMatrix() const;
    void MakeMatrix(UMath::Matrix4 &m, bool addXLate) const;

    UMath::Vector4 fMatRow0Width; // offset 0x0, size 0x10
    unsigned int fType : 4;       // offset 0x10, size 0x4
    unsigned int fShape : 4;      // offset 0x10, size 0x4
    unsigned int fFlags : 24;     // offset 0x10, size 0x4
    float fHeight;                // offset 0x14, size 0x4
    EventList *fEvents;           // offset 0x18, size 0x4
    unsigned short fIterStamp;    // offset 0x1C, size 0x2
    unsigned short fFingerprint;  // offset 0x1E, size 0x2
    UMath::Vector4 fMatRow2Length; // offset 0x20, size 0x10
    UMath::Vector4 fPosRadius;     // offset 0x30, size 0x10
};

// total size: 0x40
struct WTrigger : public Trigger {
    ~WTrigger();
    bool UpdatePos(const UMath::Vector3 &newPos, unsigned int triggerInd);

    static void operator delete(void *mem, unsigned int size);
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
    static void Init();
    static void Shutdown();
    void Update(float dT);

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
