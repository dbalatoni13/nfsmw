#include "uiCredits.hpp"

#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Career/FEGameWonScreen.hpp"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"

FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
void FEngSetInvisible(FEObject *obj);
FEString *FEngFindString(const char *pkg_name, int name_hash);
int FEngSNPrintf(char *buffer, int buf_size, const char *fmt, ...);
const char *GetLanguageName(eLanguages lang);

MenuScreen *uiCredits::Create(ScreenConstructorData *sd) {
    return new uiCredits(sd);
}

uiCredits::uiCredits(ScreenConstructorData *sd)
    : MenuScreen(sd) //
      ,
      initComplete_(false) //
      ,
      prototypeStr_(nullptr) //
      ,
      pendingDelete_(nullptr) //
      ,
      uf_() {
    if (!FEDatabase->IsBeatGameMode()) {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xeb4cf244));
        cFEng::Get()->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x0bf41045));
        cFEng::Get()->QueuePackageMessage(0x3111b806, GetPackageName(), nullptr);
    }
}

uiCredits::~uiCredits() {}

void uiCredits::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
        case 0x35f8620b: {
            char filename[32];
            const char *languageName = GetLanguageName(static_cast<eLanguages>(GetCurrentLanguage()));
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
        case 0xe1fde1d1:
            uf_.Unload();
            initComplete_ = false;
            if (!FEDatabase->IsBeatGameMode()) {
                cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            } else {
                FEGameWonScreen::QueuePackageSwitchForNextScreen();
            }
            break;
        case 0xc98356ba:
            if (pendingDelete_ != nullptr) {
                ConstructData.pPackage->Objects.RemNode(pendingDelete_);
                cFEngRender::mInstance->RemoveCachedRender(pendingDelete_, nullptr);
                delete pendingDelete_;
                pendingDelete_ = nullptr;
            }
            break;
        case 0xe6e946b8:
            if (initComplete_) {
                short *creditLine = uf_.Next();
                if (creditLine == nullptr) {
                    creditLine = uf_.First();
                }
                if (creditLine != nullptr) {
                    FEString *ns = static_cast<FEString *>(prototypeStr_->Clone(false));
                    ns->Cached = nullptr;
                    *ns->GetObjData() = *prototypeStr_->GetObjData();
                    ns->SetString(creditLine);
                    ns->Flags |= 0x400000;
                    if (!FEDatabase->IsBeatGameMode()) {
                        ns->SetScript(FEHashUpper("RollCredit"), false);
                    } else {
                        ns->SetScript(FEHashUpper("RollCredit_ENDGAME"), false);
                    }
                    ConstructData.pPackage->Objects.AddNode(ConstructData.pPackage->Objects.GetTail(), ns);
                }
            }
            break;
        case 0x29161540:
            pendingDelete_ = pobj;
            break;
        case 0x911ab364:
            cFEng::Get()->QueuePackageMessage(0x587c018b, nullptr, nullptr);
            break;
        case 0x406415e3:
            if (FEDatabase->IsBeatGameMode()) {
                cFEng::Get()->QueuePackageMessage(0x587c018b, nullptr, nullptr);
            }
            break;
    }
}
