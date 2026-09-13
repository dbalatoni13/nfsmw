#ifndef __UI_REP_SHEET_RACE_EVENTS_HPP__
#define __UI_REP_SHEET_RACE_EVENTS_HPP__

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"

// total size: 0x28
class RaceDatum : public ArrayDatum {
  public:
    RaceDatum() : ArrayDatum(0, 0), race(nullptr) {}

    RaceDatum(uint32 hash, uint32 desc, GRaceParameters *race) : ArrayDatum(hash, desc), race(race) {}

    ~RaceDatum() override {}

    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override;

    GRaceParameters *race; // offset 0x24, size 0x4
};

// total size: 0x1D4
class UISafehouseRaceSheet : public ArrayScrollerMenu {
  public:
    UISafehouseRaceSheet(ScreenConstructorData *sd);
    ~UISafehouseRaceSheet() override;
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  private:
    void RefreshHeader() override;
    void Setup();
    bool AddRace(GRaceParameters *race);
    void ToggleList();

    UITrackMapStreamer TrackMapStreamer; // offset 0xE8, size 0xDC
    FEMultiImage *TrackMap;              // offset 0x1C4, size 0x4
    bool bIsInGame;                      // offset 0x1C8, size 0x1
    bool currentEvents;                  // offset 0x1CC, size 0x1
#ifndef EA_BUILD_A124
    int currentIndex; // offset 0x1D0, size 0x4
#endif
};

#endif
