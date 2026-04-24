#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_OPTIONS_UIEATRAXJUKEBOX_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_OPTIONS_UIEATRAXJUKEBOX_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"

struct JukeboxEntry;

struct JukeBoxScrollerSlot : public ScrollerSlot {
    JukeBoxScrollerSlot() {}
    ~JukeBoxScrollerSlot() override {}
};

struct JukeBoxScrollerDatum : public ScrollerDatum {
    unsigned int SongIndex;          // offset 0x18, size 0x4
    unsigned char PlayabilityField;  // offset 0x1C, size 0x1

    JukeBoxScrollerDatum()
        : SongIndex(0) //
        , PlayabilityField(0) {}
    ~JukeBoxScrollerDatum() override {}
};

// total size: 0x120
struct UIEATraxScreen : public MenuScreen {
    Scrollerina Tracks;                  // offset 0x2C, size 0xC8
    unsigned char playOrder;             // offset 0xF4, size 0x1
    FEString* trackOrder;                // offset 0xF8, size 0x4
    FEObject* pSlotHighlight[4];         // offset 0xFC, size 0x10
    int NumSlots;                        // offset 0x10C, size 0x4
    int NumSongs;                        // offset 0x110, size 0x4
    JukeboxEntry* OriginalPlaylist;      // offset 0x114, size 0x4
    int OriginalPlayState;               // offset 0x118, size 0x4
    int bTrackGrabbed;                   // offset 0x11C, size 0x1

    UIEATraxScreen(ScreenConstructorData* sd);
    ~UIEATraxScreen() override;

    void AddTrackSlot(ScrollerSlot* slot, unsigned int baseHash, int num);
    void RefreshHeader();
    unsigned int GetPlaybilityString(unsigned char playability);
    unsigned int GetStateString(unsigned char state);
    void SetupSongList();
    void ScrollOrderState(unsigned long msg);
    void ScrollTracks(unsigned long msg);
    void ScrollTrackPlayability(unsigned long msg);
    void MoveTrack(unsigned long msg);
    void PreviewSong();
    void ReInsertSong();
    void NotificationMessage(unsigned long msg, FEObject* pObject, unsigned long Param1,
                             unsigned long Param2) override;
    bool OptionsDidNotChange();
    void RestoreOriginals();
};

#endif
