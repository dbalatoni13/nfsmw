#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_Chyron.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"

Chyron::Chyron(ScreenConstructorData *sd) : MenuScreen(sd), mDelayTimer() {}

// UNSOLVED
void InitChyron() {
    ChyronScreenPtr = static_cast<MenuScreen *>(bMalloc(sizeof(Chyron), "Chyron", 0, 0));
}

void CloseChyron() {}

MenuScreen *CreateChyronScreen(ScreenConstructorData *sd) {
    return new Chyron(sd);
}

void Chyron::NotificationMessage(u32 msg, FEObject *pobject, u32 param1, u32 param2) {
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

    uint16 widestring[256];

    FEngFont *font = FindFont(title->Handle);
    bStrCpy(widestring, mTitle);
    float maxString = font->GetTextWidth(reinterpret_cast<const int16 *>(widestring), title->Flags);

    font = FindFont(artist->Handle);
    bStrCpy(widestring, mArtist);
    maxString = bMax(maxString, font->GetTextWidth(reinterpret_cast<const int16 *>(widestring), artist->Flags));

    bStrCpy(widestring, mAlbum);
    font = FindFont(album->Handle);
    maxString = bMax(maxString, font->GetTextWidth(reinterpret_cast<const int16 *>(widestring), album->Flags));

    uint32 startScript;
    if (maxString <= 185.0f) {
        startScript = 0x57521F29;
    } else if (maxString <= 250.0f) {
        startScript = 0x57520493;
    } else {
        startScript = 0x73D17048;
    }

    FEPrintf(title, mTitle);
    FEPrintf(artist, mArtist);
    FEPrintf(album, mAlbum);

    cFEng::Get()->QueuePackageMessage(startScript, GetPackageName(), nullptr);

    if (TheGameFlowManager.IsInFrontend()) {
        cFEng::Get()->QueuePackageMessage(FEHashUpper("TRAX_POS_2"), GetPackageName(), nullptr);
    } else {
        if ((MemoryCard::GetInstance()->IsAutoSaving() || MemoryCard::GetInstance()->AutoSaveRequested()) && GRaceStatus::Exists() &&
            GRaceStatus::Get().GetRaceParameters() != nullptr && GRaceStatus::Get().GetRaceParameters()->GetIsDDayRace()) {
            cFEng::Get()->QueuePackageMessage(FEHashUpper("TRAX_POS_3"), GetPackageName(), nullptr);
        } else {
            cFEng::Get()->QueuePackageMessage(FEHashUpper("TRAX_POS_1"), GetPackageName(), nullptr);
        }
    }

    bool widescreen = FEDatabase->GetOptionsSettings()->TheVideoSettings.WideScreen;
    if (TheGameFlowManager.IsInFrontend()) {
        cFEng::Get()->QueuePackageMessage(widescreen ? FEHashUpper("WIDESCREEN_MODE") : FEHashUpper("NORMAL_MODE"), nullptr, nullptr);
    } else {
        cFEng::Get()->QueuePackageMessage(widescreen ? FEHashUpper("WIDESCREEN_MODEHUD") : FEHashUpper("NORMAL_MODEHUD"), nullptr, nullptr);
    }
}

void DismissChyron() {
    if (cFEng::Get()->IsPackagePushed("EA_Trax.fng")) {
        cFEng::Get()->PopNoControlPackage("EA_Trax.fng");
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
        if (!cFEng::Get()->IsPackagePushed("EA_Trax.fng")) {
            cFEng::Get()->PushNoControlPackage("EA_Trax.fng", FE_PACKAGE_PRIORITY_CLOSEST);
        }
        Chyron *chyron = static_cast<Chyron *>(FEngFindScreen("EA_Trax.fng"));
        if (chyron != nullptr) {
            chyron->mDelayTimer = RealTimer;
        }
    }
}
