// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRTrackSelect.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"

void FEngSetLanguageHash(const char *pkg_name, unsigned int obj_hash, unsigned int lang_hash);
extern int FEngGetLastButton(const char *pkg_name);
extern unsigned int FEngHashString(const char *format, ...);
extern FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern void FEngSetButtonTexture(FEImage *img, unsigned int hash);
extern FEImage *FEngFindImage(const char *pkg_name, unsigned int hash);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
extern int FEngSNPrintf(char *buf, int size, const char *fmt, ...);
extern const char *GetLocalizedString(unsigned int hash);
extern void FEPrintf(const char *pkg_name, unsigned int hash, const char *fmt, ...);
extern unsigned int CalcLanguageHash(const char *prefix, GRaceParameters *rp);
extern bool DoesStringExist(unsigned int hash);
extern unsigned long FEHashUpper(const char *str);
extern int GetMikeMannBuild();
extern void StartRace();
extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool play);
extern int IsEventAvailable(unsigned int hash);
extern int IsTrackUnlocked(int filter, unsigned int hash, int param);
extern void SetNumOpponents(void *custom, int num);
extern void SetCopsEnabled(void *custom, bool enabled);
extern const char *gOnlineMainMenu;

UIQRTrackSelect::UIQRTrackSelect(ScreenConstructorData *sd) : MenuScreen(sd) {
    TrackMapStreamer.Init(nullptr, nullptr, 0, 0);
    Tracks.InitList();
    pCurrentTrack = nullptr;
    pCurrentNode = nullptr;
    Setup();
}

UIQRTrackSelect::~UIQRTrackSelect() {}

void UIQRTrackSelect::Setup() {
    if (cFEng::Get()->IsPackagePushed("UI_OLViewTrack.fng")) {
        return;
    }
    GRace::Type raceMode = FEDatabase->RaceMode;
    unsigned int hash;
    switch (raceMode) {
    case GRace::kRaceType_P2P:
        hash = 0xc2d85652;
        break;
    case GRace::kRaceType_Circuit:
        hash = 0x3de80a85;
        break;
    case GRace::kRaceType_Drag:
        hash = 0x136c5c90;
        break;
    case GRace::kRaceType_Knockout:
        hash = 0xd6d65640;
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
    FEngSetLanguageHash(PackageFilename, 0xb71b576d, hash);
    unsigned int objHash = FEngHashString("TRACK_MAP");
    FEObject *obj = FEngFindObject(PackageFilename, objHash);
    TrackMap = reinterpret_cast<FEMultiImage *>(obj);
    BuildPresetTrackList();
    RefreshHeader();
}

void UIQRTrackSelect::SetSelectedTrack(GRaceParameters *track) {
    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    settings->EventHash = track->GetEventHash();
}

bool UIQRTrackSelect::IsRaceValidForMike(GRaceParameters *parms) {
    return true;
}

void UIQRTrackSelect::TryToAddTrack(GRaceParameters *parms, int unlock_filter, int bin_num) {
    GRace::Type raceType = parms->GetRaceType();
    if (raceType == FEDatabase->RaceMode) {
        if (GetMikeMannBuild()) {
            if (!IsRaceValidForMike(parms)) {
                return;
            }
        } else {
            unsigned int eventHash = parms->GetEventHash();
            if (!IsEventAvailable(eventHash)) {
                return;
            }
            eventHash = parms->GetEventHash();
            if (!IsTrackUnlocked(unlock_filter, eventHash, 0)) {
                return;
            }
        }
        SelectableTrack *node = new SelectableTrack(parms, false, bin_num);
        Tracks.AddTail(node);
    }
}

void UIQRTrackSelect::BuildPresetTrackList() {
    while (!Tracks.IsEmpty()) {
        SelectableTrack *node = Tracks.GetHead();
        node->Remove();
        delete node;
    }
    int unlockFilter = 0;
    unsigned int gameMode = FEDatabase->GetGameMode();
    if ((gameMode & 1) != 0) {
        unlockFilter = 2;
    } else if ((gameMode & 4) != 0) {
        unlockFilter = 1;
    } else if ((gameMode & 8) != 0 || (gameMode & 0x40) != 0) {
        unlockFilter = 4;
    }
    int binIdx = 0x15;
    pCurrentNode = nullptr;
    do {
        GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(binIdx);
        for (unsigned int i = 0; i < bin->GetWorldRaceCount(); i++) {
            unsigned int hash = bin->GetWorldRaceHash(i);
            GRaceParameters *rp = GRaceDatabase::Get().GetRaceFromHash(hash);
            TryToAddTrack(rp, unlockFilter, binIdx);
        }
        for (unsigned int i = 0; i < bin->GetBossRaceCount(); i++) {
            unsigned int hash = bin->GetBossRaceHash(i);
            GRaceParameters *rp = GRaceDatabase::Get().GetRaceFromHash(hash);
            TryToAddTrack(rp, unlockFilter, binIdx);
        }
        if (binIdx == 0x15) {
            binIdx = 0x10;
        }
        binIdx--;
    } while (binIdx > 0);
    if (!pCurrentNode) {
        pCurrentTrack = nullptr;
        int count = Tracks.CountElements();
        if (count > 0) {
            pCurrentNode = Tracks.GetHead();
        }
        if (!pCurrentNode) {
            goto skip;
        }
    }
    pCurrentTrack = pCurrentNode->pRaceParams;
skip:
    TrackMapStreamer.Init(pCurrentTrack, TrackMap, 0, 0);
}

void UIQRTrackSelect::ScrollTracks(eScrollDir dir) {
    int count = Tracks.CountElements();
    if (count < 1) {
        return;
    }
    GRaceParameters *oldTrack = pCurrentTrack;
    if (dir == eSD_PREV) {
        SelectableTrack *prev = pCurrentNode->GetPrev();
        if (prev == Tracks.EndOfList()) {
            prev = Tracks.GetTail();
        }
        pCurrentNode = prev;
    } else if (dir == eSD_NEXT) {
        SelectableTrack *next = pCurrentNode->GetNext();
        if (next == Tracks.EndOfList()) {
            next = Tracks.GetHead();
        }
        pCurrentNode = next;
    }
    pCurrentTrack = pCurrentNode->pRaceParams;
    if (oldTrack != pCurrentTrack) {
        TrackMapStreamer.Init(pCurrentTrack, TrackMap, 0, 0);
        RefreshHeader();
    }
}

void UIQRTrackSelect::ScrollRegions(eScrollDir dir) {
    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    unsigned char region = settings->RegionFilterBits;
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
    settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    settings->RegionFilterBits = region;
    BuildPresetTrackList();
    RefreshHeader();
}

void UIQRTrackSelect::RefreshHeader() {
    // Stub - complex function, implement later
}

void UIQRTrackSelect::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    switch (param1) {
    case 0x9120409e:
        ScrollTracks(eSD_PREV);
        break;
    case 0x5073ef13:
        ScrollRegions(eSD_PREV);
        break;
    case 0x406415e3:
        if (!pCurrentTrack) {
            return;
        }
        if (pCurrentNode->bLocked) {
            return;
        }
        SetSelectedTrack(pCurrentTrack);
        if (FEDatabase->RaceMode == GRace::kRaceType_None) {
            FEDatabase->RaceMode = pCurrentTrack->GetRaceType();
        }
        cFEng::Get()->QueuePackageMessage(0x2e76edfb, PackageFilename, nullptr);
        break;
    case 0x911ab364: {
        GRaceDatabase::Get().ClearStartupRace();
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        settings->EventHash = 0;
        const char *pkg;
        if ((FEDatabase->GetGameMode() & 8) == 0 && (FEDatabase->GetGameMode() & 0x40) == 0) {
            pkg = "MainMenu_Sub.fng";
        } else {
            pkg = "OL_MAIN.fng";
        }
        cFEng::Get()->QueuePackageSwitch(pkg, 0, 0, false);
        break;
    }
    case 0xc98356ba:
        TrackMapStreamer.UpdateAnimation();
        break;
    case 0xb5971bf1:
        ScrollTracks(eSD_NEXT);
        break;
    case 0xd9feec59:
        ScrollRegions(eSD_NEXT);
        break;
    case 0xe1fde1d1:
        if (pCurrentTrack) {
            bool isSplitQR = false;
            if ((FEDatabase->GetGameMode() & 4) != 0) {
                isSplitQR = FEDatabase->iNumPlayers == 2;
            }
            GRace::Type rt = pCurrentTrack->GetRaceType();
            if (isSplitQR && (rt == GRace::kRaceType_Drag || rt == GRace::kRaceType_P2P || rt == GRace::kRaceType_SpeedTrap)) {
                GRaceCustom *custom = GRaceDatabase::Get().AllocCustomRace(pCurrentTrack);
                SetNumOpponents(custom, 1);
                SetCopsEnabled(custom, false);
                GRaceDatabase::Get().SetStartupRace(custom, kRaceContext_QuickRace);
                GRaceDatabase::Get().FreeCustomRace(custom);
                cFEng::Get()->QueuePackageSwitch("PressStart.fng", 0, 0, false);
                return;
            }
        }
        cFEng::Get()->QueuePackageSwitch("Track_Options.fng", static_cast<unsigned long>(reinterpret_cast<unsigned int>(pCurrentTrack)), 0, false);
        RefreshHeader();
        break;
    }
}
