// OWNED BY zFeOverlay AGENT - do not empty or overwrite
#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRCHALLENGESERIES_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRCHALLENGESERIES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

#include <types.h>

struct GRaceParameters;
struct FEMultiImage;

// total size: 0x28
struct ChallengeDatum : public ArrayDatum {
    ChallengeDatum(unsigned int hash, unsigned int desc, GRaceParameters *race)
        : ArrayDatum(hash, desc) //
        , race(race) {}

    ~ChallengeDatum() override {}

    void NotificationMessage(unsigned long msg, FEObject *pObj, unsigned long param1, unsigned long param2) override;

    GRaceParameters *race; // offset 0x24, size 0x4
};

// total size: 0x1D0
struct UIQRChallengeSeries : public ArrayScrollerMenu {
    UIQRChallengeSeries(ScreenConstructorData *sd);
    ~UIQRChallengeSeries() override;

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;

    void Setup();
    void RefreshHeader() override;
    void BuildSeriesList();
    void AddRace(GRaceParameters *race);
    bool IsRaceValidForMike(GRaceParameters *parms);
    void ChooseTransmission();

    UITrackMapStreamer TrackMapStreamer;     // offset 0xE8, size 0xDC
    FEMultiImage *TrackMap;                // offset 0x1C4, size 0x4
    unsigned int MapHash;                  // offset 0x1C8, size 0x4
    Timer tTimer;                          // offset 0x1CC, size 0x4
};

#endif
