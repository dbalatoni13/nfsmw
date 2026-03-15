#ifndef WORLD_WTRIGGER_H
#define WORLD_WTRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

struct EventList;

// total size: 0x40
struct __attribute__((packed)) Trigger {
    UMath::Vector4 fMatRow0Width;    // offset 0x00, size 0x10
    unsigned int fType : 4;          // offset 0x10:0
    unsigned int fShape : 4;         // offset 0x10:4
    unsigned int fFlags : 24;        // offset 0x11:0
    float fHeight;                   // offset 0x14, size 0x4
    EventList *fEvents;              // offset 0x18, size 0x4
    unsigned short fIterStamp;       // offset 0x1C, size 0x2
    unsigned short fFingerprint;     // offset 0x1E, size 0x2
    UMath::Vector4 fMatRow2Length;   // offset 0x20, size 0x10
    UMath::Vector4 fPosRadius;       // offset 0x30, size 0x10
};

// total size: 0x40
class WTrigger : public Trigger {
  public:
    void *operator new(std::size_t size) { return gFastMem.Alloc(size, nullptr); }
    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    WTrigger(const UMath::Matrix4 *objectmatrix, const UMath::Vector3 *dim, EventList *eventList,
             unsigned int flags);
    ~WTrigger();
    void FireEvents(HSIMABLE__ *simable);
    void UpdateBox(const UMath::Matrix4 *objectmatrix, const UMath::Vector3 *dim);
    void UpdateCylinder(const UMath::Matrix4 *objectmatrix);
    bool UpdatePos(const UMath::Matrix4 *objectmatrix);
    void MakeMatrix(UMath::Matrix4 &matrix) const;
    void GetCenter(UMath::Vector3 &center) const;
    void GetBase(UMath::Vector3 &base) const;
    bool HasEvent(unsigned int eventID) const;

    inline void Enable() { fFlags |= 1; }

    inline void Disable() { fFlags &= ~1; }

    inline bool IsEnabled(bool allowSilencables = true) const {
        if (!(fFlags & 1))
            return false;
        return true;
    }
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
