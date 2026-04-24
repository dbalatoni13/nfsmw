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
extern FEImage *FEngFindImage(const char *pkg_name, int hash);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
extern int FEngSNPrintf(char *buf, int size, const char *fmt, ...);
extern const char *GetLocalizedString(unsigned int hash);
extern int FEPrintf(const char *pkg_name, int hash, const char *fmt, ...);
extern unsigned int CalcLanguageHash(const char *prefix, GRaceParameters *rp);
extern bool DoesStringExist(unsigned int hash);
extern unsigned long FEHashUpper(const char *str);
extern int GetMikeMannBuild();
extern void StartRace();
extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool play);

extern void SetNumOpponents(void *custom, int num);
extern void SetCopsEnabled(void *custom, bool enabled);
extern const char *gOnlineMainMenu;
extern int bStrICmp(const char *, const char *);

struct GRaceSaveInfo {
    unsigned int mRaceHash;
    unsigned int mFlags;
    float mHighScores;
    unsigned short mTopSpeed;
    unsigned short mAverageSpeed;
};

inline void FEngSetVisible(const char *pkg_name, unsigned int obj_hash) {
    FEngSetVisible(FEngFindObject(pkg_name, obj_hash));
}

inline void FEngSetInvisible(const char *pkg_name, unsigned int obj_hash) {
    FEngSetInvisible(FEngFindObject(pkg_name, obj_hash));
}

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
    FEngSetLanguageHash(PackageFilename, 0xb71b576d, hash);
    const char *pkg = PackageFilename;
    unsigned int objHash = FEngHashString("TRACK_MAP");
    FEObject *obj = FEngFindObject(pkg, objHash);
    TrackMap = reinterpret_cast<FEMultiImage *>(obj);
    BuildPresetTrackList();
    RefreshHeader();
}

void UIQRTrackSelect::SetSelectedTrack(GRaceParameters *track) {
    if (track) {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(track->GetRaceType());
        settings->EventHash = track->GetEventHash();
    }
}

bool UIQRTrackSelect::IsRaceValidForMike(GRaceParameters *parms) {
    static const char *ValidForMikeMann[] = {"15.2.1", "14.2.1", "16.2.3", "15.1.1",     "16.1.1", "14.1.2",
                                             "5.1.1",  "11.4.2", "7.4.2",  "5.4.14.4.1", "10.7.1"};
    static const char *goddamcrap[] = {"16.1.1.r", "15.1.1"};

    int build = GetMikeMannBuild();
    if (build == 1) {
        for (int i = 0; i < 11; i++) {
            if (bStrICmp(parms->GetEventID(), ValidForMikeMann[i]) == 0) {
                return true;
            }
        }
    } else {
        build = GetMikeMannBuild();
        if (build == 2) {
            for (int i = 0; i < 2; i++) {
                if (bStrICmp(parms->GetEventID(), goddamcrap[i]) == 0) {
                    return true;
                }
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
    int isDDay = parms->GetIsDDayRace();
    if (isDDay) {
        return;
    }
    if (GetMikeMannBuild()) {
        if (!IsRaceValidForMike(parms)) {
            return;
        }
        SelectableTrack *node = new SelectableTrack(parms, isDDay, bin_num);
        Tracks.AddTail(node);
    } else {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        unsigned char region = settings->RegionFilterBits;
        if (parms->GetRegion() != region && region != kRaceRegion_NumRegions) {
            return;
        }
        unsigned int eventHash = parms->GetEventHash();
        bool isUnlocked = UnlockSystem::IsTrackUnlocked(static_cast<eUnlockFilters>(unlock_filter), eventHash, 0);
        SelectableTrack *node = new SelectableTrack(parms, !isUnlocked, bin_num);
        Tracks.AddTail(node);
        settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
        unsigned int settingsHash = settings->EventHash;
        if (parms->GetEventHash() == settingsHash) {
            pCurrentNode = node;
        }
    }
}

void UIQRTrackSelect::BuildPresetTrackList() {
    // TODO: normalized DWARF still differs around DeleteAllElements/GetHead ownership.
    Tracks.DeleteAllElements();
    int unlock_filter = 0;
    if (FEDatabase->IsCareerMode()) {
        unlock_filter = 2;
    } else if (FEDatabase->IsQuickRaceMode()) {
        unlock_filter = 1;
    } else if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
        unlock_filter = 4;
    }
    {
        int bin_num = 0x15;
        pCurrentNode = nullptr;
        do {
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
            bin_num--;
        } while (bin_num > 0);
    }
    if (!pCurrentNode) {
        pCurrentTrack = nullptr;
        if (Tracks.CountElements() > 0) {
            pCurrentNode = Tracks.GetHead();
        }
    }
    if (pCurrentNode) {
        pCurrentTrack = pCurrentNode->pRaceParams;
    }
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
        pCurrentTrack = prev->pRaceParams;
    } else if (dir == eSD_NEXT) {
        SelectableTrack *next = pCurrentNode->GetNext();
        if (next == Tracks.EndOfList()) {
            next = Tracks.GetHead();
        }
        pCurrentNode = next;
        pCurrentTrack = next->pRaceParams;
    }
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
    FEImage *img;
    img = FEngFindImage(PackageFilename, 0x91c4a50);
    FEngSetButtonTexture(img, 0x5bc);
    img = FEngFindImage(PackageFilename, 0x2d145be3);
    FEngSetButtonTexture(img, 0x682);

    RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    unsigned int hash;
    switch (settings->RegionFilterBits) {
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
    FEngSetLanguageHash(PackageFilename, 0x78008599, hash);
    FEngSetLanguageHash(PackageFilename, 0x4510987f, hash);
    FEPrintf(PackageFilename, 0x6f25a248, "%d", Tracks.GetNodeNumber(pCurrentNode));
    FEPrintf(PackageFilename, 0xb2037bdc, "%d", Tracks.CountElements());

    FEngSetLanguageHash(PackageFilename, 0xb5154998, FEDatabase->GetRaceNameHash(FEDatabase->RaceMode));

    FEngSetVisible(PackageFilename, 0x6b67d70b);

    if (!pCurrentTrack) {
        FEPrintf(PackageFilename, 0x6f25a248, "0");
        FEPrintf(PackageFilename, 0xb2037bdc, "0");
        FEPrintf(PackageFilename, 0x5e7b09c9, "");
        FEPrintf(PackageFilename, 0xdfb7a2e, "");
        FEPrintf(PackageFilename, 0xb5154999, "--");
        FEPrintf(PackageFilename, 0xb515499c, "%s", GetLocalizedString(0x472aa00a));
        FEngSetLanguageHash(PackageFilename, 0x68215623, 0xf9c0519a);
        FEngSetInvisible(PackageFilename, 0xe08434fc);
    } else {
        if (!pCurrentNode->bLocked) {
            FEngSetInvisible(PackageFilename, 0x6b67d70b);
            FEngSetVisible(PackageFilename, 0xe08434fc);
        } else {
            char rival_name_locdb[128];
            FEngSNPrintf(rival_name_locdb, 0x80, "blacklist_rival_%02d_aka", pCurrentNode->bin);
            const char *pkg = PackageFilename;
            const char *rival_label = GetLocalizedString(0xbd563be5);
            unsigned int aka_hash = FEHashUpper(rival_name_locdb);
            const char *aka_name = GetLocalizedString(aka_hash);
            FEPrintf(pkg, 0x68215623, rival_label, aka_name, pCurrentNode->bin);
            FEngSetInvisible(PackageFilename, 0xe08434fc);
        }

        unsigned int trackNameHash = CalcLanguageHash("TRACKNAME_", pCurrentTrack);
        if (DoesStringExist(trackNameHash)) {
            FEngSetLanguageHash(PackageFilename, 0x5e7b09c9, trackNameHash);
            FEngSetLanguageHash(PackageFilename, 0xdfb7a2e, trackNameHash);
        } else {
            FEPrintf(PackageFilename, 0x5e7b09c9, pCurrentTrack->GetEventID());
            FEPrintf(PackageFilename, 0xdfb7a2e, pCurrentTrack->GetEventID());
        }

        FEngSetInvisible(PackageFilename, 0xbbf970cd);

        bool kph = true;
        const char *distUnits;
        const char *speedUnits;
        if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
            distUnits = GetLocalizedString(0x8569a26a);
            speedUnits = GetLocalizedString(0x8569a25f);
        } else {
            distUnits = GetLocalizedString(0x867dcfd9);
            speedUnits = GetLocalizedString(0x8569ab44);
            kph = false;
        }

        const char *distFmt = "%$0.1f %s";
        const char *distPkg = PackageFilename;
        float distance = pCurrentTrack->GetRaceLengthMeters();
        if (kph) {
            distance *= 0.001f;
        } else {
            distance *= 0.000621371f;
        }
        FEPrintf(distPkg, 0xb5154999, distFmt, distance, distUnits);

        GRaceSaveInfo *info = GRaceDatabase::Get().GetScoreInfo(pCurrentTrack->GetEventHash());

        if (pCurrentTrack->GetRaceType() == GRace::kRaceType_P2P || pCurrentTrack->GetRaceType() == GRace::kRaceType_Circuit ||
            pCurrentTrack->GetRaceType() == GRace::kRaceType_Drag || pCurrentTrack->GetRaceType() == GRace::kRaceType_Knockout ||
            pCurrentTrack->GetRaceType() == GRace::kRaceType_Tollbooth) {
            Timer t(info->mHighScores);
            char buf[64];
            t.PrintToString(buf, 0);
            FEPrintf(PackageFilename, 0xb515499c, "%s", buf);
        } else if (pCurrentTrack->GetRaceType() == GRace::kRaceType_SpeedTrap) {
            float max_speed;
            if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
                max_speed = info->mHighScores;
            } else {
                max_speed = MPS2MPH(KPH2MPS(info->mHighScores));
            }
            FEngSetLanguageHash(PackageFilename, 0x28462c64, 0x512e823);
            FEPrintf(PackageFilename, 0xb515499c, "%$0.0f %s", max_speed, speedUnits);
        } else {
            FEPrintf(PackageFilename, 0xb515499c, "%s", GetLocalizedString(0x472aa00a));
        }

        if (pCurrentTrack->GetRaceType() == GRace::kRaceType_Circuit || pCurrentTrack->GetRaceType() == GRace::kRaceType_Knockout) {
            FEngSetLanguageHash(PackageFilename, 0x28462c64, 0xc5b5a177);
        }

        const char *pkg = PackageFilename;
        unsigned int raceIconHash = FEDatabase->GetRaceIconHash(pCurrentTrack->GetRaceType());
        img = FEngFindImage(pkg, 0x8007b4c);
        FEngSetTextureHash(img, raceIconHash);
    }
}

void UIQRTrackSelect::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
        case 0xe1fde1d1:
            if (pCurrentTrack) {
                bool isSplitQR = false;
                if ((FEDatabase->GetGameMode() & 4) != 0) {
                    isSplitQR = FEDatabase->iNumPlayers == 2;
                }
                GRace::Type rt = pCurrentTrack->GetRaceType();
                if (isSplitQR && (rt == GRace::kRaceType_Drag || pCurrentTrack->GetRaceType() == GRace::kRaceType_P2P ||
                                  pCurrentTrack->GetRaceType() == GRace::kRaceType_SpeedTrap)) {
                    GRaceCustom *custom = GRaceDatabase::Get().AllocCustomRace(pCurrentTrack);
                    SetNumOpponents(custom, 1);
                    SetCopsEnabled(custom, false);
                    GRaceDatabase::Get().SetStartupRace(custom, kRaceContext_QuickRace);
                    GRaceDatabase::Get().FreeCustomRace(custom);
                    cFEng::Get()->QueuePackageSwitch("PressStart.fng", 0, 0, false);
                    return;
                }
            }
            cFEng::Get()->QueuePackageSwitch("Track_Options.fng", static_cast<unsigned long>(reinterpret_cast<unsigned int>(pCurrentTrack)), 0,
                                             false);
            RefreshHeader();
            break;
        case 0x911ab364: {
            GRaceDatabase::Get().ClearStartupRace();
            RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
            settings->EventHash = 0;
            const char *pkg;
            if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
                pkg = "OL_MAIN.fng";
            } else {
                pkg = "MainMenu_Sub.fng";
            }
            cFEng::Get()->QueuePackageSwitch(pkg, 0, 0, false);
            break;
        }
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
        case 0x9120409e:
            ScrollTracks(eSD_PREV);
            break;
        case 0xb5971bf1:
            ScrollTracks(eSD_NEXT);
            break;
        case 0xd9feec59:
            ScrollRegions(eSD_NEXT);
            break;
        case 0x5073ef13:
            ScrollRegions(eSD_PREV);
            break;
        case 0xc98356ba:
            TrackMapStreamer.UpdateAnimation();
            break;
    }
}
