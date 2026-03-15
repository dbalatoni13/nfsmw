#include "uiRapSheetRS.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
uiRapSheetRS::uiRapSheetRS(ScreenConstructorData* sd) : MenuScreen(sd) { RefreshHeader(); }
uiRapSheetRS::~uiRapSheetRS() {}
void uiRapSheetRS::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0xE1FDE1D1) { cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false); }
}
void uiRapSheetRS::RefreshHeader() {
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    HighScoresDatabase* scores = prof->GetHighScores();
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0xB259EEA7, GetLocalizedString(0x6031106E), prof->GetProfileName());
    FEPrintf(GetPackageName(), 0xB259EEA8, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0xB259EEA9, GetLocalizedString(0xA355FEDD), scores->GetCareerPursuitScore(static_cast<ePursuitDetailTypes>(7)));
    FEPrintf(GetPackageName(), 0xB259EEAA, GetLocalizedString(0xB1E58DB1), stable->GetNumImpoundedCars());
    FEPrintf(GetPackageName(), 0x6EB5AC50, GetLocalizedString(0x091CB790), stable->GetTotalNumInfractions(true));
    FEPrintf(GetPackageName(), 0x6EB5AC51, GetLocalizedString(0x1903C44D), stable->GetTotalNumInfractions(false));
    FEPrintf(GetPackageName(), 0xD919049F, GetLocalizedString(0x3598476F), stable->GetTotalEvadedPursuits());
    FEPrintf(GetPackageName(), 0xD91904A0, GetLocalizedString(0x2E90D7ED), stable->GetTotalBustedPursuits());
    FEPrintf(GetPackageName(), 0xD01E18C5, GetLocalizedString(0x82A67697), stable->GetTotalFines(true));
    FEPrintf(GetPackageName(), 0xD01E18C6, GetLocalizedString(0xD77B89B7), stable->GetTotalFines(false));
    FEPrintf(GetPackageName(), 0xD7E5D0CC, GetLocalizedString(0x50EC3763), scores->GetCareerPursuitScore(static_cast<ePursuitDetailTypes>(2)));
    FEPrintf(GetPackageName(), 0xD7E5D0CD, GetLocalizedString(0xE8DB4BF3), scores->GetCareerPursuitScore(static_cast<ePursuitDetailTypes>(3)));
    Attrib::Gen::frontend rapsheetSummaryString(Attrib::StringToKey("rap_sheet_summary"), 0, nullptr);
    if (rapsheetSummaryString.IsValid()) {
        unsigned int wanring_val = rapsheetSummaryString.Num_WarningLevel();
        int totalInfractions = stable->GetTotalNumInfractions(true) + stable->GetTotalNumInfractions(false);
        for (unsigned int i = 0; i < rapsheetSummaryString.Num_WarningLevel(); i++) {
            if (static_cast<unsigned int>(totalInfractions) <= rapsheetSummaryString.WarningLevel(i)) { wanring_val = i; break; }
        }
        if (wanring_val == 0) { wanring_val = 1; }
        FEngSetLanguageHash(GetPackageName(), 0x90211462, FEngHashString("RAPSHEET_WARNING_%d", wanring_val));
    }
}
