#include "uiRapSheetPD.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
const char* GetLocalizedString(unsigned int hash);
uiRapSheetPD::uiRapSheetPD(ScreenConstructorData* sd) : MenuScreen(sd) , pursuit_number(sd->Arg) { Setup(); }
uiRapSheetPD::~uiRapSheetPD() {}
void uiRapSheetPD::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0xE1FDE1D1) { cFEng::Get()->QueuePackageSwitch("RapSheetTEP.fng", 0, 0, false); }
}
void uiRapSheetPD::Setup() {
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    HighScoresDatabase* scores = prof->GetHighScores();
    const TopEvadedPursuitDetail& pursuit = scores->GetTopEvadedPursuitScores(static_cast<unsigned short>(pursuit_number));
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0xEB406FEC, GetLocalizedString(0x6031106E), prof->GetProfileName());
    FEPrintf(GetPackageName(), 0xD91934E7, GetLocalizedString(0xA656CD29), pursuit.PursuitName);
    Timer t(pursuit.Length);
    char time_str[16];
    t.PrintToString(time_str, 0);
    FEPrintf(GetPackageName(), 0x9068C46D, "%s", time_str);
    FEPrintf(GetPackageName(), 0x9068C46E, "%d", pursuit.NumCops);
    FEPrintf(GetPackageName(), 0x9068C46F, "%d", pursuit.NumCopsDamaged);
    FEPrintf(GetPackageName(), 0x9068C470, "%d", pursuit.NumCopsDestroyed);
    FEPrintf(GetPackageName(), 0x9068C471, "%d", pursuit.NumRoadblocksDodged);
    FEPrintf(GetPackageName(), 0x9068C472, "%d", pursuit.NumSpikeStripsDodged);
    FEPrintf(GetPackageName(), 0x9068C473, "%d", pursuit.TotalCostToState);
    FEPrintf(GetPackageName(), 0x9068C474, "%d", pursuit.NumInfractions);
    FEPrintf(GetPackageName(), 0x9068C475, "%d", pursuit.NumHelicopters);
    FEPrintf(GetPackageName(), 0x9D81523D, "%d", pursuit.Bounty);
}
