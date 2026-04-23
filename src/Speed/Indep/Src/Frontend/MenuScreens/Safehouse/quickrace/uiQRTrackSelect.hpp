#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRTRACKSELECT_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UIQRTRACKSELECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconPanel.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

struct FEMultiImage;
struct GRaceParameters;
struct SelectableTrack;

// total size: 0x120
struct UIQRTrackSelect : public MenuScreen {
    UIQRTrackSelect(ScreenConstructorData *sd);
    ~UIQRTrackSelect() override;

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;

    virtual void Setup();

    void SetSelectedTrack(GRaceParameters *track);
    bool IsRaceValidForMike(GRaceParameters *parms);
    void TryToAddTrack(GRaceParameters *parms, int unlock_filter, int bin_num);
    void BuildPresetTrackList();
    void RefreshHeader();
    void ScrollTracks(eScrollDir dir);
    void ScrollRegions(eScrollDir dir);

    UITrackMapStreamer TrackMapStreamer; // offset 0x2C, size 0xDC
    bTList<SelectableTrack> Tracks;      // offset 0x108, size 0x8
    SelectableTrack *pCurrentNode;       // offset 0x110, size 0x4
    GRaceParameters *pCurrentTrack;      // offset 0x114, size 0x4
    FEMultiImage *TrackMap;              // offset 0x118, size 0x4
    unsigned int MapHash;                // offset 0x11C, size 0x4
};

#endif
