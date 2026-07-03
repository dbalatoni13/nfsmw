#ifndef UIQRTRACKOPTIONS_H
#define UIQRTRACKOPTIONS_H

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feUIWidgetMenu.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"

#include <types.h>

// total size: 0x154
class UIQRTrackOptions : public UIWidgetMenu {
  public:
    UIQRTrackOptions(ScreenConstructorData *sd);
    ~UIQRTrackOptions() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void Setup() override;
    void SetupCircuit();
    void SetupSprint();
    void SetupDrag();
    void SetupKnockout();
    void SetupSpeedTrap();
    void SetupTollbooth();
    void BoilerPlateOnline(const bool &boAddLaps);
#ifdef EA_BUILD_A123
    void SetOnlineTogglesProperties();
    void LoadOnlineDefaults();
    int32 GetDisconnectPercentage();
    static void GetExtendedStatsCB();
#endif

    GRaceParameters *race;        // offset 0x138, size 0x4
    uint32 ConfirmPasswordId;     // offset 0x13C, size 0x4
    uint32 EnterPasswordId;       // offset 0x140, size 0x4
    uint32 PasswordProtectedId;   // offset 0x144, size 0x4
    int m_code;                   // offset 0x148, size 0x4
    dialog_handle msgHandle;      // offset 0x14C, size 0x4
    bool m_boDisconnectPercAvail; // offset 0x150, size 0x1
};

#endif
