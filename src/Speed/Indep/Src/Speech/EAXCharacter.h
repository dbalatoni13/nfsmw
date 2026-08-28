//
//
//
//
//
//
//
//
//
//
//
//
//
#include "Speed/Indep/Src/EAXSound/SND_GEN/P2temp.h"
#ifndef __EAXCHARACTER_H
#define __EAXCHARACTER_H 1

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/EAXSound/SND_GEN/COPSPEECH.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

namespace Speech {

// total size: 0x8
// Decl: 34
struct History {
    // Decl: 35
    History() {
        this->time = Timer(0);
        this->count = 0;
        this->speakers = 0;
    }

    // Decl: 42
    void Touch(unsigned short spkrID) {
        this->time = WorldTimer;
        this->count = this->count + 1;
        if (spkrID < 10 && ((((this->speakers >> spkrID) ^ 1) & 1) != 0)) {
            this->speakers = this->speakers | (1 << spkrID);
        }
    }

    Timer time;              // offset 0x0, size 0x4, Decl: 50
    unsigned short count;    // offset 0x4, size 0x2, Decl: 51
    unsigned short speakers; // offset 0x6, size 0x2, Decl: 52
};

// total size: 0xC
// Decl: 58
class HistoryPair {
  public:
    HistoryPair() : id(kSPCH1_EventID_MaxEventID) {} // Decl: 59

    SPCHType_1_EventID id; // offset 0x0, size 0x4, Decl: 61
    History history;       // offset 0x4, size 0x8, Decl: 62

    // Decl: 63
    bool operator<(const Speech::HistoryPair &from) const {
        return this->id < from.id;
    }
};

}; // namespace Speech

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
    virtual void HeatJump(Csis::Type_heat_level heat);

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

    virtual Csis::Type_code GetRandomizedCode() {}

    virtual void Reset();

  protected:
    int mTimeLastSpoken;        // offset 0x4, size 0x4
    int mLastEvent;             // offset 0x8, size 0x4
    int mSpeakerID;             // offset 0xC, size 0x4
    HSIMABLE mHandle;           // offset 0x10, size 0x4
    Sound::Battalion mCallsign; // offset 0x14, size 0x8
    UMath::Vector3 mPos;        // offset 0x1C, size 0xC
    float mSpeed;               // offset 0x28, size 0x4
    float mDistance;            // offset 0x2C, size 0x4
    float mHealth;              // offset 0x30, size 0x4
    bool mDestroyed;            // offset 0x34, size 0x1
    bool mActive;               // offset 0x38, size 0x1
    bool mSuspectLOS;           // offset 0x3C, size 0x1
};

#endif
