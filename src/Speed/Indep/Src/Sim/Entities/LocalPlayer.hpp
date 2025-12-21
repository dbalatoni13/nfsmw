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
    void SetRenderPort(int renderport) override {
        mRenderPort = renderport;
    }

    int GetRenderPort() const override {
        return mRenderPort;
    }

    PlayerSettings *GetSettings() const override;

    int GetSettingsIndex() const override {
        return mSettingIndex;
    }

    void SetSettings(int fe_index) override {
        mSettingIndex = fe_index;
    }

    IHud *GetHud() const override {
        return mHud;
    }

    void SetHud(ePlayerHudType ht) override;
    void SetControllerPort(int port) override;

    int GetControllerPort() const override {
        return mControllerPort;
    }

    IFeedback *GetFFB() override;
    ISteeringWheel *GetSteeringDevice() override;

    // IEntity
    ISimable *GetSimable() const override {
        return Sim::Entity::GetSimable();
    }

    bool IsLocal() const override {
        return true;
    }

    const UMath::Vector3 &GetPosition() const override {
        return Sim::Entity::GetPosition();
    }

    bool SetPosition(const UMath::Vector3 &position) override {
        return Sim::Entity::SetPosition(position);
    }

    // IAttachable
    void OnAttached(IAttachable *pOther) override;
    void OnDetached(IAttachable *pOther) override;

    // IPlayer
    bool InGameBreaker() const override {
        return mInGameBreaker;
    }

    bool CanRechargeNOS() const override;
    void ResetGameBreaker(bool full) override;

    void ChargeGameBreaker(float amount) override {
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
#ifndef EA_PLATFORM_PLAYSTATION2
    HACTIVITY mLastPursuit; // offset 0x90, size 0x4
#endif
};

#endif
