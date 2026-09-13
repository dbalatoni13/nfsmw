#ifndef GICON_H__
#define GICON_H__

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

struct EmitterGroup;
struct WorldModel;

class GIcon {
  public:
    enum Type {
        kType_Invalid = 0,
        kType_RaceSprint = 1,
        kType_RaceCircuit = 2,
        kType_RaceDrag = 3,
        kType_RaceKnockout = 4,
        kType_RaceTollbooth = 5,
        kType_RaceSpeedtrap = 6,
        kType_RaceRival = 7,
        kType_GateSafehouse = 8,
        kType_GateCarLot = 9,
        kType_GateCustomShop = 10,
        kType_HidingSpot = 11,
        kType_PursuitBreaker = 12,
        kType_SpeedTrap = 13,
        kType_SpeedTrapInRace = 14,
        kType_AreaUnlock = 15,
        kType_Checkpoint = 16,
        kType_Count = 17,
    };

    void Show() {}             // Decl: 62
    void Hide() {}             // Decl: 63
    void HideUntilRespawn() {} // Decl: 64

    void ShowOnMap() {} // Decl: 66
    void HideOnMap() {} // Decl: 67

    void SetGPSing() { // Decl: 70
        SetFlag(0x80);
    }

    void ClearGPSing() { // Decl: 71
        ClearFlag(0x80);
    }

    void SnapToGround(); // Decl: 73

    Type GetType() const { // Decl: 75
        return static_cast<Type>(mType);
    }
    int GetSectionID() const { // Decl: 76
        return mSectionID;
    }
    int GetCombinedSectionID() const { // Decl: 77
        return mCombSectionID;
    }

    bool GetVisibleInWorld() const {} // Decl: 78
    bool GetVisibleOnMap() const {}   // Decl: 79
    bool GetIsDisposable() const {}   // Decl: 80
    bool GetIsSnapped() const {}      // Decl: 81
    bool GetIsGPSing() const {        // Decl: 82
        return IsFlagSet(0x80);
    }

    const UMath::Vector3 &GetPosition() const { // Decl: 84
        return mPosition;
    }
    void GetPosition2D(bVector2 &outPos) { // Decl: 85
        outPos.x = mPosition.x;
        outPos.y = mPosition.y;
    }

    struct EffectInfo { // Decl: 104
        Type mType;
        uint32 mModelHash;
        uint32 mParticleHash;
    };

    GIcon(Type type, const UMath::Vector3 &pos, float rotDeg); // Decl: 143

  private:
    ~GIcon(); // Decl: 144

    void MarkDisposable() {} // Decl: 146

    bool GetIsEnabled() const {} // Decl: 147

    void Spawn(); // Decl: 149

    void Unspawn(); // Decl: 150

    void Enable(); // Decl: 152

    void Disable(); // Decl: 153

    void FindSection(); // Decl: 155

    void RefreshEffects(); // Decl: 156

    void SetFlag(unsigned int mask) { // Decl: 160
        mFlags |= mask;
    };
    void ClearFlag(unsigned int mask) { // Decl: 161
        mFlags &= ~mask;
    }

    bool IsFlagSet(unsigned int mask) const { // Decl: 162
        return (mFlags & mask) != 0;
    }

    bool IsFlagClear(unsigned int mask) const { // Decl: 163
        return (mFlags & mask) == 0;
    }

    struct AcidEffect *CreateParticleEffect(uint32 particleHash); // Decl: 165

    void ReleaseParticleEffect(); // Decl: 166

    struct WorldModel *CreateGeometry(uint32 modelHash); // Decl: 168

    void ReleaseGeometry(); // Decl: 169

    void SetPosition(); // Decl: 171

    static void NotifyEmitterGroupDelete(void *obj, EmitterGroup *group);

  private:
    uint16 mType; // offset 0x0, size 0x2, Decl: 123
    uint16 mFlags;
    int16 mSectionID;
    int16 mCombSectionID;
    WorldModel *mModel;
    EmitterGroup *mEmitter;
    UMath::Vector3 mPosition;
    bAngle mRotation;
    uint16 mPad;

    static EffectInfo kEffectInfo[];
};

#endif
