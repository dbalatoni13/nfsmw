#ifndef GAMEPLAY_GICON_H
#define GAMEPLAY_GICON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class EmitterGroup;
class WorldModel;

// total size: 0x20
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

    // total size: 0xC
    struct EffectInfo {
        unsigned int mType;         // offset 0x0, size 0x4
        unsigned int mModelHash;    // offset 0x4, size 0x4
        unsigned int mParticleHash; // offset 0x8, size 0x4
    };

    USE_FASTALLOC(GIcon);

    GIcon(Type type, const UMath::Vector3 &position, float rotation);
    ~GIcon();

    void Spawn();
    void Unspawn();
    void FindSection();
    void SnapToGround();
    static void NotifyEmitterGroupDelete(void *obj, EmitterGroup *group);
    EmitterGroup *CreateParticleEffect(unsigned int particleHash);
    void ReleaseParticleEffect();
    void RefreshEffects();
    WorldModel *CreateGeometry(unsigned int modelHash);
    void ReleaseGeometry();
    void SetPosition();
    void Enable();
    void Disable();

    void Show() { SetFlag(0x1); }
    void Hide() { ClearFlag(0x1); }
    inline void HideUntilRespawn();
    void ShowOnMap() { SetFlag(0x2); }
    void HideOnMap() { ClearFlag(0x2); }
    void SetGPSing() { SetFlag(0x80); }
    void ClearGPSing() { ClearFlag(0x80); }
    bool IsGPSing() const { return IsFlagSet(0x80); }
    Type GetType() const {
        return (Type)mType;
    }
    int GetSectionID() const {
        return mSectionID;
    }
    int GetCombinedSectionID() const {
        return mCombSectionID;
    }
    bool GetVisibleInWorld() const {
        return IsFlagSet(0x1);
    }
    bool GetVisibleOnMap() const {
        return GetVisibleInWorld() && IsFlagSet(0x2);
    }
    bool GetIsDisposable() const {
        return IsFlagSet(0x10);
    }
    inline bool GetIsSnapped() const;
    inline bool GetIsGPSing() const;
    const UMath::Vector3 &GetPosition() const {
        return mPosition;
    }
    // Declarada y nunca definida: el bl salia a un simbolo que no existe en el
    // ELF. El objetivo la expande (lfs 0x10/0x14 -> stfs en el bVector2).
    void GetPosition2D(struct bVector2 &outPos) {
        outPos.x = mPosition.x;
        outPos.y = mPosition.y;
    }
    void MarkDisposable() {
        SetFlag(0x10);
    }
    bool GetIsEnabled() const {
        return IsFlagSet(0x8);
    }
    void SetFlag(unsigned int mask) {
        mFlags |= mask;
    }

    void ClearFlag(unsigned int mask) {
        mFlags &= ~mask;
    }

    bool IsFlagSet(unsigned int mask) const {
        return (mFlags & mask) != 0;
    }

    bool IsFlagClear(unsigned int mask) const {
        return (mFlags & mask) == 0;
    }

    static EffectInfo kEffectInfo[];

  private:
    unsigned short mType;        // offset 0x0, size 0x2
    unsigned short mFlags;       // offset 0x2, size 0x2
    short mSectionID;            // offset 0x4, size 0x2
    short mCombSectionID;        // offset 0x6, size 0x2
    WorldModel *mModel;          // offset 0x8, size 0x4
    EmitterGroup *mEmitter;      // offset 0xC, size 0x4
    UMath::Vector3 mPosition;    // offset 0x10, size 0xC
    unsigned short mRotation;    // offset 0x1C, size 0x2
    unsigned short mPad;         // offset 0x1E, size 0x2
};

#endif
