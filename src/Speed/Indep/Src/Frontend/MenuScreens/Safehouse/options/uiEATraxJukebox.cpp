#include "uiEATraxJukebox.hpp"

#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"

void UIEATraxScreen::AddTrackSlot(ScrollerSlot *slot, uint32 baseHash, int num) {
    FEObject *string = FEngFindObject(GetPackageName(), baseHash + num);
    uint32 hash;
    slot->AddData(string);
}

UIEATraxScreen::UIEATraxScreen(ScreenConstructorData *sd)
    : MenuScreen(sd),                                                                       //
      Tracks(GetPackageName(), "ARRAY_SCROLL_REGION", "ScrollBar", true, true, false, true) //
{
    const u32 FEObj_TrackModeType = 0xCA74A2FA;
    NumSlots = 4;
    NumSongs = Songs.size();
    bTrackGrabbed = false;
    Tracks.SetClickToSelectMode(true);
    Tracks.SetMouseDownMsg(1);
    trackOrder = FEngFindString(GetPackageName(), FEObj_TrackModeType);
    JukeboxEntry *playlist = FEDatabase->GetUserProfile(0)->Playlist;
    OriginalPlaylist = new ("JukeboxEntry[] - backup copy", 0) JukeboxEntry[NumSongs];
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
    uint32 hash = GetStateString(FEDatabase->GetAudioSettings()->PlayState);
    FEngSetLanguageHash(trackOrder, hash);
}

uint32 UIEATraxScreen::GetPlaybilityString(uint8 playability) {
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

uint32 UIEATraxScreen::GetStateString(uint8 state) {
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
        JukeBoxScrollerSlot *slot = new ("JukeBoxScrollerSlot", 0) JukeBoxScrollerSlot();
        slot->SetBacking(FEngFindObject(GetPackageName(), FEngHashString("MOUSE_REGION_%d", i + 1)));
        AddTrackSlot(slot, 0xE454E9A5, i);
        AddTrackSlot(slot, 0x66646FC4, i);
        AddTrackSlot(slot, 0x2890C8AF, i);
        AddTrackSlot(slot, 0x77BD189E, i);
        AddTrackSlot(slot, 0xF3EBDC4E, i);
        Tracks.AddSlot(slot);
    }

    for (int i = 0; i < NumSongs; i++) {
        Sound::stSongInfo *const *itr;
        JukeBoxScrollerDatum *datum = new ("JukeBoxScrollerDatum", 0) JukeBoxScrollerDatum();
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
    uint8 state = FEDatabase->GetAudioSettings()->PlayState;
    if (state == 0) {
        FEDatabase->GetAudioSettings()->PlayState = 1;
    } else {
        FEDatabase->GetAudioSettings()->PlayState = 0;
    }

    RefreshHeader();
    MControlPathfinder(true, 0, 0, 0).Send("EATraxInit");
}

void UIEATraxScreen::ScrollTracks(u32 msg) {
    if (msg == __PAD_UP__) {
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
    uint32 index = datum->SongIndex;
    JukeboxEntry *entry = nullptr;
    JukeboxEntry *playlist = FEDatabase->GetUserProfile(0)->Playlist;

    for (int i = 0; i < NumSongs; i++) {
        entry = &playlist[i];
        if (entry->SongIndex == index) {
            break;
        }
    }

    int play_flag = entry->PlayabilityField;

    JukeBoxScrollerSlot *slot = static_cast<JukeBoxScrollerSlot *>(Tracks.GetSelectedSlot());

    if (msg == __PAD_LEFT__) {
        play_flag--;
        if (play_flag < 0) {
            play_flag = 3;
        }
    } else if (msg == __PAD_RIGHT__) {
        play_flag++;
        if (play_flag > 3) {
            play_flag = 0;
        }
    }
    entry->PlayabilityField = play_flag;
    datum->PlayabilityField = play_flag;

    ScrollerDatumNode *node = datum->Strings.GetTail();
    FEngSNPrintf(node->String, 128, GetLocalizedString(GetPlaybilityString(entry->PlayabilityField)));
    Tracks.Update(true);
    MControlPathfinder(true, 0, 0, 0).Send("EATraxInit");
}

void UIEATraxScreen::MoveTrack(u32 msg) {
    ScrollerSlot *old_slot = Tracks.GetSelectedSlot();
    int oldSlotIndex = Tracks.GetSelectedSlotIndex();

    if (msg == __PAD_UP__) {
        Tracks.MovePrev();
    } else if (msg == __PAD_DOWN__) {
        Tracks.MoveNext();
    }

    ScrollerDatumNode *node;
    int num = 1;
    ScrollerDatum *datum = Tracks.GetFirstDatum();
    while (datum != Tracks.GetLastDatum()) {
        node = datum->Strings.GetHead();
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
        case FEHASH_INITCOMPLETE: {
            Tracks.HighlightSelected();
            break;
        }
        case __PAD_LTRIGGER__:
        case __PAD_RTRIGGER__:
            ScrollOrderState(msg);
            break;
        case __PAD_LEFT__:
        case __PAD_RIGHT__:
            ScrollTrackPlayability(msg);
            break;
        case __PAD_UP__:
        case __PAD_DOWN__:
            if (bTrackGrabbed) {
                MoveTrack(msg);
            } else {
                ScrollTracks(msg);
            }
            break;
        case __PAD_BACK__:
            if (OptionsDidNotChange()) {
                cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            } else {
                DialogInterface::ShowTwoButtons(GetPackageName(), "Dialog.fng", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0x775DBA97,
                                                dialog_message_no, dialog_message_no, first_dialog_button2, GetLocalizedString(0xE9CB802F));
            }
            MControlPathfinder(true, 0, 0, 0).Send("EATraxInit");
            break;
        case 0x775DBA97:
            RestoreOriginals();
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            break;
        case __PAD_BUTTON5__:
            bTrackGrabbed = !bTrackGrabbed;
            break;
        case __PAD_BUTTON4__:
            PreviewSong();
            break;
        case FEHASH_EXITCOMPLETE:
            MControlPathfinder(true, 0xFFFFFFFF, 0, 0).Send("EATraxInit");

            FEDatabase->SetOptionsDirty(FEDatabase->IsOptionsDirty() || !OptionsDidNotChange());

            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            break;
    }
}

bool UIEATraxScreen::OptionsDidNotChange() {
#ifdef EA_BUILD_A124
    return bMemCmp(FEDatabase->GetUserProfile(0)->Playlist, OriginalPlaylist, NumSongs * sizeof(JukeboxEntry)) == 0; // UNSOLVED
#else
    bool ret = bMemCmp(FEDatabase->GetUserProfile(0)->Playlist, OriginalPlaylist, NumSongs * sizeof(JukeboxEntry)) == 0;

    if (FEDatabase->GetAudioSettings()->PlayState != OriginalPlayState) {
        ret = false;
    }

    return ret;
#endif
}

void UIEATraxScreen::RestoreOriginals() {
    bMemCpy(FEDatabase->GetUserProfile(0)->Playlist, OriginalPlaylist, NumSongs * sizeof(JukeboxEntry));
#ifndef EA_BUILD_A124
    FEDatabase->GetAudioSettings()->PlayState = OriginalPlayState;
#endif
}
