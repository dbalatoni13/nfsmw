#ifndef GICON_H__
#define GICON_H__

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

struct EmitterGroup;
struct WorldModel;

class GIcon {
  public:
    typedef enum {
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
        kType_Checkpoint = 14,
        kType_Count = 15
    } Type;

    void Show() {}             // Decl: speed/indep/src/Gameplay/GIcon.h:62
    void Hide() {}             // Decl: speed/indep/src/Gameplay/GIcon.h:63
    void HideUntilRespawn() {} // Decl: speed/indep/src/Gameplay/GIcon.h:64

    void ShowOnMap() {} // Decl: speed/indep/src/Gameplay/GIcon.h:66
    void HideOnMap() {} // Decl: speed/indep/src/Gameplay/GIcon.h:67

    void SetGPSing() { // Decl: speed/indep/src/Gameplay/GIcon.h:70
        SetFlag(0x80);
    }

    void ClearGPSing() { // Decl: speed/indep/src/Gameplay/GIcon.h:71
        ClearFlag(0x80);
    }

    void SnapToGround(); // Decl: speed/indep/src/Gameplay/GIcon.h:73

    Type GetType() const { // Decl: speed/indep/src/Gameplay/GIcon.h:75
        return static_cast<Type>(mType);
    }
    int GetSectionID() const { // Decl: speed/indep/src/Gameplay/GIcon.h:76
        return mSectionID;
    }
    int GetCombinedSectionID() const { // Decl: speed/indep/src/Gameplay/GIcon.h:77
        return mCombSectionID;
    }

    bool GetVisibleInWorld() const {} // Decl: speed/indep/src/Gameplay/GIcon.h:78
    bool GetVisibleOnMap() const {}   // Decl: speed/indep/src/Gameplay/GIcon.h:79
    bool GetIsDisposable() const {}   // Decl: speed/indep/src/Gameplay/GIcon.h:80
    bool GetIsSnapped() const {}      // Decl: speed/indep/src/Gameplay/GIcon.h:81
    bool GetIsGPSing() const {        // Decl: speed/indep/src/Gameplay/GIcon.h:82
        return IsFlagSet(0x80);
    }

    const UMath::Vector3 &GetPosition() const { // Decl: speed/indep/src/Gameplay/GIcon.h:84
        return mPosition;
    }
    void GetPosition2D(bVector2 &outPos) { // Decl: speed/indep/src/Gameplay/GIcon.h:85
        outPos.x = mPosition.x;
        outPos.y = mPosition.y;
    }

    struct EffectInfo { // Decl: speed/indep/src/Gameplay/GIcon.h:104
        Type mType;
        uint32 mModelHash;
        uint32 mParticleHash;
    };

    GIcon(Type type, const UMath::Vector3 &pos, float rotDeg); // Decl: speed/indep/src/Gameplay/GIcon.h:143

  private:
    ~GIcon(); // Decl: speed/indep/src/Gameplay/GIcon.h:144

    void MarkDisposable() {} // Decl: speed/indep/src/Gameplay/GIcon.h:146

    bool GetIsEnabled() const {} // Decl: speed/indep/src/Gameplay/GIcon.h:147

    void Spawn(); // Decl: speed/indep/src/Gameplay/GIcon.h:149

    void Unspawn(); // Decl: speed/indep/src/Gameplay/GIcon.h:150

    void Enable(); // Decl: speed/indep/src/Gameplay/GIcon.h:152

    void Disable(); // Decl: speed/indep/src/Gameplay/GIcon.h:153

    void FindSection(); // Decl: speed/indep/src/Gameplay/GIcon.h:155

    void RefreshEffects(); // Decl: speed/indep/src/Gameplay/GIcon.h:156

    void SetFlag(unsigned int mask) { // Decl: speed/indep/src/Gameplay/GIcon.h:160
        mFlags |= mask;
    };
    void ClearFlag(unsigned int mask) { // Decl: speed/indep/src/Gameplay/GIcon.h:161
        mFlags &= ~mask;
    }

    bool IsFlagSet(unsigned int mask) const { // Decl: speed/indep/src/Gameplay/GIcon.h:162
        return (mFlags & mask) == mask;
    }

    bool IsFlagClear(unsigned int mask) const { // Decl: speed/indep/src/Gameplay/GIcon.h:163
        return (mFlags & mask) == 0;
    }

    struct AcidEffect *CreateParticleEffect(uint32 particleHash); // Decl: speed/indep/src/Gameplay/GIcon.h:165

    void ReleaseParticleEffect(); // Decl: speed/indep/src/Gameplay/GIcon.h:166

    struct WorldModel *CreateGeometry(uint32 modelHash); // Decl: speed/indep/src/Gameplay/GIcon.h:168

    void ReleaseGeometry(); // Decl: speed/indep/src/Gameplay/GIcon.h:169

    void SetPosition(); // Decl: speed/indep/src/Gameplay/GIcon.h:171

    static void NotifyEmitterGroupDelete(void *obj, EmitterGroup *group);

  private:
    uint16 mType; // offset 0x0, size 0x2, Decl: speed/indep/src/Gameplay/GIcon.h:123
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
