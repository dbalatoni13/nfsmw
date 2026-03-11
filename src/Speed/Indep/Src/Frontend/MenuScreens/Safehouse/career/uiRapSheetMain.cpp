#include "uiRapSheetMain.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);
unsigned char FEngGetLastButton(const char* pkg_name);
void FEngSetLastButton(const char* pkg_name, unsigned char button);
void FEngSetCurrentButton(const char* pkg_name, unsigned int hash);
uiRapSheetMain::uiRapSheetMain(ScreenConstructorData* sd)
    : UIWidgetMenu(sd) //
    , button_pressed(0)
{ RefreshHeader(); }
uiRapSheetMain::~uiRapSheetMain() {}
void uiRapSheetMain::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
    case 0x35F8620B: {
        unsigned char button = FEngGetLastButton(GetPackageName());
        if (button == 0) { button = 1; }
        FEngSetCurrentButton(GetPackageName(), FEngHashString("BL_%d", button));
        break;
    }
    case 0x0C407210:
        button_pressed = pobj->NameHash;
        break;
    case 0xE1FDE1D1: {
        int button_num = 1;
        if (button_pressed == 0xCDA0A66D) { button_num = 3; cFEng::Get()->QueuePackageSwitch("RapSheetCTS.fng", 0, 0, false); }
        else if (button_pressed == 0xCDA0A66B) { cFEng::Get()->QueuePackageSwitch("RapSheetRS.fng", 0, 0, false); }
        else if (button_pressed == 0xCDA0A66C) { button_num = 2; cFEng::Get()->QueuePackageSwitch("RapSheetUS.fng", 0, 0, false); }
        else if (button_pressed == 0xCDA0A66F) { button_num = 5; cFEng::Get()->QueuePackageSwitch("RapSheetRankings.fng", 0, 0, false); }
        else if (button_pressed == 0xCDA0A66E) { button_num = 4; cFEng::Get()->QueuePackageSwitch("RapSheetTEP.fng", 0, 0, false); }
        else if (button_pressed == 0xCDA0A670) { button_num = 6; cFEng::Get()->QueuePackageSwitch("RapSheetVD.fng", 0, 0, false); }
        else { button_num = 1; cFEng::Get()->QueuePackageSwitch("MainMenu_Sub.fng", 0, 0, false); FEDatabase->ClearGameMode(eFE_GAME_MODE_RAP_SHEET); }
        FEngSetLastButton(GetPackageName(), static_cast<unsigned char>(button_num));
        break;
    }
    }
}
void uiRapSheetMain::RefreshHeader() {
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    HighScoresDatabase* scores = prof->GetHighScores();
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0xE3DA78E7, GetLocalizedString(0x6031106E), prof->GetProfileName());
    FEPrintf(GetPackageName(), 0xE3DA78E8, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xE3DA78E9, GetLocalizedString(0xA355FEDD), scores->GetCareerPursuitScore(static_cast<ePursuitDetailTypes>(7)));
    FEPrintf(GetPackageName(), 0xE3DA78EA, GetLocalizedString(0xB1E58DB1), stable->GetNumImpoundedCars());
    FEPrintf(GetPackageName(), 0xE3DA78EB, GetLocalizedString(0x79FB7D16), stable->GetTotalFines(true));
    FEPrintf(GetPackageName(), 0xE3DA78EC, GetLocalizedString(0x463B461B), stable->GetTotalEvadedPursuits());
    FEPrintf(GetPackageName(), 0xE3DA78ED, GetLocalizedString(0xC5094459), stable->GetTotalBustedPursuits());
    FEPrintf(GetPackageName(), 0xE3DA78EE, GetLocalizedString(0x6DEE0C7A), stable->GetNumCareerCarsWithARecord());
}
