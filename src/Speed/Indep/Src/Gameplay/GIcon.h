#ifndef GAMEPLAY_GICON_H
#define GAMEPLAY_GICON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

struct EmitterGroup;
struct WorldModel;

struct GIcon {
    enum Type {
        kType_Invalid = 0, kType_RaceSprint = 1, kType_RaceCircuit = 2,
        kType_RaceDrag = 3, kType_RaceKnockout = 4, kType_RaceTollbooth = 5,
        kType_RaceSpeedtrap = 6, kType_RaceRival = 7, kType_GateSafehouse = 8,
        kType_GateCarLot = 9, kType_GateCustomShop = 10, kType_HidingSpot = 11,
        kType_PursuitBreaker = 12, kType_SpeedTrap = 13, kType_SpeedTrapInRace = 14,
        kType_AreaUnlock = 15, kType_Checkpoint = 16, kType_Count = 17,
    };
    struct EffectInfo { unsigned int mType; unsigned int mModelHash; unsigned int mParticleHash; };
    unsigned short mType;
    unsigned short mFlags;
    short mSectionID;
    short mCombSectionID;
    WorldModel* mModel;
    EmitterGroup* mEmitter;
    UMath::Vector3 mPosition;
    unsigned short mRotation;
    unsigned short mPad;
    static EffectInfo kEffectInfo[];
    void SetFlag(unsigned int mask);
    void ClearFlag(unsigned int mask);
    bool IsFlagSet(unsigned int mask) const;
    bool IsFlagClear(unsigned int mask) const { return (mFlags & mask) == 0; }
    void SetGPSing() { SetFlag(0x80); }
    void ClearGPSing() { ClearFlag(0x80); }
    bool GetIsGPSing() const { return IsFlagSet(0x80); }
    Type GetType() const { return static_cast< Type >(mType); }
    int GetSectionID() const { return mSectionID; }
    int GetCombinedSectionID() const { return mCombSectionID; }
    const UMath::Vector3& GetPosition() const { return mPosition; }
    void GetPosition2D(bVector2& outPos) { outPos.x = mPosition.x; outPos.y = mPosition.y; }
    GIcon(Type type, const UMath::Vector3& pos, float rotDeg);
    ~GIcon();
    void Spawn();
    void Unspawn();
    void FindSection();
    void SnapToGround();
    void Enable();
    void Disable();
};

#endif
