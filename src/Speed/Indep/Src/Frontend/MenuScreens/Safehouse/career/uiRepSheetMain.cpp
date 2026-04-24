#include "uiRepSheetMain.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOff.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct FEObject;

FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
FEImage *FEngFindImage(const char *pkg_name, int hash);
void FEngSetVisible(FEObject *obj);
void FEngSetInvisible(FEObject *obj);
void FEngSetTextureHash(FEImage *image, unsigned int hash);
void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool);
void FEngSetLanguageHash(const char *pkg_name, unsigned int obj_hash, unsigned int lang_hash);
int FEPrintf(const char *pkg_name, int hash, const char *fmt, ...);
int FEngSNPrintf(char *buffer, int buf_size, const char *fmt, ...);
unsigned int FEngHashString(const char *format, ...);
const char *GetLocalizedString(unsigned int hash);
void eUnloadStreamingTexture(unsigned int *textures, int count);
void WaitForResourceLoadingComplete();
unsigned char FEngGetLastButton(const char *pkg_name);

extern unsigned int iCurrentViewBin;
extern int selection;

struct RepSheetIcon : public IconOption {
    unsigned int id;

    RepSheetIcon(unsigned int tex_hash, unsigned int name_hash, unsigned int the_id) : IconOption(tex_hash, name_hash, 0) {
        id = the_id;
    }

    ~RepSheetIcon() override {}

    void React(const char *pkg, unsigned int data, FEObject *obj, unsigned int p1, unsigned int p2) override;
};

void RepSheetIcon::React(const char *pkg, unsigned int data, FEObject *obj, unsigned int p1, unsigned int p2) {
    if (data != 0xc407210)
        return;
    selection = id;
}

uiRepSheetMain::uiRepSheetMain(ScreenConstructorData *sd)
    : IconScrollerMenu(sd), bIsInGame(sd->Arg != 0), //
      bBossAvailable(false),                         //
      bBossBeaten(false),                            //
      DefeatedTextureHash(0),                        //
      RivalStreamer(sd->PackageFilename, bIsInGame) {
    if (bIsInGame) {
        Options.SetIdleColor(0xffffae40);
        Options.SetFadeColor(0x00ffae40);
        new EFadeScreenOff(0x14035fb);
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

eMenuSoundTriggers uiRepSheetMain::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if (bBossBeaten && msg == 0x7b6b89d7) {
        return static_cast<eMenuSoundTriggers>(-1);
    }
    return maybe;
}

void uiRepSheetMain::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
        case 0x911c0a4b:
            ScrollRival(static_cast<eScrollDir>(1));
            return;
        case 0x72619778:
            ScrollRival(static_cast<eScrollDir>(-1));
            return;
        case 0xe1fde1d1: {
            if (PrevButtonMessage == 0xc407210) {
                goto handle_selection;
            }
            if (PrevButtonMessage == 0x911ab364) {
                goto handle_eracesheet;
            }
            return;
        handle_selection:
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
            return;
        handle_eracesheet:
            if (bIsInGame) {
                new ERaceSheetOff();
                return;
            }
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            return;
        }
        case 0xc519bfc3:
            if (bBossBeaten) {
                return;
            }
            if (!bBossAvailable) {
                return;
            }
            if (!bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("SafeHouseRivalChallenge.fng", 0, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("InGameRivalChallenge.fng", 1, 0, false);
            }
            return;
        default:
            return;
    }
}

void uiRepSheetMain::Setup() {
    if (FEDatabase->GetCareerSettings()->GetCurrentBin() == 0xf) {
        FEngSetInvisible(GetPackageName(), 0x47b22fca);
        FEngSetInvisible(GetPackageName(), 0x72ad598c);
    }

    AddOption(new RepSheetIcon(0xefc9662e, 0x84e4a54c, 0));
    AddOption(new RepSheetIcon(0xd807e9b3, 0x216f1b81, 1));
    AddOption(new RepSheetIcon(0x021a4b0c, 0xe451941e, 2));
    AddOption(new RepSheetIcon(0xe97e4e83, 0x2d159737, 4));

    selection = 0;

    int lastButton = FEngGetLastButton(GetPackageName());

    if (bFadeInIconsImmediately) {
        Options.StartFadeIn();
    }

    Options.SetInitialPos(lastButton);
    IconScrollerMenu::RefreshHeader();

    if (bIsInGame) {
        FEngSetLanguageHash(GetPackageName(), 0xb71b576d, 0x2e3919e9);
    } else {
        FEngSetLanguageHash(GetPackageName(), 0xb71b576d, 0xcace5999);
    }

    pRivalImg = FEngFindImage(GetPackageName(), 0xc1f62308);
    pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);

    RivalStreamer.Init(iCurrentViewBin, pRivalImg, pTagImg, nullptr);

    FEngSetInvisible(GetPackageName(), 0x7fe4020f);

    DefeatedTextureHash = GetDefeatedTexture();
    FEngSetTextureHash(GetPackageName(), 0x7fe4020f, DefeatedTextureHash);

    eLoadStreamingTexture(DefeatedTextureHash, TextureLoadedCallback, reinterpret_cast<unsigned int>(this), 0);

    UpdateInfo();
}

void uiRepSheetMain::NotifyTextureLoaded() {
    FEngSetVisible(FEngFindObject(GetPackageName(), 0x7FE4020F));
}

unsigned int uiRepSheetMain::GetDefeatedTexture() {
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
            return 0x87b96bc;
        case 9:
            return 0x87b73c4;
        case 10:
            return 0x87b90ab;
        case 11:
            return 0x87bbc0d;
        default:
            return 0x87b7d0a;
    }
}

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
    FEPrintf(GetPackageName(), 0x322b18f9, "%u", completed_bounty);
    FEPrintf(GetPackageName(), 0xde7ad199, "%d", required_races);
    FEPrintf(GetPackageName(), 0x7242962e, "%d", required_challenges);
    FEPrintf(GetPackageName(), 0x055c6e5f, "%u", required_bounty);

    if (completed_races >= required_races) {
        FEngSetScript(GetPackageName(), 0x4c3b1536, 0xe6361f46, true);
    } else {
        FEngSetScript(GetPackageName(), 0x4c3b1536, 0x16a259, true);
    }
    if (completed_challenges >= required_challenges) {
        FEngSetScript(GetPackageName(), 0x4c3b1537, 0xe6361f46, true);
    } else {
        FEngSetScript(GetPackageName(), 0x4c3b1537, 0x16a259, true);
    }
    if (completed_bounty >= required_bounty) {
        FEngSetScript(GetPackageName(), 0x4c3b1538, 0xe6361f46, true);
    } else {
        FEngSetScript(GetPackageName(), 0x4c3b1538, 0x16a259, true);
    }

    char buf[32];
    if (bIsInGame) {
        FEngSNPrintf(buf, 32, GetLocalizedString(0x96ca2471), iCurrentViewBin);
    } else {
        FEngSNPrintf(buf, 32, GetLocalizedString(0x3a64de21), iCurrentViewBin);
    }
    FEPrintf(GetPackageName(), 0x242657ce, "%s", buf);

    const char *rival_name = GetLocalizedString(FEngHashString("BL_NAME_%d", iCurrentViewBin));
    const char *challenge_blurb = GetLocalizedString(FEngHashString("BL_BLURB_%d", iCurrentViewBin));
    FEPrintf(GetPackageName(), 0x7ac3d0c9, "%s", rival_name);
    FEPrintf(GetPackageName(), 0x79cf0442, "%s", challenge_blurb);

    unsigned int bossRaceCount = bin->GetBossRaceCount();
    bBossAvailable = false;
    for (unsigned int i = 0; i < bossRaceCount; i++) {
        unsigned int hash = bin->GetBossRaceHash(i);
        GRaceParameters *race = GRaceDatabase::Get().GetRaceFromHash(hash);
        int available = race->GetIsAvailable(GRace::kRaceContext_Career);
        bBossAvailable = (bBossAvailable != 0) | (available != 0);
    }

    bBossBeaten = false;
    if (FEDatabase->GetCareerSettings()->HasBeatenCareer() ||
        static_cast<int>(iCurrentViewBin) > static_cast<int>(FEDatabase->GetCareerSettings()->GetCurrentBin())) {
        bBossBeaten = true;
    }

    FEngSetInvisible(GetPackageName(), 0x34d4433b);

    if (bBossBeaten) {
        FEngSetInvisible(GetPackageName(), 0x55f6aa1a);
        FEngSetVisible(GetPackageName(), 0x34d4433b);
        cFEng::Get()->QueuePackageMessage(0xb4c144b1, GetPackageName(), nullptr);
    } else {
        if (bBossAvailable) {
            unsigned int msgHash = FEngHashString("BOSS_AVAILABLE");
            cFEng::Get()->QueuePackageMessage(msgHash, GetPackageName(), nullptr);
            FEngSetVisible(GetPackageName(), 0x55f6aa1a);
        } else {
            unsigned int msgHash = FEngHashString("BOSS_UNAVAILABLE");
            cFEng::Get()->QueuePackageMessage(msgHash, GetPackageName(), nullptr);
            FEngSetInvisible(GetPackageName(), 0x55f6aa1a);
        }
    }

    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(GetPackageName(), 0xb514e2d8, "%s %d", GetLocalizedString(0xce6b99b1), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xf91a59f6, "%s %d", GetLocalizedString(0x73b79e0), FEDatabase->GetCareerSettings()->GetCash());
}

void uiRepSheetMain::ScrollRival(eScrollDir dir) {
    unsigned char currentBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    unsigned int oldViewBin = iCurrentViewBin;
    if (currentBin == 15) {
        return;
    }
    if (dir == static_cast<eScrollDir>(1)) {
        iCurrentViewBin--;
        if (static_cast<int>(iCurrentViewBin) < 0 || iCurrentViewBin < currentBin) {
            iCurrentViewBin = 15;
        }
    } else if (dir == static_cast<eScrollDir>(-1)) {
        iCurrentViewBin++;
        if (static_cast<int>(iCurrentViewBin) > 15) {
            iCurrentViewBin = currentBin;
        }
    }
    if (oldViewBin != iCurrentViewBin) {
        if (dir == static_cast<eScrollDir>(1)) {
            FEngSetScript(GetPackageName(), 0xc1f62308, 0xaf9d73f2, true);
        } else if (dir == static_cast<eScrollDir>(-1)) {
            FEngSetScript(GetPackageName(), 0xc1f62308, 0x9e5e6b5f, true);
        }
        RivalStreamer.Init(iCurrentViewBin, pRivalImg, pTagImg, nullptr);
        UpdateInfo();
    }
}

void uiRepSheetMain::TextureLoadedCallback(unsigned int tex) {
    reinterpret_cast<uiRepSheetMain *>(tex)->NotifyTextureLoaded();
}
