import os, stat

files = {
    '/Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiCredits.cpp': '''#include "uiCredits.hpp"

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Career/FEGameWonScreen.hpp"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"

FEString* FEngFindString(const char* pkg_name, int name_hash);
int FEngSNPrintf(char* buffer, int buf_size, const char* fmt, ...);
int GetCurrentLanguage();
const char* GetLanguageName(eLanguages lang);

MenuScreen* uiCredits::Create(ScreenConstructorData* sd) {
    return new uiCredits(sd);
}

uiCredits::uiCredits(ScreenConstructorData* sd)
    : MenuScreen(sd) //
    , initComplete_(false) //
    , prototypeStr_(nullptr) //
    , pendingDelete_(nullptr) //
    , uf_() {
    if (FEDatabase->IsBeatGameMode()) {
        FEngSetInvisible(GetPackageName(), 0x0bf41045);
        cFEng::Get()->QueuePackageMessage(0x3111b806, GetPackageName(), nullptr);
    } else {
        FEngSetInvisible(GetPackageName(), 0xeb4cf244);
        cFEng::Get()->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
    }
}

uiCredits::~uiCredits() {}

void uiCredits::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1,
                                    unsigned long param2) {
    const unsigned long CREDIT_AT_TOP = 0xc98356ba;
    const unsigned long CREDIT_NEXT = 0xe6e946b8;

    if (msg == 0x911ab364) {
        cFEng::Get()->QueuePackageMessage(0x587c018b, nullptr, nullptr);
    } else if (msg == 0x35f8620b) {
        char filename[32];
        const char* languageName =
            GetLanguageName(static_cast<eLanguages>(GetCurrentLanguage()));
        const char* prefix = "";
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
    } else if (msg == 0x29161540) {
        pendingDelete_ = pobj;
    } else if (msg == 0x406415e3) {
        if (FEDatabase->IsBeatGameMode()) {
            cFEng::Get()->QueuePackageMessage(0x587c018b, nullptr, nullptr);
        }
    } else if (msg == CREDIT_AT_TOP) {
        if (pendingDelete_ != nullptr) {
            FEPackage* currentPackage = GetPackage();
            currentPackage->RemoveObject(pendingDelete_);
            cFEngRender::mInstance->RemoveCachedRender(pendingDelete_, nullptr);
            delete pendingDelete_;
            pendingDelete_ = nullptr;
        }
    } else if (msg == 0xe1fde1d1) {
        uf_.Unload();
        initComplete_ = false;
        if (FEDatabase->IsBeatGameMode()) {
            FEGameWonScreen::QueuePackageSwitchForNextScreen();
        } else {
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
        }
    } else if (msg == CREDIT_NEXT && initComplete_) {
        short* creditLine = uf_.Next();
        if (creditLine == nullptr) {
            creditLine = uf_.First();
        }
        if (creditLine != nullptr) {
            FEPackage* currentPackage = GetPackage();
            FEString* ns = static_cast<FEString*>(prototypeStr_->Clone(false));
            ns->Cached = nullptr;
            *ns->GetObjData() = *prototypeStr_->GetObjData();
            ns->SetString(creditLine);
            ns->Flags |= 0x400000;
            if (FEDatabase->IsBeatGameMode()) {
                ns->SetScript(FEHashUpper("RollCredit_ENDGAME"), false);
            } else {
                ns->SetScript(FEHashUpper("RollCredit"), false);
            }
            currentPackage->AddObject(ns);
        }
    }
}
''',

    '/Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsTrailers.cpp': '''#include "uiOptionsTrailers.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int language);
unsigned char FEngGetLastButton(const char* pkg_name);

struct GarageMainScreen : public MenuScreen {
    char _pad_2c[0x2C];
    bool CameraPushRequested; // offset 0x58

    GarageMainScreen(ScreenConstructorData* sd) : MenuScreen(sd) {}
    ~GarageMainScreen() override;
    void NotificationMessage(unsigned long, FEObject*, unsigned long, unsigned long) override;
    void CancelCameraPush() { CameraPushRequested = false; }
    static GarageMainScreen* GetInstance();
};

UIOptionsTrailers::UIOptionsTrailers(ScreenConstructorData* sd)
    : IconScrollerMenu(sd) {
    Setup();
}

void UIOptionsTrailers::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1,
                                            unsigned long param2) {
    if (msg != 0x0c407210) {
        IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    }

    if (msg == 0x911ab364) {
        StorePrevNotification(0x911ab364, pobj, param1, param2);
        cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
    } else if (msg == 0x0c407210) {
        cFEng::Get()->QueuePackageMessage(0x8cb81f09, nullptr, nullptr);
        Options.GetCurrentOption()->React(GetPackageName(), 0x0c407210, pobj, param1, param2);
    } else if (msg == 0xd05fc3a3) {
        Options.GetCurrentOption()->React(GetPackageName(), 0xd05fc3a3, pobj, param1, param2);
    } else if (msg == 0xe1fde1d1 && PrevButtonMessage == 0x911ab364) {
        FEDatabase->ClearGameMode(eFE_GAME_TRAILERS);
        FEDatabase->GetOptionsSettings()->CurrentCategory = static_cast<eOptionsCategory>(-1);
        cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
    }
}

void UIOptionsTrailers::Setup() {
    const unsigned long FEObj_TITLEGROUP = 0xb71b576d;

    unsigned char lastButton = FEngGetLastButton(GetPackageName());

    if (bFadeInIconsImmediately) {
        Options.bFadingIn = true;
        Options.bFadingOut = false;
        Options.bDelayUpdate = false;
        Options.fCurFadeTime = 0.0f;
    }

    SetInitialOption(lastButton);
    GarageMainScreen::GetInstance()->CancelCameraPush();
    FEngSetLanguageHash(GetPackageName(), FEObj_TITLEGROUP, 0xb65a46d8);
    RefreshHeader();
}
'''
}

for path, content in files.items():
    try:
        current = os.stat(path).st_mode
        os.chmod(path, current | stat.S_IWUSR)
    except Exception as e:
        print(f"chmod error for {path}: {e}")
    with open(path, 'w') as f:
        f.write(content)
    print(f"Wrote {path} ({os.path.getsize(path)} bytes)")

for path in files:
    with open(path) as f:
        lines = f.readlines()
    print(f"\n--- {path.split('/')[-1]} ---")
    print(f"Total lines: {len(lines)}")
    print("First 3 lines:")
    for line in lines[:3]:
        print(f"  {line.rstrip()}")
    print("Last 3 lines:")
    for line in lines[-3:]:
        print(f"  {line.rstrip()}")
