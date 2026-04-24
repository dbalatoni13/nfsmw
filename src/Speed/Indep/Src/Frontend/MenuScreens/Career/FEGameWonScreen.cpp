#include "Speed/Indep/Src/Frontend/MenuScreens/Career/FEGameWonScreen.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

int FEPrintf(const char* pkg_name, int object_hash, const char* fmt, ...);
const char* GetLocalizedString(unsigned int hash);

int FEGameWonScreen::mCurrentScreen;

FEGameWonScreen::FEGameWonScreen(ScreenConstructorData* sd)
    : MenuScreen(sd)
{
    switch (mCurrentScreen) {
    case 0:
        break;
    case 3:
        FEPrintf(GetPackageName(), static_cast<int>(0x3cc94d6), "> %s", FEDatabase->GetUserProfile(0)->GetProfileName());
        break;
    case 4: {
        FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
        UserProfile& prof = *FEDatabase->GetUserProfile(0);
        HighScoresDatabase* scores = prof.GetHighScores();

        FEPrintf(GetPackageName(), static_cast<int>(0x1232703a), GetLocalizedString(0xe21d083c), prof.GetCareer()->GetCaseFileName());
        FEPrintf(GetPackageName(), static_cast<int>(0xe3da78e7), GetLocalizedString(0x6031106e), prof.GetProfileName());
        FEPrintf(GetPackageName(), static_cast<int>(0x22f33e0a), GetLocalizedString(0x6031106e), prof.GetProfileName());
        FEPrintf(GetPackageName(), static_cast<int>(0xe3da78e8), GetLocalizedString(0x364e4525), stable->GetTotalBounty());
        FEPrintf(GetPackageName(), static_cast<int>(0xe3da78e9), GetLocalizedString(0xa355fedd), scores->GetCareerPursuitScore(static_cast<ePursuitDetailTypes>(7)));
        FEPrintf(GetPackageName(), static_cast<int>(0xe3da78ea), GetLocalizedString(0xb1e58db1), stable->GetNumImpoundedCars());
        FEPrintf(GetPackageName(), static_cast<int>(0xe3da78eb), GetLocalizedString(0x79fb7d16), stable->GetTotalFines(true));
        FEPrintf(GetPackageName(), static_cast<int>(0xe3da78ec), GetLocalizedString(0x463b461b), stable->GetTotalEvadedPursuits());
        FEPrintf(GetPackageName(), static_cast<int>(0xe3da78ed), GetLocalizedString(0xc5094459), stable->GetTotalBustedPursuits());
        FEPrintf(GetPackageName(), static_cast<int>(0xe3da78ee), GetLocalizedString(0x6dee0c7a), stable->GetNumCareerCarsWithARecord());
        break;
    }
    }
}

FEGameWonScreen::~FEGameWonScreen() {}

void FEGameWonScreen::NotificationMessage(unsigned long msg, FEObject* obj,
                                           unsigned long param1, unsigned long param2) {
    if (msg == 0xe1fde1d1) {
        QueuePackageSwitchForNextScreen();
    }
}

void FEGameWonScreen::QueuePackageSwitchForNextScreen() {
    mCurrentScreen++;
    switch (mCurrentScreen) {
        case 1:
            cFEng::Get()->QueuePackageSwitch("Credits.fng", 0, 0, false);
            break;
        case 2:
            cFEng::Get()->QueuePackageSwitch("RapSheetLogin_ENDGAME.fng", 0, 0, false);
            break;
        case 3:
            cFEng::Get()->QueuePackageSwitch("RapSheetLogin2_ENDGAME.fng", 0, 0, false);
            break;
        case 4:
            cFEng::Get()->QueuePackageSwitch("RapSheetMain_ENDGAME.fng", 0, 0, false);
            break;
        case 5:
            cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false);
            break;
    }
}

void FEGameWonScreen::Initialize() {
    mCurrentScreen = 0;
}
