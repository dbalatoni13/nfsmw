#ifndef UIEATRAXJUKEBOX_H
#define UIEATRAXJUKEBOX_H

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"

struct JukeboxEntry;

class JukeBoxScrollerDatum : public ScrollerDatum {
  public:
    uint32 SongIndex;       // offset 0x18, size 0x4
    uint8 PlayabilityField; // offset 0x1C, size 0x1

    JukeBoxScrollerDatum() : SongIndex(0), PlayabilityField(0) {}
    ~JukeBoxScrollerDatum() override {}
};

class JukeBoxScrollerSlot : public ScrollerSlot {
  public:
    JukeBoxScrollerSlot() {}
    ~JukeBoxScrollerSlot() override {}
};

// total size: 0x120
class UIEATraxScreen : public MenuScreen {
  public:
    Scrollerina Tracks;             // offset 0x2C, size 0xC8
    uint8 playOrder;                // offset 0xF4, size 0x1
    FEString *trackOrder;           // offset 0xF8, size 0x4
    FEObject *pSlotHighlight[4];    // offset 0xFC, size 0x10
    int NumSlots;                   // offset 0x10C, size 0x4
    int NumSongs;                   // offset 0x110, size 0x4
    JukeboxEntry *OriginalPlaylist; // offset 0x114, size 0x4
#ifndef EA_BUILD_A124
    int OriginalPlayState; // offset 0x118, size 0x4
#endif
    bool bTrackGrabbed; // offset 0x11C, size 0x1

    UIEATraxScreen(ScreenConstructorData *sd);
    ~UIEATraxScreen() override;

    void AddTrackSlot(ScrollerSlot *slot, uint32 baseHash, int num);
    void RefreshHeader();
    uint32 GetPlaybilityString(uint8 playability);
    uint32 GetStateString(uint8 state);
    void SetupSongList();
    void ScrollOrderState(u32 msg);
    void ScrollTracks(u32 msg);
    void ScrollTrackPlayability(u32 msg);
    void MoveTrack(u32 msg);
    void PreviewSong();
    void ReInsertSong();
    void NotificationMessage(u32 msg, FEObject *pObject, u32 Param1, u32 Param2) override;
    bool OptionsDidNotChange();
    void RestoreOriginals();
};

#endif
