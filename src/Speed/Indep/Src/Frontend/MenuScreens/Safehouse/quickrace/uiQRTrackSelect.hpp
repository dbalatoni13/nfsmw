#ifndef UIQRTRACKSELECT_H
#define UIQRTRACKSELECT_H

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

// total size: 0x120
class UIQRTrackSelect : public MenuScreen {
  public:
    UIQRTrackSelect(ScreenConstructorData *sd);
    ~UIQRTrackSelect() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    void RefreshHeader();

  protected:
    virtual void Setup();
    void BuildPresetTrackList();
    void SetSelectedTrack(GRaceParameters *track);
    void ScrollTracks(eScrollDir dir);
    void ScrollRegions(eScrollDir dir);
    bool IsRaceValidForMike(GRaceParameters *parms);

    void TryToAddTrack(GRaceParameters *parms, int unlock_filter, int bin_num);

    UITrackMapStreamer TrackMapStreamer; // offset 0x2C, size 0xDC
    bTList<SelectableTrack> Tracks;      // offset 0x108, size 0x8
    SelectableTrack *pCurrentNode;       // offset 0x110, size 0x4
    GRaceParameters *pCurrentTrack;      // offset 0x114, size 0x4
    FEMultiImage *TrackMap;              // offset 0x118, size 0x4
    uint32 MapHash;                      // offset 0x11C, size 0x4
};

#endif
