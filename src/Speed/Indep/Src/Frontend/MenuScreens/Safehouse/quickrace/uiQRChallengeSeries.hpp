#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRCHALLENGESERIES_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRCHALLENGESERIES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include <types.h>

struct GRaceParameters;
struct FEMultiImage;

// total size: 0x28
struct ChallengeDatum : public ArrayDatum {
    ChallengeDatum(unsigned int hash, unsigned int desc, GRaceParameters *race)
        : ArrayDatum(hash, desc) //
          ,
          race(race) {}

    ~ChallengeDatum() override {}

    void NotificationMessage(unsigned long msg, FEObject *pObj, unsigned long param1, unsigned long param2) override;

    GRaceParameters *race; // offset 0x24, size 0x4
};

// total size: 0x1D0
struct UIQRChallengeSeries : public ArrayScrollerMenu {
    UIQRChallengeSeries(ScreenConstructorData *sd);
    ~UIQRChallengeSeries() override;

    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;

    void ChooseTransmission();
    void RefreshHeader() override;
    void AddRace(GRaceParameters *race);
    bool IsRaceValidForMike(GRaceParameters *parms);
    void Setup();

    unsigned int prev_race_hash;         // offset 0xE8, size 0x4
    FEMultiImage *TrackMap;              // offset 0xEC, size 0x4
    UITrackMapStreamer TrackMapStreamer; // offset 0xF0, size 0xDC
    char *pMovieName;                    // offset 0x1CC, size 0x4
};

#endif
