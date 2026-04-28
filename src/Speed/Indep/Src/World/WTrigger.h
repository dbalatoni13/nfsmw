#ifndef WORLD_WTRIGGER_H
#define WORLD_WTRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

namespace CARP {
struct EventStaticData;
struct EventList;
}

struct IRigidBody;
struct WTrigger;

// total size: 0x40
struct Trigger {
    bool ValidateMatrix() const;
    void MakeMatrix(UMath::Matrix4 &m, bool addXLate) const;

    UMath::Vector4 fMatRow0Width; // offset 0x0, size 0x10
    unsigned int fType : 4;       // offset 0x10, size 0x4
    unsigned int fShape : 4;      // offset 0x10, size 0x4
    unsigned int fFlags : 24;     // offset 0x10, size 0x4
    float fHeight;                // offset 0x14, size 0x4
    CARP::EventList *fEvents;           // offset 0x18, size 0x4
    unsigned short fIterStamp;    // offset 0x1C, size 0x2
    unsigned short fFingerprint;  // offset 0x1E, size 0x2
    UMath::Vector4 fMatRow2Length; // offset 0x20, size 0x10
    UMath::Vector4 fPosRadius;     // offset 0x30, size 0x10
};

// total size: 0x40
struct WTrigger : public Trigger {
    WTrigger();
    WTrigger(const UMath::Matrix4 &boxMat, const UMath::Vector3 &center, CARP::EventList *events, unsigned int type);
    ~WTrigger();
    bool HasEvent(unsigned int eventID, const CARP::EventStaticData** foundEvent) const;
    bool TestDirection(const UMath::Vector3& vec) const;
    void FireEvents(HSIMABLE__ *hSimable);
    void UpdateBox(const UMath::Matrix4& boxMat, const UMath::Vector3& center);
    bool UpdatePos(const UMath::Vector3 &newPos, unsigned int triggerInd);

    void Enable() { fFlags |= 1; }
    void Disable() { fFlags &= ~1; }
    bool IsEnabled() const { return (fFlags & 1) != 0; }

    bool IsEnabled(bool allowSilencables) const {
        unsigned int flags = (static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(this)[0x11]) << 16)
                           | (static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(this)[0x12]) << 8)
                           | static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(this)[0x13]);
        if (flags & 1) {
            if ((flags & 0x400) && !allowSilencables) {
                return false;
            }
            return true;
        }
        return false;
    }

    void MakeMatrix(UMath::Matrix4 &m, bool addXLate, bool frombase) const {
        m[0][0] = fMatRow0Width.x;
        m[0][1] = fMatRow0Width.y;
        m[0][2] = fMatRow0Width.z;
        m[0][3] = 0.0f;
        if ((static_cast<unsigned int>(reinterpret_cast<const unsigned char *>(this)[0x12]) << 8) & 0x1000) {
            m[1][0] = fMatRow2Length.y * fMatRow0Width.z - fMatRow2Length.z * fMatRow0Width.y;
            m[1][1] = fMatRow2Length.z * fMatRow0Width.x - fMatRow2Length.x * fMatRow0Width.z;
            m[1][2] = fMatRow2Length.x * fMatRow0Width.y - fMatRow2Length.y * fMatRow0Width.x;
        } else {
            m[1][0] = 0.0f;
            m[1][1] = 1.0f;
            m[1][2] = 0.0f;
        }
        m[1][3] = 0.0f;
        m[2][0] = fMatRow2Length.x;
        m[2][1] = fMatRow2Length.y;
        m[2][2] = fMatRow2Length.z;
        m[2][3] = 0.0f;
        if (addXLate) {
            m[3][0] = fPosRadius.x;
            m[3][2] = fPosRadius.z;
            if (frombase) {
                m[3][1] = fPosRadius.y - fHeight * 0.5f;
            } else {
                m[3][1] = fPosRadius.y;
            }
        } else {
            m[3][0] = 0.0f;
            m[3][1] = 0.0f;
            m[3][2] = 0.0f;
        }
        m[3][3] = 1.0f;
    }

    static void operator delete(void *mem, unsigned int size) { if (mem) gFastMem.Free(mem, size, nullptr); }
};

struct WTriggerList : public UTL::Std::vector<WTrigger *, _type_vector> {};

struct FireOnExitRec {
    FireOnExitRec(WTrigger &trigger, HSIMABLE__ *hSimable) : mTrigger(trigger)
        , mhSimable(hSimable) {}

    bool operator==(const FireOnExitRec &rhs) const {
        return &mTrigger == &rhs.mTrigger && mhSimable == rhs.mhSimable;
    }

    bool operator<(const FireOnExitRec &rhs) const {
        if (mhSimable < rhs.mhSimable) return true;
        return &mTrigger < &rhs.mTrigger;
    }

    WTrigger &mTrigger; // offset 0x0, size 0x4
    HSIMABLE__ *mhSimable; // offset 0x4, size 0x4
};

// total size: 0x10
class FireOnExitList : public std::set<FireOnExitRec> {
  public:
    static void *operator new(unsigned int size) { return gFastMem.Alloc(size, nullptr); }
    static void operator delete(void *mem, unsigned int size) { if (mem) gFastMem.Free(mem, size, nullptr); }
};

// total size: 0x10
class WTriggerManager {
  public:
    static void *operator new(unsigned int size) { return gFastMem.Alloc(size, nullptr); }
    static void operator delete(void *mem, unsigned int size) { if (mem) gFastMem.Free(mem, size, nullptr); }

    WTriggerManager();
    ~WTriggerManager();

    static void Init();
    static void Shutdown();
    static void Restart();
    static bool Exists() { return fgTriggerManager != nullptr; }

    static WTriggerManager &Get() {
        return *fgTriggerManager;
    }

    void EnableSilencables() { fSilencableEnabled = true; }
    void DisableSilencables() { fSilencableEnabled = false; }
    int GetCurrentStimulus() const { return fProcessingStimulus; }

    void SubmitForFire(WTrigger &trig, HSIMABLE__ *hSimable);
    void ProcessRB(IRigidBody *rBody, float dT);
    void ProcessSRB(IRigidBody *srBody, float dT);
    bool CheckCollideRB(const IRigidBody *rBody, const WTrigger *trig, float dT) const;
    bool CheckCollideSRB(const IRigidBody *srBody, const WTrigger *trig, float dT) const;
    void GetIntersectingTriggers(const UMath::Vector3 &pt, float radius, WTriggerList *triggerList) const;
    void DeleteRefs(const WTrigger *trig);
    void ClearAllFireOnExit();
    void Update(float dT);

    static WTriggerManager *fgTriggerManager;

    bool fSilencableEnabled;          // offset 0x0, size 0x1
    int fProcessingStimulus;          // offset 0x4, size 0x4
    FireOnExitList *fgFireOnExitList; // offset 0x8, size 0x4
    mutable unsigned short fIterCount;        // offset 0xC, size 0x2
};

#endif
