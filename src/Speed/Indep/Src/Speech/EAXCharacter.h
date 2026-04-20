#ifndef SPEECH_EAXCHARACTER_H
#define SPEECH_EAXCHARACTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

namespace Csis {
enum Type_heat_level {
    Type_heat_level_2 = 1,
    Type_heat_level_3 = 2,
    Type_heat_level_4 = 4,
    Type_heat_level_5 = 8,
    Type_heat_level_6 = 16,
    Type_heat_level_7 = 32,
    Type_heat_level_8 = 64,
    Type_heat_level_9 = 128,
    Type_heat_level_10 = 256,
};
}
typedef Csis::Type_heat_level Type_heat_level;

enum Type_code {
    Type_code_Use_10_code = 1,
    Type_code_dont_use_10_code = 2,
};

// TODO where to put this?
// total size: 0x8
struct Battalion {
    Battalion(int nam, int num) {}

    int name;   // offset 0x0, size 0x4
    int number; // offset 0x4, size 0x4
};

// total size: 0x40
class EAXCharacter : public AudioMemBase {
  public:
    EAXCharacter(int sID, HSIMABLE wID, int bID, int cID);
    virtual ~EAXCharacter();

    static void *operator new(unsigned int size);
    static void operator delete(void *ptr);

    // Virtual functions
    virtual void Ack();
    virtual void Deny();
    virtual void InterruptStatic();
    virtual void InterruptExpletive();
    virtual void InterruptViolent();
    virtual void InterruptComposedLow();
    virtual void InterruptComposedHigh();
    virtual void DriverHistory();
    virtual void HeatJump(Csis::Type_heat_level heat);

    virtual HSIMABLE GetHandle() {
        return mHandle;
    }

    virtual void SetHandle(HSIMABLE handle) {
        mHandle = handle;
    }

    virtual int GetSpeakerID() {
        return mSpeakerID;
    }

    virtual int GetCallsign() {
        return mCallsign.name;
    }

    virtual int GetUnitNumber() {
        return mCallsign.number;
    }

    virtual void SetCallsign(int callsign) {
        mCallsign.name = callsign;
    }

    virtual void SetUnitNumber(int unitnum) {
        mCallsign.number = unitnum;
    }

    virtual void SetSpeakerID(int spkrID) {
        mSpeakerID = spkrID;
    }

    virtual void SetPosition(const UMath::Vector3 &v) {
        mPos = v;
    }

    virtual const UMath::Vector3 GetPosition() {
        return mPos;
    }

    virtual void SetSpeed(const float speed) {
        mSpeed = speed;
    }

    virtual void Update();

    virtual float GetDistance() {
        return mDistance;
    }

    virtual float GetHealth() {
        return mHealth;
    }

    virtual int IsActive() {
        return *reinterpret_cast<unsigned int *>(&mActive);
    }

    virtual void SetActive(bool active) {
        *reinterpret_cast<unsigned int *>(&mActive) = active;
    }

    virtual float GetSpeed() {
        return mSpeed;
    }

    virtual int IsDead() {
        return *reinterpret_cast<unsigned int *>(&mDestroyed);
    }

    virtual int HasLOS() {
        return *reinterpret_cast<unsigned int *>(&mSuspectLOS);
    }

    virtual void SetLOS(bool yes) {
        *reinterpret_cast<unsigned int *>(&mSuspectLOS) = yes;
    }

    virtual Type_code GetRandomizedCode() {
        return Type_code_dont_use_10_code;
    }

    virtual void Reset();

  protected:
    int mTimeLastSpoken; // offset 0x4, size 0x4
    int mLastEvent;      // offset 0x8, size 0x4
    int mSpeakerID;      // offset 0xC, size 0x4
    HSIMABLE mHandle;    // offset 0x10, size 0x4
    Battalion mCallsign; // offset 0x14, size 0x8
    UMath::Vector3 mPos; // offset 0x1C, size 0xC
    float mSpeed;        // offset 0x28, size 0x4
    float mDistance;     // offset 0x2C, size 0x4
    float mHealth;       // offset 0x30, size 0x4
    bool mDestroyed;     // offset 0x34, size 0x1
    bool mActive;        // offset 0x38, size 0x1
    bool mSuspectLOS;    // offset 0x3C, size 0x1
};

#endif
