#ifndef FRONTEND_HUD_FEMENUZONETRIGGER_H
#define FRONTEND_HUD_FEMENUZONETRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IMenuZoneTrigger.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct GRuntimeInstance;
class FEGroup;

// total size: 0x50
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

    ~MenuZoneTrigger() override {}
    void RequestCingularLogo() override {
        mbCingularQueued = 1;
    }

    MenuZoneTrigger(UTL::COM::Object *pOuter, const char *pkg_name, int player_number);

    void Update(IPlayer *player) override;
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
    FEGroup *mEngageMechanic;            // offset 0x30, size 0x4
    FEImage *mEventIcon;                 // offset 0x34, size 0x4
    FEGroup *mCingularIcon;              // offset 0x38, size 0x4
    const char *mZoneType;               // offset 0x3C, size 0x4
    GRuntimeInstance *mpRaceActivity;    // offset 0x40, size 0x4
    int mbCingularQueued;                // offset 0x44, size 0x4
    int mbInsideTrigger;                 // offset 0x48, size 0x4
    Timer mCingularTimer;                // offset 0x4C, size 0x4
};

#endif
