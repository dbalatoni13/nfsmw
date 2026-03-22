#ifndef GAMEPLAY_GICON_H
#define GAMEPLAY_GICON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"

#include <new>

struct EmitterGroup;
struct WorldModel;
struct bVector2;

// total size: 0x20
struct GIcon {
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

    struct EffectInfo {
        unsigned int mType;         // offset 0x0, size 0x4
        unsigned int mModelHash;    // offset 0x4, size 0x4
        unsigned int mParticleHash; // offset 0x8, size 0x4
    };

    void Show();
    void Hide();
    void HideUntilRespawn();
    void ShowOnMap();
    void HideOnMap();
    void SetGPSing();
    void ClearGPSing();
    Type GetType() const;
    int GetSectionID() const;
    int GetCombinedSectionID() const;
    bool GetVisibleInWorld() const;
    bool GetVisibleOnMap() const;
    bool GetIsDisposable() const;
    bool GetIsSnapped() const;
    bool GetIsGPSing() const;
    const UMath::Vector3 &GetPosition() const;
    void GetPosition2D(bVector2 &outPos);

    static void *operator new(unsigned int size);
    static void operator delete(void *mem, unsigned int size);
    static void *operator new(unsigned int size, const char *name);
    static void operator delete(void *mem, const char *name);
    static void operator delete(void *mem, unsigned int size, const char *name);

    void MarkDisposable();
    bool GetIsEnabled() const;
    void SetFlag(unsigned int mask);
    void ClearFlag(unsigned int mask);
    bool IsFlagSet(unsigned int mask) const;
    bool IsFlagClear(unsigned int mask) const;

    GIcon(Type type, const UMath::Vector3 &pos, float rotDeg);
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

    static EffectInfo kEffectInfo[];

  private:
    enum Flags {
        kFlag_VisibleInWorld = 0x1,
        kFlag_VisibleOnMap = 0x2,
        kFlag_Spawned = 0x4,
        kFlag_Enabled = 0x8,
        kFlag_Disposable = 0x10,
        kFlag_Snapped = 0x20,
        kFlag_ShowWhenSpawned = 0x40,
        kFlag_GPSing = 0x80,
    };

    unsigned short mType;     // offset 0x0, size 0x2
    unsigned short mFlags;    // offset 0x2, size 0x2
    short mSectionID;         // offset 0x4, size 0x2
    short mCombSectionID;     // offset 0x6, size 0x2
    WorldModel *mModel;       // offset 0x8, size 0x4
    EmitterGroup *mEmitter;   // offset 0xC, size 0x4
    UMath::Vector3 mPosition; // offset 0x10, size 0xC
    unsigned short mRotation; // offset 0x1C, size 0x2
    unsigned short mPad;      // offset 0x1E, size 0x2
};

inline void GIcon::Show() {
    SetFlag(kFlag_VisibleInWorld);
}

inline void GIcon::Hide() {
    ClearFlag(kFlag_VisibleInWorld);
}

inline void GIcon::HideUntilRespawn() {
    if (IsFlagSet(kFlag_Spawned)) {
        Hide();
    } else {
        SetFlag(kFlag_ShowWhenSpawned);
    }
}

inline void GIcon::ShowOnMap() {
    SetFlag(kFlag_VisibleOnMap);
}

inline void GIcon::HideOnMap() {
    ClearFlag(kFlag_VisibleOnMap);
}

inline void GIcon::SetGPSing() {
    SetFlag(kFlag_GPSing);
}

inline void GIcon::ClearGPSing() {
    ClearFlag(kFlag_GPSing);
}

inline GIcon::Type GIcon::GetType() const {
    return static_cast<Type>(mType);
}

inline int GIcon::GetSectionID() const {
    return mSectionID;
}

inline int GIcon::GetCombinedSectionID() const {
    return mCombSectionID;
}

inline bool GIcon::GetVisibleInWorld() const {
    return IsFlagSet(kFlag_VisibleInWorld);
}

inline bool GIcon::GetVisibleOnMap() const {
    return IsFlagSet(kFlag_VisibleOnMap);
}

inline bool GIcon::GetIsDisposable() const {
    return IsFlagSet(kFlag_Disposable);
}

inline bool GIcon::GetIsSnapped() const {
    return IsFlagSet(kFlag_Snapped);
}

inline bool GIcon::GetIsGPSing() const {
    return IsFlagSet(kFlag_GPSing);
}

inline const UMath::Vector3 &GIcon::GetPosition() const {
    return mPosition;
}

inline void GIcon::GetPosition2D(bVector2 &outPos) {
    outPos.x = mPosition.x;
    outPos.y = mPosition.z;
}

inline void *GIcon::operator new(unsigned int size) {
    return ::operator new(size);
}

inline void GIcon::operator delete(void *mem, unsigned int) {
    ::operator delete(mem);
}

inline void *GIcon::operator new(unsigned int size, const char *) {
    return ::operator new(size);
}

inline void GIcon::operator delete(void *mem, const char *) {
    ::operator delete(mem);
}

inline void GIcon::operator delete(void *mem, unsigned int, const char *) {
    ::operator delete(mem);
}

inline void GIcon::MarkDisposable() {
    SetFlag(kFlag_Disposable);
}

inline bool GIcon::GetIsEnabled() const {
    return IsFlagSet(kFlag_Enabled);
}

inline void GIcon::SetFlag(unsigned int mask) {
    mFlags = static_cast<unsigned short>(mFlags | mask);
}

inline void GIcon::ClearFlag(unsigned int mask) {
    mFlags = static_cast<unsigned short>(mFlags & ~mask);
}

inline bool GIcon::IsFlagSet(unsigned int mask) const {
    return (mFlags & mask) != 0;
}

inline bool GIcon::IsFlagClear(unsigned int mask) const {
    return (mFlags & mask) == 0;
}

#endif
