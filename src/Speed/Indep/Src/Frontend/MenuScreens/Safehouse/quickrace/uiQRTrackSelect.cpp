#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRTrackSelect.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"

UIQRTrackSelect::UIQRTrackSelect(ScreenConstructorData *sd) : MenuScreen(sd), Tracks(), pCurrentNode(nullptr), pCurrentTrack(nullptr) {
    Setup();
}

UIQRTrackSelect::~UIQRTrackSelect() {}

void UIQRTrackSelect::Setup() {
    if (cFEng::Get()->IsPackagePushed("UI_OLViewTrack.fng")) {
        return;
    }
    uint32 hash;
    switch (FEDatabase->RaceMode) {
        case GRace::kRaceType_Circuit:
            hash = 0x3de80a85;
            break;
        case GRace::kRaceType_Drag:
            hash = 0x136c5c90;
            break;
        case GRace::kRaceType_Knockout:
            hash = 0xd6d65640;
            break;
        case GRace::kRaceType_P2P:
            hash = 0xc2d85652;
            break;
        case GRace::kRaceType_Tollbooth:
            hash = 0xe3afadc9;
            break;
        case GRace::kRaceType_SpeedTrap:
            hash = 0x3070453a;
            break;
        default:
            hash = 0;
            break;
    }
    const u32 FEObj_TITLEGROUP = __TITLE_GROUP__;
    FEngSetLanguageHash(GetPackageName(), FEObj_TITLEGROUP, hash);
    TrackMap = reinterpret_cast<FEMultiImage *>(FEngFindObject(GetPackageName(), FEngHashString("TRACK_MAP")));
    BuildPresetTrackList();
    RefreshHeader();
}

void UIQRTrackSelect::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case FEHASH_EXITCOMPLETE:
            if (pCurrentTrack != nullptr) {
                if (FEDatabase->IsSplitScreenMode() &&
                    (pCurrentTrack->GetRaceType() == GRace::kRaceType_Drag || pCurrentTrack->GetRaceType() == GRace::kRaceType_P2P ||
                     pCurrentTrack->GetRaceType() == GRace::kRaceType_SpeedTrap)) {
                    GRaceCustom *custom_race = GRaceDatabase::Get().AllocCustomRace(pCurrentTrack);
                    custom_race->SetNumOpponents(1);
                    custom_race->SetCopsEnabled(false);
                    GRaceDatabase::Get().SetStartupRace(custom_race, GRace::kRaceContext_QuickRace);
                    GRaceDatabase::Get().FreeCustomRace(custom_race);
                    cFEng::Get()->QueuePackageSwitch("PressStart.fng", 0, 0, false);
                    return;
                }
            }
            cFEng::Get()->QueuePackageSwitch("Track_Options.fng", reinterpret_cast<u32>(pCurrentTrack), 0, false);
            RefreshHeader();
            break;
        case __PAD_BACK__:
            GRaceDatabase::Get().ClearStartupRace();
            FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->EventHash = 0;
            if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                cFEng::Get()->QueuePackageSwitch("OL_MAIN.fng", 0, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            }
            break;
        case __PAD_ACCEPT__:
            if (pCurrentTrack == nullptr) {
                return;
            }
            if (pCurrentNode->bLocked) {
                return;
            }
            SetSelectedTrack(pCurrentTrack);
            if (FEDatabase->RaceMode == GRace::kRaceType_None) {
                FEDatabase->RaceMode = pCurrentTrack->GetRaceType();
            }
            cFEng::Get()->QueuePackageMessage(0x2e76edfb, GetPackageName(), nullptr);
            break;
        case __PAD_LEFT__:
            ScrollTracks(eSD_PREV);
            break;
        case __PAD_RIGHT__:
            ScrollTracks(eSD_NEXT);
            break;
        case __PAD_RTRIGGER__:
            ScrollRegions(eSD_NEXT);
            break;
        case __PAD_LTRIGGER__:
            ScrollRegions(eSD_PREV);
            break;
        case FEMSG_SCREEN_TICK:
            TrackMapStreamer.UpdateAnimation();
            break;
    }
}

void UIQRTrackSelect::SetSelectedTrack(GRaceParameters *track) {
    if (track != nullptr) {
        RaceSettings *t = FEDatabase->GetQuickRaceSettings(track->GetRaceType());
        t->EventHash = track->GetEventHash();
    }
}

bool UIQRTrackSelect::IsRaceValidForMike(GRaceParameters *parms) {
    if (GetMikeMannBuild() == 1) {
        static char *ValidForMikeMann[] = {"15.2.1", "14.2.1", "16.2.3", "15.1.1",     "16.1.1", "14.1.2",
                                           "5.1.1",  "11.4.2", "7.4.2",  "5.4.14.4.1", "10.7.1"};
        for (int i = 0; i < 11; i++) {
            if (bStrICmp(parms->GetEventID(), ValidForMikeMann[i]) == 0) {
                return true;
            }
        }
    } else if (GetMikeMannBuild() == 2) {
        static char *goddamcrap[] = {"16.1.1.r", "15.1.1"};
        for (int i = 0; i < 2; i++) {
            if (bStrICmp(parms->GetEventID(), goddamcrap[i]) == 0) {
                return true;
            }
        }
    }

    return false;
}

void UIQRTrackSelect::TryToAddTrack(GRaceParameters *parms, int unlock_filter, int bin_num) {
    if (!UnlockSystem::IsEventAvailable(parms->GetEventHash())) {
        return;
    }
    if (parms->GetNeverInQuickRace()) {
        return;
    }
    if (parms->GetRaceType() != FEDatabase->RaceMode) {
        return;
    }
    if (parms->GetIsDDayRace()) {
        return;
    }
    if (GetMikeMannBuild() != 0) {
        if (IsRaceValidForMike(parms)) {
            Tracks.AddTail(new ("SelectableTrack", 0) SelectableTrack(parms, false, bin_num));
        }
    } else {
        uint8 regionFilter = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->RegionFilterBits;
        if (parms->GetRegion() == regionFilter || regionFilter == GRace::kRaceRegion_NumRegions) {
            bool unlocked = UnlockSystem::IsTrackUnlocked(static_cast<eUnlockFilters>(unlock_filter), parms->GetEventHash(), 0);
            Tracks.AddTail(new ("SelectableTrack", 0) SelectableTrack(parms, !unlocked, bin_num));
            uint32 hash = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->EventHash;
            if (parms->GetEventHash() == hash) {
                pCurrentNode = Tracks.GetTail();
            }
        }
    }
}

void UIQRTrackSelect::BuildPresetTrackList() {
    Tracks.DeleteAllElements();
    int unlock_filter = 0;
    if (FEDatabase->IsCareerMode()) {
        unlock_filter = 2;
    } else if (FEDatabase->IsQuickRaceMode()) {
        unlock_filter = 1;
    } else if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        unlock_filter = 4;
    }

    pCurrentNode = nullptr;
    for (int bin_num = 0x15; bin_num > 0; bin_num--) {
        GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(bin_num);
        for (unsigned int i = 0; i < bin->GetWorldRaceCount(); i++) {
            unsigned int raceHash = bin->GetWorldRaceHash(i);
            GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromHash(raceHash);
            TryToAddTrack(parms, unlock_filter, bin_num);
        }
        for (unsigned int i = 0; i < bin->GetBossRaceCount(); i++) {
            unsigned int raceHash = bin->GetBossRaceHash(i);
            GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromHash(raceHash);
            TryToAddTrack(parms, unlock_filter, bin_num);
        }
        if (bin_num == 0x15) {
            bin_num = 0x10;
        }
    }

    if (pCurrentNode == nullptr) {
        pCurrentTrack = nullptr;
        if (Tracks.CountElements() > 0) {
            pCurrentNode = Tracks.GetHead();
        }
    }
    if (pCurrentNode != nullptr) {
        pCurrentTrack = pCurrentNode->pRaceParams;
    }
    Tracks.IsEmpty(); // unknown
    TrackMapStreamer.Init(pCurrentTrack, TrackMap, 0, 0);
}

void UIQRTrackSelect::RefreshHeader() {
    // TODO
    const u32 FEObj_NUMBER = 0;
    const u32 FEObj_NUMBEROF = 0;
    const u32 FEObj_ICONTITLE = 0x5E7B09C9;
    const u32 FEObj_ICONTITLESHADOW = 0x0DFB7A2E;
    const u32 FEObj_option01 = 0;
    const u32 FEObj_option02 = 0;
    const u32 FEObj_option03 = 0;
    const u32 FEObj_option04 = 0;
    const u32 FEObj_option05 = 0;
    const u32 FEObj_option06 = 0;
    const u32 FEObj_optionheader05 = 0;
    const u32 FEObj_Accept = 0;

    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x91c4a50), 0x5bc);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x2d145be3), 0x682);

    uint32 hash;
    switch (FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->RegionFilterBits) {
        case 0:
            hash = 0xa6850651;
            break;
        case 1:
            hash = 0xa5c20e7d;
            break;
        case 2:
            hash = 0x8663faef;
            break;
        case 3:
            hash = 0x632dd19b;
            break;
        default:
            hash = 0;
            break;
    }

    // TODO
    const u32 FEObj_LEVEL_TITLE = 0;
    const u32 FEObj_LEVEL_TITLE_SHADOW = 0;

    FEngSetLanguageHash(GetPackageName(), 0x78008599, hash);
    FEngSetLanguageHash(GetPackageName(), 0x4510987f, hash);
    FEPrintf(GetPackageName(), 0x6f25a248, "%d", Tracks.GetNodeNumber(pCurrentNode));
    FEPrintf(GetPackageName(), 0xb2037bdc, "%d", Tracks.CountElements());

    FEngSetLanguageHash(GetPackageName(), 0xb5154998, FEDatabase->GetRaceNameHash(FEDatabase->RaceMode));

    FEngSetVisible(GetPackageName(), 0x6b67d70b);

    if (pCurrentTrack == nullptr) {
        FEPrintf(GetPackageName(), 0x6f25a248, "0");
        FEPrintf(GetPackageName(), 0xb2037bdc, "0");
        FEPrintf(GetPackageName(), FEObj_ICONTITLE, "");
        FEPrintf(GetPackageName(), FEObj_ICONTITLESHADOW, "");
        FEPrintf(GetPackageName(), 0xb5154999, "--");
        FEPrintf(GetPackageName(), 0xb515499c, "%s", GetLocalizedString(0x472aa00a));
        FEngSetLanguageHash(GetPackageName(), 0x68215623, 0xf9c0519a);
        FEngSetInvisible(GetPackageName(), 0xe08434fc);
        return;
    }

    if (!pCurrentNode->bLocked) {
        FEngSetInvisible(GetPackageName(), 0x6b67d70b);
        FEngSetVisible(GetPackageName(), 0xe08434fc);
    } else {
        char rival_name_locdb[128];
        FEngSNPrintf(rival_name_locdb, sizeof(rival_name_locdb), "blacklist_rival_%02d_aka", pCurrentNode->bin);
        FEPrintf(GetPackageName(), 0x68215623, GetLocalizedString(0xbd563be5), GetLocalizedString(FEHashUpper(rival_name_locdb)), pCurrentNode->bin);
        FEngSetInvisible(GetPackageName(), 0xe08434fc);
    }

    hash = CalcLanguageHash("TRACKNAME_", pCurrentTrack);
    if (DoesStringExist(hash)) {
        FEngSetLanguageHash(GetPackageName(), FEObj_ICONTITLE, hash);
        FEngSetLanguageHash(GetPackageName(), FEObj_ICONTITLESHADOW, hash);
    } else {
        FEPrintf(GetPackageName(), FEObj_ICONTITLE, pCurrentTrack->GetEventID());
        FEPrintf(GetPackageName(), FEObj_ICONTITLESHADOW, pCurrentTrack->GetEventID());
    }

    FEngSetInvisible(GetPackageName(), 0xbbf970cd);

    const char *distUnits;
    const char *speedUnits;
    bool kph = true;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        distUnits = GetLocalizedString(0x8569a26a);
        speedUnits = GetLocalizedString(0x8569a25f);
    } else {
        distUnits = GetLocalizedString(0x867dcfd9);
        speedUnits = GetLocalizedString(0x8569ab44);
        kph = false;
    }

    FEPrintf(GetPackageName(), 0xb5154999, "%$0.1f %s", pCurrentTrack->GetRaceLengthMeters() * (kph ? 0.001f : 0.000621371f), distUnits);

    GRaceSaveInfo *info = GRaceDatabase::Get().GetScoreInfo(pCurrentTrack->GetEventHash());

    if (pCurrentTrack->GetRaceType() == GRace::kRaceType_P2P || pCurrentTrack->GetRaceType() == GRace::kRaceType_Circuit ||
        pCurrentTrack->GetRaceType() == GRace::kRaceType_Drag || pCurrentTrack->GetRaceType() == GRace::kRaceType_Knockout ||
        pCurrentTrack->GetRaceType() == GRace::kRaceType_Tollbooth) {
        Timer t(info->mHighScores.mBestTime);
        char buf[64];
        t.PrintToString(buf, 0);
        FEPrintf(GetPackageName(), 0xb515499c, "%s", buf);
    } else if (pCurrentTrack->GetRaceType() == GRace::kRaceType_SpeedTrap) {
        float max_speed;
        if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
            max_speed = info->mHighScores.mBestSpeed;
        } else {
            max_speed = MPS2MPH(KPH2MPS(info->mHighScores.mBestSpeed));
        }
        FEngSetLanguageHash(GetPackageName(), 0x28462c64, 0x512e823);
        FEPrintf(GetPackageName(), 0xb515499c, "%$0.0f %s", max_speed, speedUnits);
    } else {
        FEPrintf(GetPackageName(), 0xb515499c, "%s", GetLocalizedString(0x472aa00a));
    }

    if (pCurrentTrack->GetRaceType() == GRace::kRaceType_Circuit || pCurrentTrack->GetRaceType() == GRace::kRaceType_Knockout) {
        FEngSetLanguageHash(GetPackageName(), 0x28462c64, 0xc5b5a177);
    }

    FEngSetTextureHash(GetPackageName(), 0x8007b4c, FEDatabase->GetRaceIconHash(pCurrentTrack->GetRaceType()));
}

// UNSOLVED
void UIQRTrackSelect::ScrollTracks(eScrollDir dir) {
    if (Tracks.CountElements() < 1) {
        return;
    }
    GRaceParameters *oldTrack = pCurrentTrack;

    if (dir == eSD_PREV) {
        pCurrentNode = Tracks.GetPrevCircular(pCurrentNode);
        pCurrentTrack = pCurrentNode->pRaceParams;
    } else if (dir == eSD_NEXT) {
        pCurrentNode = Tracks.GetNextCircular(pCurrentNode);
        pCurrentTrack = pCurrentNode->pRaceParams;
    }

    if (oldTrack != pCurrentTrack) {
        TrackMapStreamer.Init(pCurrentTrack, TrackMap, 0, 0);
        RefreshHeader();
    }
}

void UIQRTrackSelect::ScrollRegions(eScrollDir dir) {
    uint8 region = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->RegionFilterBits;
    if (dir == eSD_PREV) {
        if (region == 0) {
            region = 3;
        } else {
            region--;
        }
    } else if (dir == eSD_NEXT) {
        if (region == 3) {
            region = 0;
        } else {
            region++;
        }
    }
    FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->RegionFilterBits = region;
    BuildPresetTrackList();
    RefreshHeader();
}
