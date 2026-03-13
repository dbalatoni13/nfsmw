#ifndef FRONTEND_HUD_FEMENUZONETRIGGER_H
#define FRONTEND_HUD_FEMENUZONETRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

class MenuZoneTrigger : public HudElement, public IMenuZoneTrigger {
  public:
    MenuZoneTrigger(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;

  private:
    FEGroup * mEngageMechanic;
    FEImage * mEventIcon;
    FEGroup * mCingularIcon;
    const char * mZoneType;
    void * mpRaceActivity;
    bool mbCingularQueued;
    bool mbInsideTrigger;
    Timer mCingularTimer;
};

#endif
