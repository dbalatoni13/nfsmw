#include "uiCredits.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Career/FEGameWonScreen.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_Credits.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_UI_DebugCarCustomize.hpp"

uiCredits::uiCredits(ScreenConstructorData *sd) : MenuScreen(sd), initComplete_(false), prototypeStr_(0), pendingDelete_(0) {
    if (!FEDatabase->IsBeatGameMode()) {
        FEngSetInvisible(GetPackageName(), 0xEB4CF244);
        cFEng::Get()->QueuePackageMessage(__ENABLE_INPUTS__, GetPackageName(), 0);
    } else {
        FEngSetInvisible(GetPackageName(), 0x0BF41045);
        cFEng::Get()->QueuePackageMessage(0x3111B806, GetPackageName(), 0);
    }
}

void uiCredits::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    const u32 CREDIT_AT_TOP = __CREDIT_AT_TOP__;
    const u32 CREDIT_NEXT = __CREDIT_NEXT__;

    switch (msg) {
        case 0x35F8620B: {
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

            FEngSNPrintf(filename, 32, "CREDITS\\%s%s.TXT", prefix, languageName);

            uf_.Load(filename);
            uf_.LineWrap(45);

            prototypeStr_ = FEngFindString(GetPackageName(), FEHashUpper("CreditsArea"));
            initComplete_ = true;

            break;
        }

        case 0xE1FDE1D1:
            uf_.Unload();
            initComplete_ = false;

            if (!FEDatabase->IsBeatGameMode()) {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            } else {
                FEGameWonScreen::QueuePackageSwitchForNextScreen();
            }

            break;

        case 0xC98356BA:
            if (pendingDelete_) {
                FEPackage *currentPackage = GetPackage();
                currentPackage->RemoveObject(pendingDelete_);

                cFEngRender::mInstance->RemoveCachedRender(pendingDelete_, nullptr);
                delete pendingDelete_;
                pendingDelete_ = nullptr;
            }

            break;

        case CREDIT_NEXT:
            if (initComplete_) {
                short *creditLine = uf_.Next();

                if (!creditLine) {
                    creditLine = uf_.First();

                    if (!creditLine) {
                        break;
                    }
                }

                FEPackage *currentPackage = GetPackage();
                FEString *ns = static_cast<FEString *>(prototypeStr_->Clone(false));

                ns->Cached = nullptr;

                FEObjData *od = ns->GetObjData();
                *od = *prototypeStr_->GetObjData();

                ns->SetString(creditLine);
                ns->Flags |= FF_DirtyCode;

                if (!FEDatabase->IsBeatGameMode()) {
                    ns->SetScript(FEHashUpper("RollCredit"), false);
                } else {
                    ns->SetScript(FEHashUpper("RollCredit_ENDGAME"), false);
                }

                currentPackage->AddObject(ns);
            }

            break;

        case CREDIT_AT_TOP:
            pendingDelete_ = pobj;

            break;

        case 0x911AB364:
            cFEng::Get()->QueuePackageMessage(0x587C018B, nullptr, nullptr);

            break;

        case 0x406415E3:
            if (FEDatabase->IsBeatGameMode()) {
                cFEng::Get()->QueuePackageMessage(0x587C018B, nullptr, nullptr);
            }

            break;
    }
}
