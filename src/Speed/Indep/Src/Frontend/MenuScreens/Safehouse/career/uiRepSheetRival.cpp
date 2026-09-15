#include "uiRepSheetRival.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EEnterBin.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOff.hpp"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Generated/Messages/MFlowReadyForOutro.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int iCurrentViewBin;

uiRepSheetRival::uiRepSheetRival(ScreenConstructorData *sd)
    : MenuScreen(sd), bIsInGame(sd->Arg != 0), launch_race(nullptr), RivalStreamer(sd->PackageFilename, bIsInGame) {
    new EFadeScreenOff(0x161a918);
    bMidRivalFlow = false;
    bOneOff = false;
    if (bIsInGame) {
        bMidRivalFlow = sd->Arg == 2;
        bOneOff = sd->Arg == 3;
    }
    Setup();
}

uiRepSheetRival::~uiRepSheetRival() {
    eWaitForStreamingTexturePackLoading(nullptr);
    eUnloadStreamingTexture(GetDefeatedTexture());
}

eMenuSoundTriggers uiRepSheetRival::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (bMidRivalFlow && msg == __PAD_BACK__) {
        return UISND_NONE;
    }
    return maybe;
}

void uiRepSheetRival::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    switch (msg) {
        case __PAD_ACCEPT__:
            if (bMidRivalFlow) {
                new ERaceSheetOff();
                MFlowReadyForOutro().Post(0x20d60dbf);
            } else if (FEDatabase->IsPostRivalMode()) {
                new EEnterBin(FEDatabase->GetCareerSettings()->GetCurrentBin() - 1);
                uiRepSheetRivalFlow::Get()->StartFlow(1);
            } else if (launch_race != nullptr) {
                if (bIsInGame) {
                    new ERaceSheetOff();
                    GManager::Get().StartRaceFromInGame(launch_race->GetEventHash());
                } else {
                    GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(launch_race);
                    GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_Career);
                    GRaceDatabase::Get().FreeCustomRace(race);
                    RaceStarter::StartRace();
                }
            }
            break;
        case __PAD_BACK__:
            if (!bMidRivalFlow) {
                if (bOneOff) {
                    new EUnPause();
                } else if (!FEDatabase->IsPostRivalMode()) {
                    if (bIsInGame) {
                        cFEng::Get()->QueuePackageSwitch("InGameReputationOverview.fng", 1, 0, false);
                    } else {
                        cFEng::Get()->QueuePackageSwitch("SafeHouseReputationOverview.fng", 0, 0, false);
                    }
                }
            }
            break;
    }
}

void uiRepSheetRival::Setup() {
    pRivalImg = FEngFindImage(GetPackageName(), 0xc1f62308);
    pDefeatedImg = FEngFindImage(GetPackageName(), 0x7fe4020f);
    pDefeatedImgBG = FEngFindImage(GetPackageName(), 0x26869897);
    pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);
    pBGImg = FEngFindImage(GetPackageName(), 0x2cbe1dd0);
    RivalStreamer.Init(iCurrentViewBin, pRivalImg, pTagImg, pBGImg);
    FEngSetInvisible(pDefeatedImg);
    FEngSetInvisible(pDefeatedImgBG);
    uint32 defeatedTexture = GetDefeatedTexture();
    FEngSetTextureHash(pDefeatedImg, defeatedTexture);
    FEngSetTextureHash(pDefeatedImgBG, defeatedTexture);
    eLoadStreamingTexture(defeatedTexture, TextureLoadedCallback, reinterpret_cast<uint32>(this), BMEMORY_DEFAULT_POOL);
    if (bIsInGame && bMidRivalFlow) {
        cFEng::Get()->QueuePackageMessage(0x34297cb0, GetPackageName(), nullptr);
    } else {
        if (FEDatabase->IsPostRivalMode()) {
            CarViewer::HideAllCars();
            iCurrentViewBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
            cFEng::Get()->QueuePackageMessage(0x0b21a45f, GetPackageName(), nullptr);
            const u32 FEObj_DEFEATED = 0xb4c144b1;
            cFEng::Get()->QueuePackageMessage(FEObj_DEFEATED, GetPackageName(), nullptr);
        } else {
            const u32 FEObj_CALLFRAMES = 0xaf922178;
            cFEng::Get()->QueuePackageMessage(FEObj_CALLFRAMES, GetPackageName(), nullptr);
        }
    }
    RefreshHeader();
}

void uiRepSheetRival::NotifyTextureLoaded() {
    FEngSetVisible(pDefeatedImg);
    FEngSetVisible(pDefeatedImgBG);
}

uint32 uiRepSheetRival::GetDefeatedTexture() {
    switch (GetCurrentLanguage()) {
        case eLANGUAGE_FRENCH:
            return 0x87b81cd;
        case eLANGUAGE_GERMAN:
            return 0x87b846e;
        case eLANGUAGE_ITALIAN:
            return 0x87b8ece;
        case eLANGUAGE_SPANISH:
            return 0x87bb8d4;
        case eLANGUAGE_DUTCH:
            return 0x87b79bd;
        case eLANGUAGE_SWEDISH:
            return 0x87bb9bf;
        case eLANGUAGE_DANISH:
            return 0x87b7723;
        case eLANGUAGE_POLISH:
            return 0x87babfb;
        case eLANGUAGE_FINNISH:
            return 0x87b80ad;
        case eLANGUAGE_KOREAN:
        case eLANGUAGE_CHINESE:
        case eLANGUAGE_JAPANESE:
        case eLANGUAGE_THAI:
        default:
            return 0x87b7d0a;
    }
}

void uiRepSheetRival::RefreshHeader() {
    GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(iCurrentViewBin);
    uint32 num_boss_races = bin->GetBossRaceCount();
    if (num_boss_races >= 5) {
        cFEng::Get()->QueuePackageMessage(0xe7177701, GetPackageName(), nullptr);
    } else if (num_boss_races == 4) {
        cFEng::Get()->QueuePackageMessage(0x9a1d3a40, GetPackageName(), nullptr);
    } else if (num_boss_races == 3) {
        cFEng::Get()->QueuePackageMessage(0x4d22fd7f, GetPackageName(), nullptr);
    } else {
        cFEng::Get()->QueuePackageMessage(0x0028c0be, GetPackageName(), nullptr);
    }
    for (unsigned int i = 0; i < bin->GetBossRaceCount(); i++) {
        unsigned int raceHash = bin->GetBossRaceHash(i);
        GRaceParameters *race = GRaceDatabase::mObj->GetRaceFromHash(raceHash);
        if (launch_race == nullptr) {
            launch_race = race;
        }
        if ((GRaceDatabase::Get(), GRaceDatabase::Get().IsCareerRaceComplete(race->GetEventHash()))) {
            char buf[64];
            bSNPrintf(buf, 64, "CROSSOUT_%d", i + 1);
            cFEng::Get()->QueuePackageMessage(FEHashUpper(buf), GetPackageName(), nullptr);
        }
        SetupRace(i + 1, race);
    }
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(GetPackageName(), 0xb514e2d8, "%s %$d", GetLocalizedString(0xce6b99b1), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xf91a59f6, "%s %$d", GetLocalizedString(0x073b79e0), FEDatabase->GetCareerSettings()->GetCash());
}

void uiRepSheetRival::SetupRace(uint32 num, GRaceParameters *race) {
    uint32 icon_hash = FEngHashString("EVENT_ICON_%d", num);
    uint32 type_hash = FEngHashString("EVENT NAME_%d", num);
    uint32 name_hash = FEngHashString("DATA_%d", num);
    uint32 best_hash = FEngHashString("RIVAL_BEST_DATA_%d", num);
    FEngSetTextureHash(GetPackageName(), icon_hash, FEDatabase->GetRaceIconHash(race->GetRaceType()));
    FEngSetLanguageHash(GetPackageName(), type_hash, FEDatabase->GetRaceNameHash(race->GetRaceType()));
    FEngSetLanguageHash(GetPackageName(), name_hash, CalcLanguageHash("TRACKNAME_", race));
    Timer t(race->GetRivalBestTime());
    char buf[64];
    t.PrintToString(buf, 0);
    FEPrintf(GetPackageName(), best_hash, "%s", buf);
}
