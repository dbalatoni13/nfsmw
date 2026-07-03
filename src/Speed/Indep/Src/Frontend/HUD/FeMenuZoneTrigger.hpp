#ifndef FEMENUZONETRIGGER_H
#define FEMENUZONETRIGGER_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// File: speed/indep/src/frontend/hud/FeMenuZoneTrigger.hpp
// total size: 0x50
// Decl: speed/indep/src/frontend/hud/FeMenuZoneTrigger.hpp:14
class MenuZoneTrigger : public HudElement, public IMenuZoneTrigger {
  public:
    typedef enum {
        ENGAGE_DPAD_ELEMENT_UP = 0,
        ENGAGE_DPAD_ELEMENT_DOWN = 1,
        ENGAGE_DPAD_ELEMENT_LEFT = 2,
        ENGAGE_DPAD_ELEMENT_RIGHT = 3
    } ENGAGE_DPAD_ELEMENT_DIRECTION;

    MenuZoneTrigger(UTL::COM::Object *pOuter, const char *pkg_name, int player_number);
    ~MenuZoneTrigger() override {} // Decl: speed/indep/src/frontend/hud/FeMenuZoneTrigger.hpp:36

    void Update(IPlayer *player) override;

    bool ShouldSeeMenuZoneCluster() override;

    bool IsPlayerInsideTrigger() override;

    bool IsType(const char *t) override;

    void EnterTrigger(GRuntimeInstance *pRaceActivity) override;

    void EnterTrigger(const char *zoneType) override;

    void ExitTrigger() override;

    void RequestEventInfoDialog(int port) override;

    void RequestZoneInfoDialog(int port) override;

    void RequestDoAction() override;

    void PulseDPadButton(ENGAGE_DPAD_ELEMENT_DIRECTION direction, FEObject *iconToShow);

    void HideDPadButton();

    void RequestCingularLogo() override { // Decl: speed/indep/src/frontend/hud/FeMenuZoneTrigger.hpp:47
        mbCingularQueued = true;
    }

  private:
    FEGroup *mEngageMechanic;         // offset 0x30, size 0x4, Decl: speed/indep/src/frontend/hud/FeMenuZoneTrigger.hpp:56
    FEImage *mEventIcon;              // offset 0x34, size 0x4, Decl: speed/indep/src/frontend/hud/FeMenuZoneTrigger.hpp:57
    FEGroup *mCingularIcon;           // offset 0x38, size 0x4
    const char *mZoneType;            // offset 0x3C, size 0x4
    GRuntimeInstance *mpRaceActivity; // offset 0x40, size 0x4
    bool mbCingularQueued;            // offset 0x44, size 0x1, Decl: speed/indep/src/frontend/hud/FeMenuZoneTrigger.hpp:61
    bool mbInsideTrigger;             // offset 0x48, size 0x1
    Timer mCingularTimer;             // offset 0x4C, size 0x4, Decl: speed/indep/src/frontend/hud/FeMenuZoneTrigger.hpp:65
};

#endif
