#include "uiRepSheetMain.hpp"

#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOff.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

static int selection = 0;
int iCurrentViewBin = 0;

void RepSheetIcon::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data != __BUTTON_PRESSED__)
        return;
    selection = id;
}

uiRepSheetMain::uiRepSheetMain(ScreenConstructorData *sd)
    : IconScrollerMenu(sd), bIsInGame(sd->Arg != 0), bBossAvailable(false), bBossBeaten(false), DefeatedTextureHash(0),
      RivalStreamer(sd->PackageFilename, bIsInGame) {
    if (bIsInGame) {
        Options.SetIdleColor(0xffffae40);
        Options.SetFadeColor(0x00ffae40);
        new EFadeScreenOff(FEHASH_15_IN);
    } else {
        RideInfo ride;
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        stable->BuildRideForPlayer(FEDatabase->GetCareerSettings()->GetCurrentCar(), 0, &ride);
        CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
        GarageMainScreen::GetInstance()->CancelCameraPush();
    }
    Setup();
}

uiRepSheetMain::~uiRepSheetMain() {
    eUnloadStreamingTexture(DefeatedTextureHash);
    WaitForResourceLoadingComplete();
}

eMenuSoundTriggers uiRepSheetMain::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (bBossBeaten && msg == 0x7b6b89d7) {
        return UISND_NONE;
    }
    return maybe;
}

void uiRepSheetMain::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
        case __PAD_DOWN__:
            ScrollRival(eSD_NEXT);
            break;
        case __PAD_UP__:
            ScrollRival(eSD_PREV);
            break;
        case FEHASH_EXITCOMPLETE:
            switch (PrevButtonMessage) {
                case __BUTTON_PRESSED__:
                    if (selection == 0) {
                        if (!bIsInGame) {
                            cFEng::Get()->QueuePackageSwitch("SafeHouseRaceSheet.fng", 0, 0, false);
                        } else {
                            cFEng::Get()->QueuePackageSwitch("InGameRaceSheet.fng", 1, 0, false);
                        }
                    } else if (selection == 1) {
                        if (!bIsInGame) {
                            cFEng::Get()->QueuePackageSwitch("SafeHouseMilestones.fng", 0, 0, false);
                        } else {
                            cFEng::Get()->QueuePackageSwitch("InGameMilestones.fng", 1, 0, false);
                        }
                    } else if (selection == 2) {
                        if (!bIsInGame) {
                            cFEng::Get()->QueuePackageSwitch("SafeHouseBounty.fng", 0, 0, false);
                        } else {
                            cFEng::Get()->QueuePackageSwitch("InGameBounty.fng", 1, 0, false);
                        }
                    } else if (selection == 4) {
                        if (!bIsInGame) {
                            cFEng::Get()->QueuePackageSwitch("SafeHouseRivalBio.fng", 0, 0, false);
                        } else {
                            cFEng::Get()->QueuePackageSwitch("InGameRivalBio.fng", 1, 0, false);
                        }
                    }
                    break;

                case __PAD_BACK__:
                    if (bIsInGame) {
                        new ERaceSheetOff();
                        break;
                    }
                    cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
                    break;
            }
            break;
        case __PAD_BUTTON4__:
            if (bBossBeaten) {
                break;
            }
            if (!bBossAvailable) {
                break;
            }
            if (!bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("SafeHouseRivalChallenge.fng", 0, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("InGameRivalChallenge.fng", 1, 0, false);
            }
            break;
    }
}

void uiRepSheetMain::Setup() {

    if (FEDatabase->GetCareerSettings()->GetCurrentBin() == 0xf) {
        FEngSetInvisible(GetPackageName(), 0x47b22fca);
        FEngSetInvisible(GetPackageName(), 0x72ad598c);
    }

    AddOption(new ("RepSheetIcon", 0) RepSheetIcon(0xefc9662e, 0x84e4a54c, 0));
    AddOption(new ("RepSheetIcon", 0) RepSheetIcon(0xd807e9b3, 0x216f1b81, 1));
    AddOption(new ("RepSheetIcon", 0) RepSheetIcon(0x021a4b0c, 0xe451941e, 2));
    AddOption(new ("RepSheetIcon", 0) RepSheetIcon(0xe97e4e83, 0x2d159737, 4));

    selection = 0;
    SetInitialOption(FEngGetLastButton(GetPackageName()));

    IconScrollerMenu::RefreshHeader();

    if (bIsInGame) {
        FEngSetLanguageHash(GetPackageName(), __TITLE_GROUP__, 0x2e3919e9);
    } else {
        FEngSetLanguageHash(GetPackageName(), __TITLE_GROUP__, 0xcace5999);
    }

    pRivalImg = FEngFindImage(GetPackageName(), 0xc1f62308);
    pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);

    RivalStreamer.Init(iCurrentViewBin, pRivalImg, pTagImg, nullptr);

    const u32 FEObj_STAMPMASTER = 0x7FE4020F;
    FEngSetInvisible(GetPackageName(), FEObj_STAMPMASTER);
    DefeatedTextureHash = GetDefeatedTexture();
    FEngSetTextureHash(GetPackageName(), FEObj_STAMPMASTER, DefeatedTextureHash);

    eLoadStreamingTexture(DefeatedTextureHash, TextureLoadedCallback, reinterpret_cast<uint32>(this), BMEMORY_DEFAULT_POOL);

#ifndef EA_BUILD_A124
    UpdateInfo();
#endif
}

void uiRepSheetMain::NotifyTextureLoaded() {
    const u32 FEObj_STAMPMASTER = 0x7FE4020F;

    FEngSetVisible(GetPackageName(), FEObj_STAMPMASTER);
}

uint32 uiRepSheetMain::GetDefeatedTexture() {
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
            return 0x87b96bc;
        case eLANGUAGE_CHINESE:
            return 0x87b73c4;
        case eLANGUAGE_JAPANESE:
            return 0x87b90ab;
        case eLANGUAGE_THAI:
            return 0x87bbc0d;
        default:
            return 0x87b7d0a;
    }
}

#ifndef EA_BUILD_A124
void uiRepSheetMain::UpdateInfo() {
    GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(iCurrentViewBin);
    int completed_races = bin->GetAwardedRaceWins();
    int required_races = bin->GetRequiredRaceWins();
    int completed_challenges = bin->GetCompletedChallenges();
    int required_challenges = bin->GetRequiredChallenges();
    int completed_bounty = FEDatabase->GetPlayerCarStable(0)->GetTotalBounty();
    int required_bounty = bin->GetRequiredBounty();

    FEPrintf(GetPackageName(), 0x15d80973, "%d", completed_races);
    FEPrintf(GetPackageName(), 0xd802fba8, "%d", completed_challenges);
    FEPrintf(GetPackageName(), 0x322b18f9, "%$d", completed_bounty);
    FEPrintf(GetPackageName(), 0xde7ad199, "%d", required_races);
    FEPrintf(GetPackageName(), 0x7242962e, "%d", required_challenges);
    FEPrintf(GetPackageName(), 0x055c6e5f, "%$d", required_bounty);

    if (completed_races >= required_races) {
        FEngSetScript(GetPackageName(), 0x4c3b1536, 0xe6361f46, true);
    } else {
        FEngSetScript(GetPackageName(), 0x4c3b1536, FEHASH_HIDE, true);
    }
    if (completed_challenges >= required_challenges) {
        FEngSetScript(GetPackageName(), 0x4c3b1537, 0xe6361f46, true);
    } else {
        FEngSetScript(GetPackageName(), 0x4c3b1537, FEHASH_HIDE, true);
    }
    if (completed_bounty >= required_bounty) {
        FEngSetScript(GetPackageName(), 0x4c3b1538, 0xe6361f46, true);
    } else {
        FEngSetScript(GetPackageName(), 0x4c3b1538, FEHASH_HIDE, true);
    }

    char buf[32];
    if (bIsInGame) {
        FEngSNPrintf(buf, 32, GetLocalizedString(0x96ca2471), iCurrentViewBin);
    } else {
        FEngSNPrintf(buf, 32, GetLocalizedString(0x3a64de21), iCurrentViewBin);
    }

    const u32 FEObj_TITLE_GROUP = 0x242657ce;

    FEPrintf(GetPackageName(), FEObj_TITLE_GROUP, "%s", buf);

    char *rival_name = GetLocalizedString(FEngHashString("BLACKLIST_RIVAL_%02d_AKA", iCurrentViewBin));
    char *challenge_blurb = GetLocalizedString(FEngHashString("BLACKLIST_RIVAL_%02d_CHALLENGE", iCurrentViewBin));
    FEPrintf(GetPackageName(), 0x7ac3d0c9, "%s", rival_name);
    FEPrintf(GetPackageName(), 0x79cf0442, "%s", challenge_blurb);

    unsigned int bossRaceCount = bin->GetBossRaceCount();
    bBossAvailable = false;
    for (unsigned int i = 0; i < bossRaceCount; i++) {
        unsigned int hash = bin->GetBossRaceHash(i);
        GRaceParameters *race = GRaceDatabase::Get().GetRaceFromHash(hash);
        bBossAvailable |= race->GetIsAvailable(GRace::kRaceContext_Career);
    }

    bBossBeaten = false;
    if (FEDatabase->GetCareerSettings()->HasBeatenCareer() || iCurrentViewBin > FEDatabase->GetCareerSettings()->GetCurrentBin()) {
        bBossBeaten = true;
    }

    FEngSetInvisible(GetPackageName(), 0x34d4433b);

    if (bBossBeaten) {
        FEngSetInvisible(GetPackageName(), 0x55f6aa1a);
        FEngSetVisible(GetPackageName(), 0x34d4433b);
        cFEng::Get()->QueuePackageMessage(0xb4c144b1, GetPackageName(), nullptr);
    } else {
        if (bBossAvailable) {
            unsigned int messageHash = FEngHashString("RIVAL_CHALLENGE_DEACTIVATED");
            cFEng::Get()->QueuePackageMessage(messageHash, GetPackageName(), nullptr);
            FEngSetVisible(GetPackageName(), 0x55f6aa1a);
        } else {
            unsigned int messageHash = FEngHashString("RIVAL_CHALLENGE_ACTIVATED");
            cFEng::Get()->QueuePackageMessage(messageHash, GetPackageName(), nullptr);
            FEngSetInvisible(GetPackageName(), 0x55f6aa1a);
        }
    }

    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(GetPackageName(), 0xb514e2d8, "%s %$d", GetLocalizedString(0xce6b99b1), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xf91a59f6, "%s %$d", GetLocalizedString(0x73b79e0), FEDatabase->GetCareerSettings()->GetCash());
}
#endif

void uiRepSheetMain::ScrollRival(eScrollDir dir) {
    if (FEDatabase->GetCareerSettings()->GetCurrentBin() == 15) {
        return;
    }

    int old_bin = iCurrentViewBin;
    if (dir == eSD_NEXT) {
        iCurrentViewBin--;
        if (iCurrentViewBin < 0 || iCurrentViewBin < FEDatabase->GetCareerSettings()->GetCurrentBin()) {
            iCurrentViewBin = 15;
        }
    } else if (dir == eSD_PREV) {
        iCurrentViewBin++;
        if (iCurrentViewBin > 15) {
            iCurrentViewBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
        }
    }

    if (old_bin != iCurrentViewBin) {
        if (dir == eSD_NEXT) {
            FEngSetScript(GetPackageName(), 0xc1f62308, 0xaf9d73f2, true);
        } else if (dir == eSD_PREV) {
            FEngSetScript(GetPackageName(), 0xc1f62308, 0x9e5e6b5f, true);
        }
        RivalStreamer.Init(iCurrentViewBin, pRivalImg, pTagImg, nullptr);
#ifdef EA_BUILD_A124
        RefreshHeader();
#else
        UpdateInfo();
#endif
    }
}
