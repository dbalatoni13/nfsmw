#ifndef __UI_REP_SHEET_BOUNTY_HPP__
#define __UI_REP_SHEET_BOUNTY_HPP__

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"

class BountyDatum : public ArrayDatum {
  public:
    BountyDatum(uint32 hash, uint32 desc, uint32 idx) : ArrayDatum(hash, desc), index(idx) {}
    ~BountyDatum() override {}
    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override;

    int index; // offset 0x24, size 0x4
};

// total size: 0x178
class uiRepSheetBounty : public ArrayScrollerMenu {
  public:
    uiRepSheetBounty(ScreenConstructorData *sd);
    ~uiRepSheetBounty() override {
        delete TrackMapStreamer;
    }

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  private:
    void RefreshHeader() override;
    void RefreshTrack();
    void Setup();

    bool bIsInGame;                       // offset 0xE8, size 0x1
    UITrackMapStreamer *TrackMapStreamer; // offset 0xEC, size 0x4
    FEMultiImage *TrackMap;               // offset 0xF0, size 0x4
    char orderedList[128];                // offset 0xF4, size 0x80
    bool tutorialPlaying;                 // offset 0x174, size 0x1
};

#endif
