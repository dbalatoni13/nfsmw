#include "uiEATraxJukebox.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"

struct stSongInfo {
    char *SongName; // offset 0x0, size 0x4
    char *Artist;   // offset 0x4, size 0x4
    char *Album;    // offset 0x8, size 0x4
    char *DefPlay;  // offset 0xC, size 0x4
    int PathEvent;  // offset 0x10, size 0x4
};

struct SongInfoList {
    stSongInfo **_M_start;
    stSongInfo **_M_finish;

    unsigned int size() const {
        return static_cast<unsigned int>((_M_finish - _M_start));
    }

    stSongInfo *&operator[](unsigned int n) {
        return _M_start[n];
    }
};

extern SongInfoList Songs;
extern cFrontendDatabase *FEDatabase;

void UIEATraxScreen::AddTrackSlot(ScrollerSlot *slot, uint32 baseHash, int num) {
    FEObject *string = FEngFindObject(GetPackageName(), baseHash + num);
    slot->AddData(string);
}

UIEATraxScreen::UIEATraxScreen(ScreenConstructorData *sd)
    : MenuScreen(sd),                                                                       //
      Tracks(GetPackageName(), "ARRAY_SCROLL_REGION", "ScrollBar", true, true, false, true) //
{
    const u32 FEObj_TrackModeType = 0xCA74A2FA;
    NumSlots = 4;
    bTrackGrabbed = false;
    Tracks.SetMouseDownMsg(1);
    Tracks.SetClickToSelectMode(true);
    NumSongs = Songs.size();
    trackOrder = FEngFindString(GetPackageName(), FEObj_TrackModeType);
    JukeboxEntry *playlist = FEDatabase->GetUserProfile(0)->Playlist;
    OriginalPlaylist = new (__FILE__, __LINE__) JukeboxEntry[NumSongs];
    bMemCpy(OriginalPlaylist, playlist, NumSongs * sizeof(JukeboxEntry));
#ifndef EA_BUILD_A124
    OriginalPlayState = FEDatabase->GetAudioSettings()->PlayState;
#endif
    SetupSongList();
}

UIEATraxScreen::~UIEATraxScreen() {
    if (OriginalPlaylist != nullptr) {
        delete[] OriginalPlaylist;
    }
}

void UIEATraxScreen::RefreshHeader() {
    unsigned int hash = GetStateString(static_cast<unsigned char>(FEDatabase->GetAudioSettings()->PlayState));
    FEngSetLanguageHash(trackOrder, hash);
}

uint32 UIEATraxScreen::GetPlaybilityString(unsigned char playability) {
    switch (playability) {
        case 0:
            return 0x9CCE9F86;
        case 1:
            return 0x5278C50B;
        case 2:
            return 0x5C1B351C;
        case 3:
            return 0x9CCE64C4;
        default:
            return 0;
    }
}

uint32 UIEATraxScreen::GetStateString(unsigned char state) {
    switch (state) {
        case 0:
            return 0x4CA36B89;
        case 1:
            return 0xA9C9C8F7;
        default:
            return 0;
    }
}

void UIEATraxScreen::SetupSongList() {
    char playability_string[128];
    char num_string[8];

    for (int i = 0; i < NumSlots; i++) {
        JukeBoxScrollerSlot *slot = new (__FILE__, __LINE__) JukeBoxScrollerSlot();
        slot->SetBacking(FEngFindObject(GetPackageName(), FEngHashString("MOUSE_REGION_%d", i + 1)));
        AddTrackSlot(slot, 0xE454E9A5, i);
        AddTrackSlot(slot, 0x66646FC4, i);
        AddTrackSlot(slot, 0x2890C8AF, i);
        AddTrackSlot(slot, 0x77BD189E, i);
        AddTrackSlot(slot, 0xF3EBDC4E, i);
        Tracks.AddSlot(slot);
    }

    for (int i = 0; i < NumSongs; i++) {
        JukeBoxScrollerDatum *datum = new (__FILE__, __LINE__) JukeBoxScrollerDatum();
        Tracks.AddData(datum);

        FEngSNPrintf(num_string, 8, "%.2d", i + 1);
        datum->AddData(num_string, 0);

        JukeboxEntry *playlist = FEDatabase->GetUserProfile(0)->Playlist;
        datum->AddData(Songs[playlist[i].SongIndex]->Artist, 0);
        datum->AddData(Songs[playlist[i].SongIndex]->SongName, 0);
        datum->AddData(Songs[playlist[i].SongIndex]->Album, 0);
        datum->SongIndex = playlist[i].SongIndex;
        datum->PlayabilityField = playlist[i].PlayabilityField;

        GetLocalizedString(playability_string, 128, GetPlaybilityString(datum->PlayabilityField));
        datum->AddData(playability_string, 0);
    }

    Tracks.Update(true);
    RefreshHeader();
}

void UIEATraxScreen::ScrollOrderState(u32 msg) {
    unsigned char state = FEDatabase->GetAudioSettings()->PlayState;
    FEDatabase->GetAudioSettings()->PlayState = (state == 0);
    RefreshHeader();
    MControlPathfinder(true, 0, 0, 0).Send("EATraxInit");
}

void UIEATraxScreen::ScrollTracks(u32 msg) {
    if (msg == 0x72619778) {
        if (!Tracks.IsAtHead()) {
            Tracks.ScrollPrev();
        }
    } else {
        if (!Tracks.IsAtTail()) {
            Tracks.ScrollNext();
        }
    }
}

void UIEATraxScreen::ScrollTrackPlayability(u32 msg) {
    JukeBoxScrollerDatum *datum = static_cast<JukeBoxScrollerDatum *>(Tracks.GetSelectedDatum());
    JukeboxEntry *entry;
    JukeboxEntry *playlist = FEDatabase->GetUserProfile(0)->Playlist;
    int play_flag;
    ScrollerDatumNode *node;

    entry = playlist;
    for (int i = 0; i < NumSongs; i++) {
        if (playlist[i].SongIndex == datum->SongIndex) {
            entry = &playlist[i];
            break;
        }
    }

    play_flag = entry->PlayabilityField;
    if (msg == 0x9120409E) {
        play_flag--;
        if (play_flag < 0) {
            play_flag = 3;
        }
    } else if (msg == 0xB5971BF1) {
        play_flag++;
        if (play_flag > 3) {
            play_flag = 0;
        }
    }
    entry->PlayabilityField = play_flag;
    datum->PlayabilityField = play_flag;

    node = datum->Strings.GetTail();
    FEngSNPrintf(node->String, 128, GetLocalizedString(GetPlaybilityString(entry->PlayabilityField)));
    Tracks.Update(true);
    MControlPathfinder(true, 0, 0, 0).Send("EATraxInit");
}

void UIEATraxScreen::MoveTrack(u32 msg) {
    ScrollerSlot *old_slot = Tracks.GetSelectedSlot();
    int oldSlotIndex = Tracks.GetSelectedSlotIndex();

    if (msg == 0x72619778) {
        Tracks.MovePrev();
    } else if (msg == 0x911C0A4B) {
        Tracks.MoveNext();
    }

    int num = 1;
    ScrollerDatum *datum = Tracks.GetFirstDatum();
    while (datum != Tracks.GetLastDatum()) {
        ScrollerDatumNode *node = datum->Strings.GetHead();
        if (node != nullptr) {
            FEngSNPrintf(node->String, 8, "%.2d", num);
            num++;
        }
        datum = datum->GetNext();
    }
    Tracks.Update(true);
    ReInsertSong();
}

void UIEATraxScreen::PreviewSong() {
    JukeBoxScrollerDatum *cur_datum = static_cast<JukeBoxScrollerDatum *>(Tracks.GetSelectedDatum());
    int path_event = Songs[cur_datum->SongIndex]->PathEvent;
    MControlPathfinder(true, path_event, 0, 0).Send("Pathfinder5");
}

void UIEATraxScreen::ReInsertSong() {
    JukeboxEntry *playlist = FEDatabase->GetUserProfile(0)->Playlist;
    JukeBoxScrollerDatum *datum = static_cast<JukeBoxScrollerDatum *>(Tracks.GetFirstDatum());

    for (int i = 0; i < NumSongs; i++) {
        playlist[i].SongIndex = datum->SongIndex;
        playlist[i].PlayabilityField = datum->PlayabilityField;
        datum = static_cast<JukeBoxScrollerDatum *>(datum->GetNext());
    }

    MControlPathfinder(true, 0, 0, 0).Send("EATraxInit");
}

void UIEATraxScreen::NotificationMessage(u32 msg, FEObject *pObject, u32 Param1, u32 Param2) {
    switch (msg) {
        case 0x35F8620B: {
            ScrollerSlot *slot = Tracks.GetSelectedSlot();
            if (slot != nullptr) {
                slot->SetScript(0x249DB7B7);
            }
            break;
        }
        case 0x5073EF13:
        case 0xD9FEEC59:
            ScrollOrderState(msg);
            break;
        case 0x9120409E:
        case 0xB5971BF1:
            ScrollTrackPlayability(msg);
            break;
        case 0x72619778:
        case 0x911C0A4B:
            if (bTrackGrabbed) {
                MoveTrack(msg);
            } else {
                ScrollTracks(msg);
            }
            break;
        case 0x911AB364:
            if (OptionsDidNotChange()) {
                cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            } else {
                const char *blurb = GetLocalizedString(0xE9CB802F);
                DialogInterface::ShowTwoButtons(GetPackageName(), "Dialog.fng", dialog_alert, 0x70E01038, 0x417B25E4, 0x775DBA97, 0x34DC1BCF,
                                                0x34DC1BCF, first_dialog_button2, blurb);
            }
            MControlPathfinder(true, 0, 0, 0).Send("EATraxInit");
            break;
        case 0x775DBA97:
            RestoreOriginals();
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            break;
        case 0xC519BFC4:
            bTrackGrabbed = bTrackGrabbed ^ 1;
            break;
        case 0xC519BFC3:
            PreviewSong();
            break;
        case 0xE1FDE1D1:
            MControlPathfinder(true, 0xFFFFFFFF, 0, 0).Send("EATraxInit");
            {
                bool dirty = false;
                if (FEDatabase->IsOptionsDirty() || !OptionsDidNotChange()) {
                    dirty = true;
                }
                FEDatabase->SetOptionsDirty(dirty);
            }
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            break;
    }
}

bool UIEATraxScreen::OptionsDidNotChange() {
    bool ret;
    JukeboxEntry *playlist = FEDatabase->GetUserProfile(0)->Playlist;
    int cmp = bMemCmp(playlist, OriginalPlaylist, NumSongs * sizeof(JukeboxEntry));
    ret = (cmp == 0);
#ifndef EA_BUILD_A124
    if (FEDatabase->GetAudioSettings()->PlayState != OriginalPlayState) {
        ret = false;
    }
#endif
    return ret;
}

void UIEATraxScreen::RestoreOriginals() {
    JukeboxEntry *playlist = FEDatabase->GetUserProfile(0)->Playlist;
    bMemCpy(playlist, OriginalPlaylist, NumSongs * sizeof(JukeboxEntry));
#ifndef EA_BUILD_A124
    FEDatabase->GetAudioSettings()->PlayState = OriginalPlayState;
#endif
}
