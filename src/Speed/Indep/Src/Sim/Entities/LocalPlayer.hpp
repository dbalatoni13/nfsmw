#ifndef SIM_ENTITIES_LOCALPLAYER_H
#define SIM_ENTITIES_LOCALPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Sim/SimEntity.h"

// total size: 0x94
class LocalPlayer : public Sim::Entity, public IPlayer, public Sim::Collision::IListener {
  public:
    LocalPlayer(Sim::Param params);
    void ReleaseHud();

    // Overrides
    // IPlayer
    override virtual PlayerSettings *GetSettings() const;

  private:
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
