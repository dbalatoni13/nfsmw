// OWNED BY zFeOverlay AGENT - do not empty or overwrite
#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_MYCARSMANAGER_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_MYCARSMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

#include <types.h>

struct FECarRecord;

// total size: 0x1BC
struct MyCarsManager : public ArrayScrollerMenu {
    MyCarsManager(ScreenConstructorData *sd);
    ~MyCarsManager() override {}

    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) override;

    void Setup();
    bool CanAddMoreCars();
    void RefreshCarList();
    void RefreshHeader() override;
    void UpdateSliders();
    void UpdateCar();

    TwoStageSlider AccelerationSlider;     // offset 0xE8, size 0x44
    TwoStageSlider TopSpeedSlider;         // offset 0x12C, size 0x44
    TwoStageSlider HandlingSlider;         // offset 0x170, size 0x44
    FECarRecord *pSelectedCar;             // offset 0x1B4, size 0x4
    Timer tCarLoadTimer;                   // offset 0x1B8, size 0x4
    bool bGoToShowcase;                    // offset 0x1BC, size 0x1
};

#endif
