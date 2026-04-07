#ifndef EAXSOUND_CARSFX_CARSFX_TRAFFICFX_H
#define EAXSOUND_CARSFX_CARSFX_TRAFFICFX_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/ENVIRO_AEMS.h"

struct CARSFX_TrafficHorn : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    CARSFX_TrafficHorn();
    ~CARSFX_TrafficHorn() override;

    static SndBase *CreateObject(unsigned int allocator);

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    void Destroy() override;
    void Detach() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    virtual void StartHonkHorn();
    virtual void StopHonkHorn();
    void EndCarHonk();
    void UpdateMixerOutputs() override;
    void CSIS_BeginHonk(int ID);
    void CSIS_EndHonk();
    void CSIS_UpdateHOnk();
    virtual bool IsHonking();
    bool IsPlayerCarInRange(int nplayer);

  private:
    SFXCTL_3DCarPos *m_p3DCarPosCtl; // offset 0x28, size 0x4
    ENV_STATIC *m_HornSound;         // offset 0x2C, size 0x4
    float HornDuration;              // offset 0x30, size 0x4
    float tHornBegin;                // offset 0x34, size 0x4
    float tSinceLastHorn;            // offset 0x38, size 0x4
    bool AIPlayingHonk;              // offset 0x3C, size 0x1
    bool SND_PlayingHonk;            // offset 0x40, size 0x1
    bool ShouldHonk;                 // offset 0x44, size 0x1
    float tSinceLastAttemptedToHonk; // offset 0x48, size 0x4
    int refCnt;                      // offset 0x4C, size 0x4
    cInterpLine HonkFadeOut;         // offset 0x50, size 0x1C
    bool IsEndingHonk;               // offset 0x6C, size 0x1
    EAX_CarState *pClosestCar;       // offset 0x70, size 0x4
};

#endif
