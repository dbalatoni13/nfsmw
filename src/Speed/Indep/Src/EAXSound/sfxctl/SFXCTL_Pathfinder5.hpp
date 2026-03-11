#ifndef EAXSOUND_SFXCTL_SFXCTL_PATHFINDER5_H
#define EAXSOUND_SFXCTL_SFXCTL_PATHFINDER5_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

enum PATHEVENTRESULT {};

struct SFXCTL_Pathfinder : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    ~SFXCTL_Pathfinder() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void UpdateMixerOutputs() override;
    void SetupSFX(CSTATE_Base *_StateBase) override;

    static void EventReleaseCallback(void *eventID, PATHEVENTRESULT result);
};

extern SFXCTL_Pathfinder *g_pSFXCTL_Pathfinder;

#endif
