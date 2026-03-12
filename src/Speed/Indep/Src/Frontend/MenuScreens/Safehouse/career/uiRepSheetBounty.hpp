#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETBOUNTY_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETBOUNTY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"

struct UITrackMapStreamer;
struct FEMultiImage;

// total size: 0x178
struct uiRepSheetBounty : public ArrayScrollerMenu {
    bool bIsInGame;                    // offset 0xE8, size 0x1
    UITrackMapStreamer* TrackMapStreamer; // offset 0xEC, size 0x4
    FEMultiImage* TrackMap;            // offset 0xF0, size 0x4
    char orderedList[128];             // offset 0xF4, size 0x80
    bool tutorialPlaying;              // offset 0x174, size 0x1

    uiRepSheetBounty(ScreenConstructorData* sd);
    ~uiRepSheetBounty() override {
        delete TrackMapStreamer;
    }

    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;

    void Setup();
    void RefreshTrack();
    void RefreshHeader() override;
};

struct BountyDatum : public ArrayDatum {
    int index; // offset 0x24, size 0x4

    BountyDatum(unsigned int hash, unsigned int desc, unsigned int idx)
        : ArrayDatum(hash, desc) //
        , index(idx) {
    }
    ~BountyDatum() override {}
    void NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) override;
};

#endif
