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

    // Overrides
    // IPlayer
    override virtual void SetRenderPort(int renderport) {
        mRenderPort = renderport;
    }

    override virtual int GetRenderPort() const {
        return mRenderPort;
    }

    override virtual PlayerSettings *GetSettings() const;

    override virtual int GetSettingsIndex() const {
        return mSettingIndex;
    }

    override virtual void SetSettings(int fe_index) {
        mSettingIndex = fe_index;
    }

    override virtual IHud *GetHud() const {
        return mHud;
    }

    override virtual void SetHud(ePlayerHudType ht);
    override virtual void SetControllerPort(int port);

    override virtual int GetControllerPort() const {
        return mControllerPort;
    }

    override virtual IFeedback *GetFFB();
    override virtual ISteeringWheel *GetSteeringDevice();

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
    override virtual void OnAttached(IAttachable *pOther);
    override virtual void OnDetached(IAttachable *pOther);

    // IPlayer
    override virtual bool InGameBreaker() const {
        return mInGameBreaker;
    }

    override virtual bool CanRechargeNOS() const;
    override virtual void ResetGameBreaker(bool full);

    override virtual void ChargeGameBreaker(float amount) {
        mGameBreakerCharge = UMath::Clamp(mGameBreakerCharge + amount, 0.0f, 1.0f);
    }

    override virtual bool ToggleGameBreaker();

    // IListener
    override virtual void OnCollision(const COLLISION_INFO &cinfo);

  protected:
    // ITaskable
    override virtual bool OnTask(HSIMTASK htask, float dT);

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
