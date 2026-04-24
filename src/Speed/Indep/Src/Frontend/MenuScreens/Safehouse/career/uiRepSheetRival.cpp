#include "uiRepSheetRival.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
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

struct FEObject;

FEImage *FEngFindImage(const char *pkg_name, int hash);
void FEngSetInvisible(FEObject *obj);
void FEngSetVisible(FEObject *obj);
void FEngSetTextureHash(FEImage *image, unsigned int hash);
void FEngSetTextureHash(const char *pkg_name, unsigned int obj_hash, unsigned int texture_hash);
void FEngSetLanguageHash(const char *pkg_name, unsigned int obj_hash, unsigned int lang_hash);
unsigned int FEngHashString(const char *format, ...);
int FEPrintf(const char *pkg_name, int hash, const char *fmt, ...);
unsigned long FEHashUpper(const char *str);
const char *GetLocalizedString(unsigned int hash);
void eLoadStreamingTexture(unsigned int *textures, int count, void (*callback)(void *), void *param, int pool);
void eUnloadStreamingTexture(unsigned int *textures, int count);
void eWaitForStreamingTexturePackLoading(const char *name);
unsigned int CalcLanguageHash(const char *prefix, GRaceParameters *pRaceParams);
void StartRace();

extern unsigned int iCurrentViewBin;

uiRepSheetRival::uiRepSheetRival(ScreenConstructorData *sd)
    : MenuScreen(sd), bIsInGame(sd->Arg != 0) //
      ,
      launch_race(nullptr) //
      ,
      RivalStreamer(sd->PackageFilename, bIsInGame) {
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
    unsigned int tex = GetDefeatedTexture();
    eUnloadStreamingTexture(&tex, 1);
}

eMenuSoundTriggers uiRepSheetRival::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if (bMidRivalFlow && msg == 0x911ab364) {
        return static_cast<eMenuSoundTriggers>(-1);
    }
    return maybe;
}

void uiRepSheetRival::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
    switch (msg) {
        case 0x406415e3:
            if (bMidRivalFlow) {
                new ERaceSheetOff();
                UCrc32 kind;
                kind.SetValue(0x20d60dbf);
                MFlowReadyForOutro msg;
                msg.Post(kind);
            } else if ((FEDatabase->GetGameMode() & 0x20000) != 0) {
                new EEnterBin(FEDatabase->GetCareerSettings()->GetCurrentBin() - 1);
                uiRepSheetRivalFlow::Get()->StartFlow(1);
            } else if (launch_race != nullptr) {
                if (bIsInGame) {
                    new ERaceSheetOff();
                    GManager::Get().StartRaceFromInGame(launch_race->GetEventHash());
                } else {
                    GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(launch_race);
                    GRaceDatabase::Get().SetStartupRace(race, kRaceContext_Career);
                    GRaceDatabase::Get().FreeCustomRace(race);
                    StartRace();
                }
            }
            break;
        case 0x911ab364:
            if (!bMidRivalFlow) {
                if (bOneOff) {
                    new EUnPause();
                } else if ((FEDatabase->GetGameMode() & 0x20000) == 0) {
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
    FEngSetInvisible(reinterpret_cast<FEObject *>(pDefeatedImg));
    FEngSetInvisible(reinterpret_cast<FEObject *>(pDefeatedImgBG));
    unsigned int defeatedTexture = GetDefeatedTexture();
    FEngSetTextureHash(pDefeatedImg, defeatedTexture);
    FEngSetTextureHash(pDefeatedImgBG, defeatedTexture);
    eLoadStreamingTexture(defeatedTexture, TextureLoadedCallback, reinterpret_cast<unsigned int>(this), 0);
    if (bIsInGame && bMidRivalFlow) {
        cFEng::Get()->QueuePackageMessage(0x34297cb0, GetPackageName(), nullptr);
    } else {
        if (FEDatabase->IsPostRivalMode()) {
            CarViewer::HideAllCars();
            iCurrentViewBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
            cFEng::Get()->QueuePackageMessage(0x0b21a45f, GetPackageName(), nullptr);
            cFEng::Get()->QueuePackageMessage(0xb4c144b1, GetPackageName(), nullptr);
        } else {
            cFEng::Get()->QueuePackageMessage(0xaf922178, GetPackageName(), nullptr);
        }
    }
    RefreshHeader();
}

void uiRepSheetRival::NotifyTextureLoaded() {
    FEngSetVisible(reinterpret_cast<FEObject *>(pDefeatedImg));
    FEngSetVisible(reinterpret_cast<FEObject *>(pDefeatedImgBG));
}

unsigned int uiRepSheetRival::GetDefeatedTexture() {
    int lang = GetCurrentLanguage();
    switch (lang) {
        case 1:
            return 0x87b81cd;
        case 2:
            return 0x87b846e;
        case 3:
            return 0x87b8ece;
        case 4:
            return 0x87bb8d4;
        case 5:
            return 0x87b79bd;
        case 6:
            return 0x87bb9bf;
        case 7:
            return 0x87b7723;
        case 12:
            return 0x87babfb;
        case 13:
            return 0x87b80ad;
        case 8:
        case 9:
        case 10:
        case 11:
        default:
            return 0x87b7d0a;
    }
}

void uiRepSheetRival::RefreshHeader() {
    GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(iCurrentViewBin);
    unsigned int num_boss_races = bin->GetBossRaceCount();
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

void uiRepSheetRival::SetupRace(unsigned int num, GRaceParameters *race) {
    unsigned int icon_hash = FEngHashString("EVENT_ICON_%d", num);
    unsigned int type_hash = FEngHashString("EVENT NAME_%d", num);
    unsigned int name_hash = FEngHashString("DATA_%d", num);
    unsigned int best_hash = FEngHashString("RIVAL_BEST_DATA_%d", num);
    FEngSetTextureHash(GetPackageName(), icon_hash, FEDatabase->GetRaceIconHash(race->GetRaceType()));
    FEngSetLanguageHash(GetPackageName(), type_hash, FEDatabase->GetRaceNameHash(race->GetRaceType()));
    FEngSetLanguageHash(GetPackageName(), name_hash, CalcLanguageHash("TRACKNAME_", race));
    Timer t(race->GetRivalBestTime());
    char buf[64];
    t.PrintToString(buf, 0);
    FEPrintf(GetPackageName(), best_hash, "%s", buf);
}

void uiRepSheetRival::TextureLoadedCallback(unsigned int tex) {
    reinterpret_cast<uiRepSheetRival *>(tex)->NotifyTextureLoaded();
}
