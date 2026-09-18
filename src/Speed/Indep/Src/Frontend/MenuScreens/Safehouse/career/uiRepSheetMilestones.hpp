#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETMILESTONES_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETMILESTONES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"

struct GMilestone;
struct GSpeedTrap;
struct UITrackMapStreamer;
struct FEMultiImage;

// total size: 0x28
struct MilestoneDatum : public ArrayDatum {
    GMilestone* my_milestone; // offset 0x24, size 0x4

    MilestoneDatum()
        : ArrayDatum(0, 0) //
        , my_milestone(nullptr) {}

    MilestoneDatum(unsigned int hash, unsigned int desc, GMilestone* milestone)
        : ArrayDatum(hash, desc) //
        , my_milestone(milestone) {}

    ~MilestoneDatum() override {}

    virtual unsigned int GetType() { return 0; }
    void NotificationMessage(u32 msg, FEObject* pObj, u32 param1,
                             u32 param2) override;
};

// total size: 0x2C
struct SpeedTrapDatum : public MilestoneDatum {
    GSpeedTrap* my_speedtrap; // offset 0x28, size 0x4

    SpeedTrapDatum()
        : MilestoneDatum() //
        , my_speedtrap(nullptr) {}

    SpeedTrapDatum(unsigned int hash, unsigned int desc, GSpeedTrap* speedtrap)
        : MilestoneDatum(hash, desc, nullptr) //
        , my_speedtrap(speedtrap) {}

    ~SpeedTrapDatum() override {}

    unsigned int GetType() override { return 1; }
};

// total size: 0xF4
struct uiRepSheetMilestones : public ArrayScrollerMenu {
    bool bIsInGame;                    // offset 0xE8, size 0x1
    UITrackMapStreamer* TrackMapStreamer; // offset 0xEC, size 0x4
    FEMultiImage* TrackMap;            // offset 0xF0, size 0x4

    uiRepSheetMilestones(ScreenConstructorData* sd);
    ~uiRepSheetMilestones() override {
        delete TrackMapStreamer;
    }

    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(u32 msg, FEObject* obj, u32 param1,
                             u32 param2) override;

    void Setup();
    void RefreshTrack();
    void AddMilestone(GMilestone* pMilestone);
    void AddSpeedtrap(GSpeedTrap* pSpeedTrap);
    void RefreshHeader() override;
};

enum MILESTONE_TYPES {
    eTYPE_MILESTONE = 0,
    eTYPE_SPEEDTRAP = 1,
};

#endif
