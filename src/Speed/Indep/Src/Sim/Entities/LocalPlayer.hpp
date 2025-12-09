#ifndef SIM_ENTITIES_LOCALPLAYER_H
#define SIM_ENTITIES_LOCALPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Sim/SimEntity.h"

// total size: 0x94
class LocalPlayer : public Sim::Entity, public IPlayer, public Sim::Collision::IListener {
  public:
    LocalPlayer(Sim::Param params);

    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    // Overrides
    // IPlayer
    override virtual void SetRenderPort(int renderport) {
        mRenderPort = renderport;
    }

    override virtual int GetRenderPort() const {
        return mRenderPort;
    }

    PlayerSettings *GetSettings() const override;

    override virtual int GetSettingsIndex() const {
        return mSettingIndex;
    }

    override virtual void SetSettings(int fe_index) {
        mSettingIndex = fe_index;
    }

    override virtual IHud *GetHud() const {
        return mHud;
    }

    void SetHud(ePlayerHudType ht) override;
    void SetControllerPort(int port) override;

    override virtual int GetControllerPort() const {
        return mControllerPort;
    }

    IFeedback *GetFFB() override;
    ISteeringWheel *GetSteeringDevice() override;

    // IEntity
    override virtual ISimable *GetSimable() const {
        return Sim::Entity::GetSimable();
    }

    override virtual bool IsLocal() const {
        return true;
    }

    override virtual const UMath::Vector3 &GetPosition() const {
        return Sim::Entity::GetPosition();
    }

    override virtual bool SetPosition(const UMath::Vector3 &position) {
        return Sim::Entity::SetPosition(position);
    }

    // IAttachable
    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;

    // IPlayer
    override virtual bool InGameBreaker() const {
        return mInGameBreaker;
    }

    bool CanRechargeNOS() const override;
    void ResetGameBreaker(bool full) override;

    override virtual void ChargeGameBreaker(float amount) {
        mGameBreakerCharge = UMath::Clamp(mGameBreakerCharge + amount, 0.0f, 1.0f);
    }

    bool ToggleGameBreaker() override;

    // IListener
    void OnCollision(const COLLISION_INFO &cinfo) override;

  protected:
    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    // IUnknown
    ~LocalPlayer();

    void ReleaseHud();
    void UpdateHud(float dT);
    void DoGameBreaker(float dT, float dT_real);
    void SetGameBreaker(bool on);
    bool CanDoGameBreaker();
    void DoFFB();
    void DoRadar(bool inPursuit, bool isCoolingDown);
    void UpdateNeighbourhood();

  private:
    bool CanDoFFB() const;

    IFeedback *mFFB;                 // offset 0x60, size 0x4
    ISteeringWheel *mWheelDevice;    // offset 0x64, size 0x4
    int mRenderPort;                 // offset 0x68, size 0x4
    int mControllerPort;             // offset 0x6C, size 0x4
    int mSettingIndex;               // offset 0x70, size 0x4
    const char *mName;               // offset 0x74, size 0x4
    unsigned int mNeighbourhoodHash; // offset 0x78, size 0x4
    IHud *mHud;                      // offset 0x7C, size 0x4
    HSIMTASK mHudTask;               // offset 0x80, size 0x4
    Sim::IActivity *mSpeech;         // offset 0x84, size 0x4
    bool mInGameBreaker;             // offset 0x88, size 0x1
    float mGameBreakerCharge;        // offset 0x8C, size 0x4
    HACTIVITY mLastPursuit;          // offset 0x90, size 0x4
};

#endif
