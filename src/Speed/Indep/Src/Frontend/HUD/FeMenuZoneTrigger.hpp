#ifndef FRONTEND_HUD_FEMENUZONETRIGGER_H
#define FRONTEND_HUD_FEMENUZONETRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct GRuntimeInstance;

class MenuZoneTrigger : public HudElement, public IMenuZoneTrigger {
  public:
    enum ENGAGE_DPAD_ELEMENT_DIRECTION {
        ENGAGE_DPAD_ELEMENT_NONE = 0,
        ENGAGE_DPAD_ELEMENT_UP = 1,
        ENGAGE_DPAD_ELEMENT_DOWN = 2,
        ENGAGE_DPAD_ELEMENT_LEFT = 3,
        ENGAGE_DPAD_ELEMENT_RIGHT = 4,
        ENGAGE_DPAD_ELEMENT_NUM = 5,
    };

    MenuZoneTrigger(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void RequestCingularLogo() override {
        mbCingularQueued = true;
    }
    bool ShouldSeeMenuZoneCluster() override;
    bool IsPlayerInsideTrigger() override;
    void EnterTrigger(GRuntimeInstance *pRaceActivity) override;
    void EnterTrigger(const char *zoneType) override;
    void ExitTrigger() override;
    void RequestEventInfoDialog(int port) override;
    void RequestZoneInfoDialog(int port) override;
    bool IsType(const char *t) override;
    void RequestDoAction() override;
    void HideDPadButton();
    void PulseDPadButton(ENGAGE_DPAD_ELEMENT_DIRECTION direction, FEObject *iconToShow);

  private:
    FEGroup * mEngageMechanic;
    FEImage * mEventIcon;
    FEGroup * mCingularIcon;
    const char * mZoneType;
    GRuntimeInstance * mpRaceActivity;
    bool mbCingularQueued;
    bool mbInsideTrigger;
    Timer mCingularTimer;
};

#endif
