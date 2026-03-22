// OWNED BY zFeOverlay AGENT - do not empty or overwrite
#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRTRACKOPTIONS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRTRACKOPTIONS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"

#include <types.h>

struct GRaceParameters;

// total size: 0x154
struct UIQRTrackOptions : public UIWidgetMenu {
    UIQRTrackOptions(ScreenConstructorData *sd);
    ~UIQRTrackOptions() override {}

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;

    void BoilerPlateOnline(const bool &boAddLaps);
    void SetupCircuit();
    void SetupSprint();
    void SetupDrag();
    void SetupKnockout();
    void SetupSpeedTrap();
    void SetupTollbooth();

    GRaceParameters *race;              // offset 0x138, size 0x4
    unsigned int ConfirmPasswordId;     // offset 0x13C, size 0x4
    unsigned int EnterPasswordId;       // offset 0x140, size 0x4
    unsigned int PasswordProtectedId;   // offset 0x144, size 0x4
    int m_code;                         // offset 0x148, size 0x4
    int msgHandle;                      // offset 0x14C, size 0x4
    bool m_boDisconnectPercAvail;       // offset 0x150, size 0x1
};

#endif
