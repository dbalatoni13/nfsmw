#include "uiRepSheetMain.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct FEObject;

FEObject* FEngFindObject(const char* pkg_name, unsigned int hash);
FEImage* FEngFindImage(const char* pkg_name, int hash);
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetTextureHash(FEImage* image, unsigned int hash);
void FEngSetScript(const char* pkg_name, unsigned int obj_hash, unsigned int script_hash, bool);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
void FEPrintf(const char* pkg_name, unsigned int hash, const char* format, ...);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);
int GetCurrentLanguage();
void eUnloadStreamingTexture(unsigned int* textures, int count);
void eWaitForStreamingTexturePackLoading(const char* name);

extern unsigned int iCurrentViewBin;
extern int selection;

struct RepSheetIcon : public IconOption {
    int id;

    RepSheetIcon(unsigned int tex_hash, unsigned int name_hash, int the_id)
        : IconOption(tex_hash, name_hash, 0) {
        id = the_id;
    }

    ~RepSheetIcon() override {}

    void React(const char* pkg, unsigned int data, FEObject* obj, unsigned int p1, unsigned int p2) override;
};

void RepSheetIcon::React(const char* pkg, unsigned int data, FEObject* obj, unsigned int p1, unsigned int p2) {
    selection = id;
}

uiRepSheetMain::uiRepSheetMain(ScreenConstructorData* sd)
    : IconScrollerMenu(sd)
    , RivalStreamer(sd->PackageFilename, sd->Arg != 0) {
    bIsInGame = sd->Arg != 0;
    bBossAvailable = false;
    bBossBeaten = false;
    pRivalImg = nullptr;
    pTagImg = nullptr;
    DefeatedTextureHash = 0;
    new EFadeScreenOff(0x161a918);
    Setup();
}

uiRepSheetMain::~uiRepSheetMain() {
    eWaitForStreamingTexturePackLoading(nullptr);
    unsigned int tex = DefeatedTextureHash;
    if (tex != 0) {
        eUnloadStreamingTexture(&tex, 1);
    }
}

eMenuSoundTriggers uiRepSheetMain::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    return IconScrollerMenu::NotifySoundMessage(msg, maybe);
}

void uiRepSheetMain::NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) {
    IconScrollerMenu::NotificationMessage(msg, obj, param1, param2);
    if (msg == 0x406415e3) {
        if (selection == 0) {
            if (bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("IG_BL_CHALLENGE", 1, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("BL_CHALLENGE", 0, 0, false);
            }
        } else if (selection == 1) {
            if (bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("IG_BL_MILESTONES", 1, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("BL_MILESTONES", 0, 0, false);
            }
        } else if (selection == 2) {
            if (bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("IG_BL_BOUNTY", 1, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("BL_BOUNTY", 0, 0, false);
            }
        } else if (selection == 3) {
            if (bIsInGame) {
                cFEng::Get()->QueuePackageSwitch("IG_BL_BIO", 1, 0, false);
            } else {
                cFEng::Get()->QueuePackageSwitch("BL_BIO", 0, 0, false);
            }
        }
    } else if (msg == 0x911ab364) {
        if (bIsInGame) {
            cFEng::Get()->QueuePackageSwitch("IG_PAUSEMENU", 1, 0, false);
        } else {
            cFEng::Get()->QueuePackageSwitch("FE_CAREER", 0, 0, false);
        }
    }
}

void uiRepSheetMain::Setup() {
    pRivalImg = FEngFindImage(GetPackageName(), 0xc1f62308);
    pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);
    FEImage* bgImg = FEngFindImage(GetPackageName(), 0x2cbe1dd0);
    RivalStreamer.Init(iCurrentViewBin, pRivalImg, pTagImg, bgImg);

    AddOption(new RepSheetIcon(0xefc9662e, 0x84e4a54c, 0));
    AddOption(new RepSheetIcon(0xd807e9b3, 0x216f1b81, 1));
    AddOption(new RepSheetIcon(0x8c99ea56, 0x578b767b, 2));
    AddOption(new RepSheetIcon(0x6b003b5, 0x3e6c8ae0, 3));

    UpdateInfo();
}

void uiRepSheetMain::NotifyTextureLoaded() {
}

unsigned int uiRepSheetMain::GetDefeatedTexture() {
    int lang = GetCurrentLanguage();
    switch (lang) {
    case 1: return 0x87b81cd;
    case 2: return 0x87b846e;
    case 3: return 0x87b8ece;
    case 4: return 0x87bb8d4;
    case 5: return 0x87b79bd;
    case 6: return 0x87bb9bf;
    case 7: return 0x87b7723;
    case 12: return 0x87babfb;
    case 13: return 0x87b80ad;
    default: return 0x87b7d0a;
    }
}

void uiRepSheetMain::UpdateInfo() {
    FEPrintf(GetPackageName(), 0xb514e2d8, "%d", 0);
    FEPrintf(GetPackageName(), 0xf91a59f6, "%d", 0);
}

void uiRepSheetMain::ScrollRival(eScrollDir dir) {
    if (dir == eSD_PREV) {
        if (iCurrentViewBin > 1) {
            iCurrentViewBin--;
        }
    } else {
        if (iCurrentViewBin < 15) {
            iCurrentViewBin++;
        }
    }
    RivalStreamer.Init(iCurrentViewBin, pRivalImg, pTagImg, nullptr);
    UpdateInfo();
}

void uiRepSheetMain::TextureLoadedCallback(unsigned int tex) {
}
