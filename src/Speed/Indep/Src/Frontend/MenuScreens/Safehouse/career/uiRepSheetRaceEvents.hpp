#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETRACEEVENTS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETRACEEVENTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"

struct GRaceParameters;
struct FEMultiImage;

// total size: 0x28
struct RaceDatum : public ArrayDatum {
    GRaceParameters* race; // offset 0x24, size 0x4

    RaceDatum()
        : ArrayDatum(0, 0) //
        , race(nullptr) {}

    RaceDatum(unsigned int hash, unsigned int desc, GRaceParameters* race)
        : ArrayDatum(hash, desc) //
        , race(race) {}

    ~RaceDatum() override {}

    void NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1,
                             unsigned long param2) override;
};

// total size: 0x1D4
struct UISafehouseRaceSheet : public ArrayScrollerMenu {
    UITrackMapStreamer TrackMapStreamer; // offset 0xE8, size 0xDC
    FEMultiImage* TrackMap;            // offset 0x1C4, size 0x4
    bool bIsInGame;                    // offset 0x1C8, size 0x1
    bool currentEvents;                // offset 0x1CC, size 0x1
    int currentIndex;                  // offset 0x1D0, size 0x4

    UISafehouseRaceSheet(ScreenConstructorData* sd);
    ~UISafehouseRaceSheet() override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
    void RefreshHeader() override;

    bool AddRace(GRaceParameters* race);
    void Setup();
    void ToggleList();
};

#endif
