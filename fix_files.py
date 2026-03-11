#!/usr/bin/env python3
"""
Script to fix file permissions and write content to the two files.
Run this if file permission fixes are needed.
"""
import os
import subprocess

# File paths
file1 = '/Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiCredits.cpp'
file2 = '/Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsTrailers.cpp'

# Step 1: Check current permissions
print("=== STEP 1: Checking current permissions ===")
for f in [file1, file2]:
    try:
        stat_info = os.stat(f)
        print(f"File: {f}")
        print(f"  Permissions: {oct(stat_info.st_mode)}")
        print(f"  Size: {stat_info.st_size} bytes")
    except Exception as e:
        print(f"Error checking {f}: {e}")

# Step 2: Try to remove immutable flags
print("\n=== STEP 2: Removing immutable flags ===")
for f in [file1, file2]:
    try:
        subprocess.run(['chflags', 'nouchg', f], check=False, capture_output=True)
        print(f"Attempted to remove flags from {f}")
    except Exception as e:
        print(f"Error removing flags from {f}: {e}")

# Step 3: Try chmod
print("\n=== STEP 3: Running chmod 644 ===")
for f in [file1, file2]:
    try:
        os.chmod(f, 0o644)
        print(f"chmod succeeded on {f}")
    except Exception as e:
        print(f"chmod failed on {f}: {e}")

# Step 4: Try writing content
print("\n=== STEP 4: Writing content to files ===")

content1 = '''#include "uiCredits.hpp"

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
'''

content2 = '''#include "uiOptionsTrailers.hpp"

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

try:
    with open(file1, 'w') as f:
        f.write(content1)
    print(f"Successfully wrote to {file1}")
except Exception as e:
    print(f"ERROR writing to {file1}: {e}")
    # Try removing and recreating
    try:
        os.remove(file1)
        with open(file1, 'w') as f:
            f.write(content1)
        print(f"Successfully wrote to {file1} after removing old file")
    except Exception as e2:
        print(f"FAILED to write to {file1} after remove: {e2}")

try:
    with open(file2, 'w') as f:
        f.write(content2)
    print(f"Successfully wrote to {file2}")
except Exception as e:
    print(f"ERROR writing to {file2}: {e}")
    # Try removing and recreating
    try:
        os.remove(file2)
        with open(file2, 'w') as f:
            f.write(content2)
        print(f"Successfully wrote to {file2} after removing old file")
    except Exception as e2:
        print(f"FAILED to write to {file2} after remove: {e2}")

# Step 5: Verify files
print("\n=== STEP 5: Verifying file contents ===")
for f in [file1, file2]:
    try:
        with open(f, 'r') as fh:
            lines = fh.readlines()
            print(f"\n{f}:")
            print(f"  Total lines: {len(lines)}")
            print(f"  First 5 lines:")
            for i, line in enumerate(lines[:5]):
                print(f"    {i+1}: {line.rstrip()}")
            print(f"  Last 5 lines:")
            for i, line in enumerate(lines[-5:]):
                print(f"    {len(lines)-4+i}: {line.rstrip()}")
    except Exception as e:
        print(f"Error reading {f}: {e}")

print("\n=== DONE ===")
