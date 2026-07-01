#ifndef __UI_REP_SHEET_MILESTONES_HPP__
#define __UI_REP_SHEET_MILESTONES_HPP__

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Gameplay/GSpeedTrap.h"
#include "Speed/Indep/Src/Gameplay/GMilestone.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"

typedef enum { eTYPE_MILESTONE = 0, eTYPE_SPEEDTRAP = 1 } MILESTONE_TYPES;

// total size: 0x28
class MilestoneDatum : public ArrayDatum {
  public:
    MilestoneDatum() : ArrayDatum(0, 0), my_milestone(nullptr) {}

    MilestoneDatum(uint32 hash, uint32 desc, GMilestone *milestone) : ArrayDatum(hash, desc), my_milestone(milestone) {}

    ~MilestoneDatum() override {}

    virtual uint32 GetType() {
        return eTYPE_MILESTONE;
    }
    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override;

    GMilestone *my_milestone; // offset 0x24, size 0x4
};

// total size: 0x2C
class SpeedTrapDatum : public MilestoneDatum {
  public:
    SpeedTrapDatum() : MilestoneDatum(), my_speedtrap(nullptr) {}

    SpeedTrapDatum(uint32 hash, uint32 desc, GSpeedTrap *speedtrap) : MilestoneDatum(hash, desc, nullptr), my_speedtrap(speedtrap) {}

    ~SpeedTrapDatum() override {}

    uint32 GetType() override {
        return eTYPE_SPEEDTRAP;
    }

    GSpeedTrap *my_speedtrap; // offset 0x28, size 0x4
};

// total size: 0xF4
class uiRepSheetMilestones : public ArrayScrollerMenu {
  public:
    uiRepSheetMilestones(ScreenConstructorData *sd);
    ~uiRepSheetMilestones() override {
        delete TrackMapStreamer;
    }

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  private:
    void RefreshHeader() override;
    void Setup();
    void RefreshTrack();
    void AddMilestone(GMilestone *pMilestone);
    void AddSpeedtrap(GSpeedTrap *pSpeedTrap);

    bool bIsInGame;                       // offset 0xE8, size 0x1
    UITrackMapStreamer *TrackMapStreamer; // offset 0xEC, size 0x4
    FEMultiImage *TrackMap;               // offset 0xF0, size 0x4
};

#endif
