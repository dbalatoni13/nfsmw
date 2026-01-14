#ifndef SPEECH_EAXCHARACTER_H
#define SPEECH_EAXCHARACTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

// TODO figure out where this is, it has smth to do with AnytimeEvents
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

    // Virtual functions
    virtual void Ack();
    virtual void Deny();
    virtual void InterruptStatic();
    virtual void InterruptExpletive();
    virtual void InterruptComposedLow();
    virtual void InterruptComposedHigh();
    virtual void DriverHistory();
    virtual void HeatJump(Type_heat_level heat);

    virtual HSIMABLE GetHandle() {}

    virtual void SetHandle(HSIMABLE handle) {}

    virtual int GetSpeakerID() {}

    virtual int GetCallsign() {}

    virtual int GetUnitNumber() {}

    virtual void SetCallsign(int callsign) {}

    virtual void SetUnitNumber(int unitnum) {}

    virtual void SetSpeakerID(int spkrID) {}

    virtual void SetPosition(const UMath::Vector3 &v) {}

    virtual const UMath::Vector3 GetPosition() {}

    virtual void SetSpeed(const float speed) {}

    virtual void Update();

    virtual float GetDistance() {}

    virtual float GetHealth() {}

    virtual void InterruptViolent();

    virtual bool IsActive() {}

    virtual void SetActive(bool active) {}

    virtual float GetSpeed() {}

    virtual bool IsDead() {}

    virtual bool HasLOS() {}

    virtual void SetLOS(bool yes) {}

    virtual Type_code GetRandomizedCode() {}

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
