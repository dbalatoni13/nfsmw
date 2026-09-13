#include "uiRapSheetCTS.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"

void RapSheetCTSArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        RapSheetCTSDatum *d = static_cast<RapSheetCTSDatum *>(datum);
        FEPrintf(pTimes, "%$d", d->getNumTimes());
        FEngSetLanguageHash(pItem, d->getItemHash());
        FEPrintf(pValue, "%$d", d->getTotalValue());
    }
}

uiRapSheetCTS::uiRapSheetCTS(ScreenConstructorData *sd) : ArrayScrollerMenu(sd, 1, 9, false) {
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEString *pTimes = FEngFindString(GetPackageName(), FEngHashString("TXT_QUANTITY_%d", i + 1));
        FEString *pItem = FEngFindString(GetPackageName(), FEngHashString("TXT_CATEGORY_%d", i + 1));
        FEString *pValue = FEngFindString(GetPackageName(), FEngHashString("TXT_CASH_%d", i + 1));
        AddSlot(new ("RapSheetCTSArraySlot", 0) RapSheetCTSArraySlot(pTimes, pItem, pValue));
    }
    Setup();
}

void uiRapSheetCTS::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    ArrayScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    if (msg == FEHASH_EXITCOMPLETE) {
        cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false);
    }
}

void uiRapSheetCTS::Setup() {
    ClearData();
    int32 quantity = 0;
    uint32 value = 0;
    uint32 total_value = 0;
    const HighScoresDatabase &scores = *FEDatabase->GetUserProfile(0)->GetHighScores();
    scores.GetCareerCST(RAP_CTS_PROPERTY_DAMAGE, quantity, value);
    AddDatum(new ("RapSheetCTSDatum", 0) RapSheetCTSDatum(quantity, 0x3682A8CF, value));
    scores.GetCareerCST(RAP_CTS_TRAFFIC_CAR_HIT, quantity, value);
    AddDatum(new ("RapSheetCTSDatum", 0) RapSheetCTSDatum(quantity, 0x6DE4810A, value));
    scores.GetCareerCST(RAP_CTS_COP_CAR_DEPLOYED, quantity, value);
    AddDatum(new ("RapSheetCTSDatum", 0) RapSheetCTSDatum(quantity, 0x89A9C941, value));
    scores.GetCareerCST(RAP_CTS_SUPPORT_VEHICLE_DEPLOYED, quantity, value);
    AddDatum(new ("RapSheetCTSDatum", 0) RapSheetCTSDatum(quantity, 0x443B615F, value));
    scores.GetCareerCST(RAP_CTS_COP_DAMAGED, quantity, value);
    AddDatum(new ("RapSheetCTSDatum", 0) RapSheetCTSDatum(quantity, 0xD3AA88DA, value));
    scores.GetCareerCST(RAP_CTS_COP_DESTROYED, quantity, value);
    AddDatum(new ("RapSheetCTSDatum", 0) RapSheetCTSDatum(quantity, 0xBDB16FEA, value));
    scores.GetCareerCST(RAP_CTS_ROADBLOCK_DEPLOYED, quantity, value);
    AddDatum(new ("RapSheetCTSDatum", 0) RapSheetCTSDatum(quantity, 0xD320C6C3, value));
    scores.GetCareerCST(RAP_CTS_SPIKE_STRIP_DEPLOYED, quantity, value);
    AddDatum(new ("RapSheetCTSDatum", 0) RapSheetCTSDatum(quantity, 0xA83862AF, value));
    scores.GetCareerCST(RAP_CTS_HELI_SPAWN, quantity, value);
    AddDatum(new ("RapSheetCTSDatum", 0) RapSheetCTSDatum(quantity, 0x80E9CCB2, value));
    RefreshHeader();
}

void uiRapSheetCTS::RefreshHeader() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    UserProfile *prof = FEDatabase->GetUserProfile(0);
    HighScoresDatabase *scores = prof->GetHighScores();
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0xE3DA78E7, GetLocalizedString(0x6031106E), prof->GetProfileName());
    FEPrintf(GetPackageName(), 0xE3DA78E8, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0x358672CC, GetLocalizedString(0xA355FEDD), scores->GetCareerPursuitScore(PD_COST_TO_STATE));
    ArrayScrollerMenu::RefreshHeader();
}
