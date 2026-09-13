#include "uiCredits.hpp"

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_Credits.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Career/FEGameWonScreen.hpp"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"

uiCredits::uiCredits(ScreenConstructorData *sd) : MenuScreen(sd), initComplete_(false), prototypeStr_(nullptr), pendingDelete_(nullptr), uf_() {
    if (!FEDatabase->IsBeatGameMode()) {
        FEngSetInvisible(GetPackageName(), 0xeb4cf244);
        cFEng::Get()->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
    } else {
        FEngSetInvisible(GetPackageName(), 0x0bf41045);
        cFEng::Get()->QueuePackageMessage(0x3111b806, GetPackageName(), nullptr);
    }
}

void uiCredits::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    const u32 CREDIT_AT_TOP = __CREDIT_AT_TOP__;
    const u32 CREDIT_NEXT = __CREDIT_NEXT__;

    switch (msg) {
        case FEHASH_INITCOMPLETE: {
            char filename[32];
            const char *languageName = GetLanguageName(GetCurrentLanguage());
            const char *prefix = "";
            if (GetCurrentLanguage() == eLANGUAGE_ENGLISH) {
                if (BuildRegion::IsAmerica()) {
                    prefix = "NA_";
                } else if (BuildRegion::IsEurope()) {
                    prefix = "UK_";
                } else {
                    languageName = "GERMAN";
                }
            }
            FEngSNPrintf(filename, 0x20, "CREDITS\\%s%s.TXT", prefix, languageName);
            uf_.Load(filename);
            uf_.LineWrap(0x2d);
            prototypeStr_ = FEngFindString(GetPackageName(), FEHashUpper("CreditsArea"));
            initComplete_ = true;
            break;
        }
        case FEHASH_EXITCOMPLETE:
            uf_.Unload();
            initComplete_ = false;
            if (!FEDatabase->IsBeatGameMode()) {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            } else {
                FEGameWonScreen::QueuePackageSwitchForNextScreen();
            }
            break;
        case FEMSG_SCREEN_TICK:
            if (pendingDelete_ != nullptr) {
                FEPackage *currentPackage = GetPackage();
                currentPackage->RemoveObject(pendingDelete_);
                cFEngRender::mInstance->RemoveCachedRender(pendingDelete_, nullptr);
                delete pendingDelete_;
                pendingDelete_ = nullptr;
            }
            break;
        case CREDIT_NEXT:
            if (initComplete_) {
                i16 *creditLine = uf_.Next();
                if (creditLine == nullptr) {
                    creditLine = uf_.First();
                }
                if (creditLine != nullptr) {
                    FEPackage *currentPackage = GetPackage();
                    FEString *ns = static_cast<FEString *>(prototypeStr_->Clone(false));
                    ns->Cached = nullptr;
                    FEObjData *od = prototypeStr_->GetObjData();
                    *ns->GetObjData() = *od;
                    ns->SetString(creditLine);
                    ns->Flags |= FF_DirtyCode;
                    if (!FEDatabase->IsBeatGameMode()) {
                        ns->SetScript(FEHashUpper("RollCredit"), false);
                    } else {
                        ns->SetScript(FEHashUpper("RollCredit_ENDGAME"), false);
                    }
                    currentPackage->AddObject(ns);
                }
            }
            break;
        case CREDIT_AT_TOP:
            pendingDelete_ = pobj;
            break;
        case __PAD_BACK__:
            cFEng::Get()->QueuePackageMessage(0x587c018b, nullptr, nullptr);
            break;
        case __PAD_ACCEPT__:
            if (FEDatabase->IsBeatGameMode()) {
                cFEng::Get()->QueuePackageMessage(0x587c018b, nullptr, nullptr);
            }
            break;
    }
}
