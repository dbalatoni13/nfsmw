#include "uiRapSheetRankingsDetail.hpp"
#include "uiRapSheetRankings.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);
unsigned char FEngGetLastButton(const char* pkg_name);
void FEngSetLastButton(const char* pkg_name, unsigned char button);
void FEngSetCurrentButton(const char* pkg_name, unsigned int hash);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
void RapSheetRankingsDatum::NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) {}
bool uiRapSheetRankings::career_view = false;
uiRapSheetRankings::uiRapSheetRankings(ScreenConstructorData* sd) : MenuScreen(sd) , button_pressed(0) , init_button(0) { Setup(); }
uiRapSheetRankings::~uiRapSheetRankings() {}
void uiRapSheetRankings::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
    case 0x0C407210:
        button_pressed = pobj->NameHash;
        break;
    case 0xC519BFC4:
        career_view = !career_view;
        Setup();
        break;
    case 0x35F8620B:
        FEngSetCurrentButton(GetPackageName(), init_button);
        break;
    case 0xE1FDE1D1: {
        int index = 10;
        switch (button_pressed) {
        case 0xCDA0A66B: index = 0; break;
        case 0xCDA0A66C: index = 1; break;
        case 0xCDA0A66D: index = 2; break;
        case 0xCDA0A66E: index = 3; break;
        case 0xCDA0A66F: index = 5; break;
        case 0xCDA0A670: index = 4; break;
        case 0xCDA0A671: index = 7; break;
        case 0xCDA0A672: index = 8; break;
        case 0xCDA0A673: index = 6; break;
        case 0x81B573FB: index = 9; break;
        }
        if (index != 10) { uiRapSheetRankingsDetail::career_view = career_view; cFEng::Get()->QueuePackageSwitch("RapSheetRankingsDetail.fng", index, 0, false); FEngSetLastButton(GetPackageName(), static_cast<unsigned char>(index)); }
        else { cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false); FEngSetLastButton(GetPackageName(), 0); }
        break;
    }
    }
}
void uiRapSheetRankings::RefreshHeader() {
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), static_cast<int>(0xEB406FEC), GetLocalizedString(0x6031106E), prof->GetProfileName());
    FEngSetLanguageHash(GetPackageName(), 0x1E4FDA, career_view ? 0x96DDF504 : 0x56E940F4);
    FEngSetLanguageHash(GetPackageName(), 0xDD2F4FB, career_view ? 0x554BBDB5 : 0xA88B3FC5);
    FEngSetLanguageHash(GetPackageName(), 0x9AE9B5CD, career_view ? 0x554BBDB5 : 0xA88B3FC5);
}
void uiRapSheetRankings::Setup() {
    PrintRanking(0x7711109B, 0xCDA0A66B, ePDT_CostToState);
    PrintRanking(0x7711109C, 0xCDA0A66C, ePDT_Bounty);
    PrintRanking(0x7711109D, 0xCDA0A66D, ePDT_Infractions);
    PrintRanking(0x7711109E, 0xCDA0A66E, ePDT_SpeedingTotalFine);
    PrintRanking(0x7711109F, 0xCDA0A66F, static_cast<ePursuitDetailTypes>(5));
    PrintRanking(0x771110A0, 0xCDA0A670, static_cast<ePursuitDetailTypes>(4));
    PrintRanking(0x771110A1, 0xCDA0A671, static_cast<ePursuitDetailTypes>(7));
    PrintRanking(0x771110A2, 0xCDA0A672, static_cast<ePursuitDetailTypes>(8));
    PrintRanking(0x771110A3, 0xCDA0A673, static_cast<ePursuitDetailTypes>(6));
    PrintRanking(0x5933242B, 0x81B573FB, static_cast<ePursuitDetailTypes>(9));
    RefreshHeader();
}
void uiRapSheetRankings::PrintRanking(unsigned int fe_rank, unsigned int button_hash, ePursuitDetailTypes type) {
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    int rank = prof->GetHighScores()->CalcPursuitRank(type, career_view);
    if (rank != 0x10) { FEPrintf(GetPackageName(), fe_rank, "%d", rank); }
    else { FEPrintf(GetPackageName(), fe_rank, "%s", GetLocalizedString(0xF3799455)); }
    unsigned char lastButton = FEngGetLastButton(GetPackageName());
    if (static_cast<unsigned char>(type) == lastButton) { init_button = button_hash; }
}
