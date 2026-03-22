#include "uiRapSheetCTS.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
int FEPrintf(FEString* text, const char* fmt, ...);
void FEngSetLanguageHash(FEString* text, unsigned int hash);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);
void RapSheetCTSDatum::NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) {}
void RapSheetCTSArraySlot::Update(ArrayDatum* datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        RapSheetCTSDatum* d = static_cast<RapSheetCTSDatum*>(datum);
        FEPrintf(pTimes, "%d", d->getNumTimes());
        FEngSetLanguageHash(pItem, d->getItemHash());
        FEPrintf(pValue, "%d", d->getTotalValue());
    }
}
uiRapSheetCTS::uiRapSheetCTS(ScreenConstructorData* sd) : ArrayScrollerMenu(sd, 1, 9, false) {
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEString* pTimes = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_TIMES_%d", i + 1));
        FEString* pItem = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_ITEM_%d", i + 1));
        FEString* pValue = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE_%d", i + 1));
        AddSlot(new(__FILE__, __LINE__) RapSheetCTSArraySlot(pTimes, pItem, pValue));
    }
    Setup();
}
uiRapSheetCTS::~uiRapSheetCTS() {}
void uiRapSheetCTS::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0xE1FDE1D1) { cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false); }
}
void uiRapSheetCTS::Setup() {
    int quantity;
    unsigned int value;
    unsigned int total_value;
    ClearData();
    quantity = 0;
    value = 0;
    const HighScoresDatabase& scores = *FEDatabase->GetUserProfile(0)->GetHighScores();
    scores.GetCareerCST(RAP_CTS_PROPERTY_DAMAGE, quantity, value); AddDatum(new(__FILE__, __LINE__) RapSheetCTSDatum(quantity, 0x3682A8CF, value));
    scores.GetCareerCST(RAP_CTS_TRAFFIC_CAR_HIT, quantity, value); AddDatum(new(__FILE__, __LINE__) RapSheetCTSDatum(quantity, 0x6DE4810A, value));
    scores.GetCareerCST(RAP_CTS_COP_CAR_DEPLOYED, quantity, value); AddDatum(new(__FILE__, __LINE__) RapSheetCTSDatum(quantity, 0x89A9C941, value));
    scores.GetCareerCST(RAP_CTS_SUPPORT_VEHICLE_DEPLOYED, quantity, value); AddDatum(new(__FILE__, __LINE__) RapSheetCTSDatum(quantity, 0x443B615F, value));
    scores.GetCareerCST(RAP_CTS_COP_DAMAGED, quantity, value); AddDatum(new(__FILE__, __LINE__) RapSheetCTSDatum(quantity, 0xD3AA88DA, value));
    scores.GetCareerCST(RAP_CTS_COP_DESTROYED, quantity, value); AddDatum(new(__FILE__, __LINE__) RapSheetCTSDatum(quantity, 0xBDB16FEA, value));
    scores.GetCareerCST(RAP_CTS_ROADBLOCK_DEPLOYED, quantity, value); AddDatum(new(__FILE__, __LINE__) RapSheetCTSDatum(quantity, 0xD320C6C3, value));
    scores.GetCareerCST(RAP_CTS_SPIKE_STRIP_DEPLOYED, quantity, value); AddDatum(new(__FILE__, __LINE__) RapSheetCTSDatum(quantity, 0xA83862AF, value));
    scores.GetCareerCST(RAP_CTS_HELI_SPAWN, quantity, value); AddDatum(new(__FILE__, __LINE__) RapSheetCTSDatum(quantity, 0x80E9CCB2, value));
    RefreshHeader();
}
void uiRapSheetCTS::RefreshHeader() {
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    HighScoresDatabase* scores = prof->GetHighScores();
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0xE3DA78E7, GetLocalizedString(0x6031106E), prof->GetProfileName());
    FEPrintf(GetPackageName(), 0xE3DA78E8, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0x358672CC, GetLocalizedString(0xA355FEDD), scores->GetCareerPursuitScore(static_cast<ePursuitDetailTypes>(7)));
    ArrayScrollerMenu::RefreshHeader();
}
