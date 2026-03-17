#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_Chyron.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"

extern ICEManager TheICEManager;
extern Timer RealTimer;
extern MenuScreen *FEngFindScreen(const char *name);
extern FEString *FEngFindString(const char *pkg_name, int hash);
extern int FEPrintf(FEString *text, const char *fmt, ...);
extern void bStrCpy(unsigned short *to, const char *from);
extern unsigned long FEHashUpper(const char *string);

extern const char lbl_803E59BC[];

static MenuScreen *ChyronScreenPtr;

Chyron::Chyron(ScreenConstructorData *sd)
    : MenuScreen(sd) //
    , mDelayTimer(0)
{
}

Chyron::~Chyron() {}

void Chyron::NotificationMessage(unsigned long msg, FEObject *pobject, unsigned long param1, unsigned long param2) {
    if (mDelayTimer.IsSet()) {
        if (!TheGameFlowManager.IsInFrontend()) {
            if ((RealTimer - mDelayTimer).GetSeconds() < 5.0f) {
                return;
            }
        }
        Start();
    }
}

void Chyron::Start() {
    if (mDelayTimer.IsSet()) {
        mDelayTimer.UnSet();
    }

    FEString *title = FEngFindString(GetPackageName(), 0xF5387816);
    FEString *artist = FEngFindString(GetPackageName(), 0x11DB9E17);
    FEString *album = FEngFindString(GetPackageName(), 0x48097852);

    unsigned short widestring[256];

    FEngFont *font = FindFont(title->Handle);
    bStrCpy(widestring, mTitle);
    float maxString = font->GetTextWidth(reinterpret_cast<const short *>(widestring), title->Flags);

    font = FindFont(artist->Handle);
    bStrCpy(widestring, mArtist);
    float artistWidth = font->GetTextWidth(reinterpret_cast<const short *>(widestring), artist->Flags);
    maxString = bMax(maxString, artistWidth);

    bStrCpy(widestring, mAlbum);
    font = FindFont(album->Handle);
    float albumWidth = font->GetTextWidth(reinterpret_cast<const short *>(widestring), album->Flags);
    maxString = bMax(maxString, albumWidth);

    unsigned int startScript;
    if (maxString > 185.0f) {
        startScript = 0x73D17048;
        if (maxString <= 250.0f) {
            startScript = 0x57520493;
        }
    } else {
        startScript = 0x57521F29;
    }

    FEPrintf(title, mTitle);
    FEPrintf(artist, mArtist);
    FEPrintf(album, mAlbum);

    cFEng::Get()->QueuePackageMessage(startScript, GetPackageName(), nullptr);

    if (TheGameFlowManager.IsInFrontend()) {
        const char *posMsg = "TRAX_POS_2";
        cFEng::Get()->QueuePackageMessage(FEHashUpper(posMsg), GetPackageName(), nullptr);
    } else {
        if ((MemoryCard::GetInstance()->IsAutoSaving()
             || MemoryCard::GetInstance()->AutoSaveRequested())
            && GRaceStatus::Exists()
            && GRaceStatus::Get().GetRaceParameters() != nullptr
            && GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
            const char *posMsg = "TRAX_POS_3";
            cFEng::Get()->QueuePackageMessage(FEHashUpper(posMsg), GetPackageName(), nullptr);
        } else {
            cFEng::Get()->QueuePackageMessage(FEHashUpper("TRAX_POS_1"), GetPackageName(), nullptr);
        }
    }

    bool widescreen = FEDatabase->GetOptionsSettings()->TheVideoSettings.WideScreen;
    if (TheGameFlowManager.IsInFrontend()) {
        if (widescreen) {
            cFEng::Get()->QueuePackageMessage(FEHashUpper("WIDESCREEN_MODE"), nullptr, nullptr);
        } else {
            cFEng::Get()->QueuePackageMessage(FEHashUpper("NORMAL_MODE"), nullptr, nullptr);
        }
    } else {
        if (widescreen) {
            cFEng::Get()->QueuePackageMessage(FEHashUpper("WIDESCREEN_MODEHUD"), nullptr, nullptr);
        } else {
            cFEng::Get()->QueuePackageMessage(FEHashUpper("NORMAL_MODEHUD"), nullptr, nullptr);
        }
    }
}

void InitChyron() {
    ChyronScreenPtr = static_cast<MenuScreen *>(bMalloc(0x30, 0));
}

MenuScreen *CreateChyronScreen(ScreenConstructorData *sd) {
    return new (ChyronScreenPtr) Chyron(sd);
}

void DismissChyron() {
    if (cFEng::Get()->IsPackagePushed(lbl_803E59BC)) {
        cFEng::Get()->PopNoControlPackage(lbl_803E59BC);
    }
}

void SummonChyron(char *title, char *artist, char *album) {
    if (!TheICEManager.IsEditorOn()) {
        if (title != nullptr && artist != nullptr && album != nullptr) {
            Chyron::mTitle = title;
            Chyron::mArtist = artist;
            Chyron::mAlbum = album;
        }
        DismissChyron();
        if (!cFEng::Get()->IsPackagePushed(lbl_803E59BC)) {
            cFEng::Get()->PushNoControlPackage(lbl_803E59BC, FE_PACKAGE_PRIORITY_CLOSEST);
        }
        Chyron *chyron = static_cast< Chyron * >(FEngFindScreen(lbl_803E59BC));
        if (chyron != nullptr) {
            chyron->mDelayTimer = RealTimer;
        }
    }
}