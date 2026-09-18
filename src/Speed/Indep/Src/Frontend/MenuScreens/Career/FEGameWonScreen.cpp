#include "FEGameWonScreen.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

int FEGameWonScreen::mCurrentScreen = 0;

FEGameWonScreen::FEGameWonScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    switch (mCurrentScreen) {
    case eYOU_WIN:
    case eCREDITS:
    case eRAP_SHEET_LOGIN:
        break;
    case eRAP_SHEET_LOGIN2:
        FEPrintf(GetPackageName(), 0x3CC94D6, "> %s", FEDatabase->GetUserProfile(0)->GetProfileName());
        break;
    case eRAP_SHEET_MAIN: {
        UserProfile *prof = FEDatabase->GetUserProfile(0);
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        HighScoresDatabase *scores = prof->GetHighScores();
        FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
        FEPrintf(GetPackageName(), 0xE3DA78E7, GetLocalizedString(0x6031106E), prof->GetProfileName());
        FEPrintf(GetPackageName(), 0x22F33E0A, GetLocalizedString(0x6031106E), prof->GetProfileName());
        FEPrintf(GetPackageName(), 0xE3DA78E8, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
        FEPrintf(GetPackageName(), 0xE3DA78E9, GetLocalizedString(0xA355FEDD),
                 scores->CareerPursuitDetails.GetValue(PD_COST_TO_STATE));
        FEPrintf(GetPackageName(), 0xE3DA78EA, GetLocalizedString(0xB1E58DB1), stable->GetNumImpoundedCars());
        FEPrintf(GetPackageName(), 0xE3DA78EB, GetLocalizedString(0x79FB7D16), stable->GetTotalFines(true));
        FEPrintf(GetPackageName(), 0xE3DA78EC, GetLocalizedString(0x463B461B), stable->GetTotalEvadedPursuits());
        FEPrintf(GetPackageName(), 0xE3DA78ED, GetLocalizedString(0xC5094459), stable->GetTotalBustedPursuits());
        FEPrintf(GetPackageName(), 0xE3DA78EE, GetLocalizedString(0x6DEE0C7A), stable->GetNumCareerCarsWithARecord());
        break;
    }
    case eDONE_WIN_FLOW_NOW_GO_BACK_TO_THE_SAFE_HOUSE:
        break;
    }
}

FEGameWonScreen::~FEGameWonScreen() {}

void FEGameWonScreen::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    if (msg == 0xE1FDE1D1) {
        QueuePackageSwitchForNextScreen();
    }
}

void FEGameWonScreen::QueuePackageSwitchForNextScreen() {
    switch (++mCurrentScreen) {
    case eCREDITS:
        cFEng::Get()->QueuePackageSwitch("Credits.fng", 0, 0, 0);
        break;
    case eRAP_SHEET_LOGIN:
        cFEng::Get()->QueuePackageSwitch("RapSheetLogin_ENDGAME.fng", 0, 0, 0);
        break;
    case eRAP_SHEET_LOGIN2:
        cFEng::Get()->QueuePackageSwitch("RapSheetLogin2_ENDGAME.fng", 0, 0, 0);
        break;
    case eRAP_SHEET_MAIN:
        cFEng::Get()->QueuePackageSwitch("RapSheetMain_ENDGAME.fng", 0, 0, 0);
        break;
    case eDONE_WIN_FLOW_NOW_GO_BACK_TO_THE_SAFE_HOUSE:
        cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, 0);
        break;
    }
}

void FEGameWonScreen::Initialize() {
    mCurrentScreen = 0;
}
