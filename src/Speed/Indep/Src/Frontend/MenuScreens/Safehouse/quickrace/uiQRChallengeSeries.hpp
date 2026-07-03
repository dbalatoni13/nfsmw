#ifndef UIQRCHALLENGESERIES_H
#define UIQRCHALLENGESERIES_H

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include <types.h>

// total size: 0x28
class ChallengeDatum : public ArrayDatum {
  public:
    ChallengeDatum(uint32 hash, uint32 desc, GRaceParameters *race) : ArrayDatum(hash, desc), race(race) {}

    ~ChallengeDatum() override {}

    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override;

    GRaceParameters *race; // offset 0x24, size 0x4
};

// total size: 0x1D0
class UIQRChallengeSeries : public ArrayScrollerMenu {
  public:
    UIQRChallengeSeries(ScreenConstructorData *sd);
    ~UIQRChallengeSeries() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  private:
    void ChooseTransmission();
    void RefreshHeader() override;
    void Setup();
    void AddRace(GRaceParameters *race);
    bool IsRaceValidForMike(GRaceParameters *parms);

    uint32 prev_race_hash;               // offset 0xE8, size 0x4
    FEMultiImage *TrackMap;              // offset 0xEC, size 0x4
    UITrackMapStreamer TrackMapStreamer; // offset 0xF0, size 0xDC
    char *pMovieName;                    // offset 0x1CC, size 0x4
};

#endif
