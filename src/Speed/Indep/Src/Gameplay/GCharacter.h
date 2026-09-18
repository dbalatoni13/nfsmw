#ifndef GAMEPLAY_GCHARACTER_H
#define GAMEPLAY_GCHARACTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRuntimeInstance.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"

class GMarker;

// total size: 0x80
class GCharacter : public GRuntimeInstance, public UTL::COM::Object, public IAttachable {
  public:
    // GRuntimeInstance y UTL::COM::Object declaran los dos operator new; en el
    // original Object no lo declara. Se redeclaran aqui para deshacer la
    // ambiguedad sin tocar UCOM.h (cabecera compartida por todo el juego).
    void *operator new(size_t size, void *ptr) {
        return ptr;
    }

    void operator delete(void *mem, void *ptr) {
    }

    USE_FASTALLOC(GCharacter);

    enum State {
        kCharState_Invalid = 0,
        kCharState_Unspawned = 1,
        kCharState_Spawning_WaitingForModel = 2,
        kCharState_Spawning_WaitingForTrack = 3,
        kCharState_Spawned = 4,
        kCharState_Unspawning_WaitingUntilOffscreen = 5,
    };

    GameplayObjType GetType() const override {
        return kGameplayObjType_Character;
    }

    bool HasStockCar() const {
        return mVehicle != NULL && IsFlagSet(kCharFlag_UsingStockCar);
    }

    GCharacter(const Attrib::Key &triggerKey);
    ~GCharacter() override;

    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;

    void Spawn(const UMath::Vector3 &pos, const UMath::Vector3 &dir, GMarker *targetPoint, float initialSpeed);
    bool SpawnPending() const;
    bool IsSpawned() const;
    void ReleaseVehicle();
    void Unspawn();
    void UnspawnWhenOffscreen();
    bool IsNoLongerUseful() const;
    bool AttemptSpawn();
    IVehicle *GetSpawnedVehicle() const;
    unsigned int GetName() const;

    bool Attach(UTL::COM::IUnknown *pOther) override {
        return mAttachments->Attach(pOther);
    }
    bool Detach(UTL::COM::IUnknown *pOther) override {
        return mAttachments->Detach(pOther);
    }
    bool IsAttached(const UTL::COM::IUnknown *pOther) const override {
        return mAttachments->IsAttached(pOther);
    }
    const List *GetAttachments() const override {
        return &mAttachments->GetList();
    }

  private:
    enum Flags {
        kCharFlag_UsingStockCar = 1,
        kCharFlag_AttachedToManager = 2,
    };

    UMath::Vector3 mSpawnPos;           // offset 0x40, size 0xC
    unsigned char mState;               // offset 0x4C, size 0x1
    unsigned char mFlags;               // offset 0x4D, size 0x1
    unsigned short mCreateAttemptsMade; // offset 0x4E, size 0x2
    UMath::Vector3 mSpawnDir;           // offset 0x50, size 0xC
    float mSpawnSpeed;                  // offset 0x5C, size 0x4
    UMath::Vector3 mTargetPos;          // offset 0x60, size 0xC
    IVehicle *mVehicle;                 // offset 0x6C, size 0x4
    UMath::Vector3 mTargetDir;          // offset 0x70, size 0xC
    Sim::Attachments *mAttachments;     // offset 0x7C, size 0x4

  public:
    void SetFlag(unsigned short flag) {
        mFlags |= flag;
    }

    void ClearFlag(unsigned short flag) {
        mFlags &= ~flag;
    }

    bool IsFlagSet(unsigned short flag) const {
        return (mFlags & flag) != 0;
    }

    bool IsFlagClear(unsigned short flag) const {
        return (mFlags & flag) == 0;
    }
};

DECLARE_CONTAINER_TYPE(ID_GCharacterList);

#endif
