#include "uiRapSheetRS.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"

uiRapSheetRS::uiRapSheetRS(ScreenConstructorData *sd) : MenuScreen(sd) {
    RefreshHeader();
}

void uiRapSheetRS::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg == FEHASH_EXITCOMPLETE) {
        cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false);
    }
}

void uiRapSheetRS::RefreshHeader() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    UserProfile &prof = *FEDatabase->GetUserProfile(0);
    HighScoresDatabase *scores = prof.GetHighScores();
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof.GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0xB259EEA7, GetLocalizedString(0x6031106E), prof.GetProfileName());
    FEPrintf(GetPackageName(), 0xB259EEA8, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xB259EEA9, GetLocalizedString(0xA355FEDD), scores->GetCareerPursuitScore(PD_COST_TO_STATE));
    FEPrintf(GetPackageName(), 0xB259EEAA, GetLocalizedString(0xB1E58DB1), stable->GetNumImpoundedCars());
    FEPrintf(GetPackageName(), 0x6EB5AC50, GetLocalizedString(0x091CB790), stable->GetTotalNumInfractions(true));
    FEPrintf(GetPackageName(), 0x6EB5AC51, GetLocalizedString(0x1903C44D), stable->GetTotalNumInfractions(false));
    FEPrintf(GetPackageName(), 0xD919049F, GetLocalizedString(0x3598476F), stable->GetTotalEvadedPursuits());
    FEPrintf(GetPackageName(), 0xD91904A0, GetLocalizedString(0x2E90D7ED), stable->GetTotalBustedPursuits());
    FEPrintf(GetPackageName(), 0xD01E18C5, GetLocalizedString(0x82A67697), stable->GetTotalFines(true));
    FEPrintf(GetPackageName(), 0xD01E18C6, GetLocalizedString(0xD77B89B7), stable->GetTotalFines(false));
    FEPrintf(GetPackageName(), 0xD7E5D0CC, GetLocalizedString(0x50EC3763), scores->GetCareerPursuitScore(PD_COPS_DAMAGED));
    FEPrintf(GetPackageName(), 0xD7E5D0CD, GetLocalizedString(0xE8DB4BF3), scores->GetCareerPursuitScore(PD_COPS_DESTROYED));
    Attrib::Gen::frontend rapsheetSummaryString(Attrib::StringToKey("rap_sheet_summary"), 0, nullptr);
    if (rapsheetSummaryString.IsValid()) {
        uint32 level = rapsheetSummaryString.Num_WarningLevel();
        uint32 warning_val = stable->GetTotalNumInfractions(true) + stable->GetTotalNumInfractions(false);
        for (uint32 i = 0; i < rapsheetSummaryString.Num_WarningLevel(); i++) {
            if (warning_val <= rapsheetSummaryString.WarningLevel(i)) {
                level = i;
                break;
            }
        }
        if (level == 0) {
            level = 1;
        }
        FEngSetLanguageHash(GetPackageName(), 0x90211462, FEngHashString("RAPSHEET_WARNING_%d", level));
    }
}
