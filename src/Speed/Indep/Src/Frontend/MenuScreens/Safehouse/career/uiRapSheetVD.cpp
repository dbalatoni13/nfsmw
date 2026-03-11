#include "uiRapSheetVD.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
int FEPrintf(FEString* text, const char* fmt, ...);
void FEngSetLanguageHash(FEString* text, unsigned int hash);
unsigned int FEngHashString(const char* format, ...);
const char* GetLocalizedString(unsigned int hash);
void RapSheetVDDatum::NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) {}
void RapSheetVDArraySlot::Update(ArrayDatum* datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        RapSheetVDDatum* d = static_cast<RapSheetVDDatum*>(datum);
        FEngSetLanguageHash(pCar, d->getCarHash());
        FEngSetLanguageHash(pToDrive, d->getStatusHash());
        FEPrintf(pBounty, "%d", d->getBounty());
        FEPrintf(pFines, "%d", d->getFines());
        FEPrintf(pUnserved, "%d", d->getUnserved());
        FEPrintf(pEvaded, "%d", d->getEvaded());
        FEPrintf(pBusted, "%d", d->getBusted());
    }
}
uiRapSheetVD::uiRapSheetVD(ScreenConstructorData* sd) : ArrayScrollerMenu(sd, 1, 5, false) {
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEString* pCar = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_CAR_%d", i + 1));
        FEString* pBounty = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE_%d", i + 1));
        FEString* pFines = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE2_%d", i + 1));
        FEString* pUnserved = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE3_%d", i + 1));
        FEString* pToDrive = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE4_%d", i + 1));
        FEString* pEvaded = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE5_%d", i + 1));
        FEString* pBusted = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE6_%d", i + 1));
        AddSlot(new(__FILE__, __LINE__) RapSheetVDArraySlot(pCar, pBounty, pFines, pUnserved, pToDrive, pEvaded, pBusted));
    }
    Setup();
}
uiRapSheetVD::~uiRapSheetVD() {}
void uiRapSheetVD::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0xE1FDE1D1) { cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false); }
}
void uiRapSheetVD::Setup() {
    int numCars = 0;
    ClearData();
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    for (int i = 0; i < 200; i++) {
        FECarRecord* fe_car = stable->GetCarByIndex(i);
        if (fe_car->IsValid() && fe_car->MatchesFilter(0xF0002)) {
            FECareerRecord* record = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
            if (record != nullptr) {
                unsigned int name_hash = fe_car->GetNameHash();
                unsigned int bounty = record->GetBounty();
                unsigned int fines = record->GetInfractions(true).GetFineValue();
                unsigned short unserved = stable->GetNumInfractionsOnCar(fe_car->Handle, true);
                unsigned int evaded = record->GetNumEvadedPursuits();
                unsigned int busted = record->GetNumBustedPursuits();
                unsigned int status_hash;
                if (record->TheImpoundData.IsImpounded()) { status_hash = 0x35E4E01F; }
                else if (busted != 0) { status_hash = 0x2089554C; }
                else { status_hash = 0xD3EFE2E5; }
                AddDatum(new(__FILE__, __LINE__) RapSheetVDDatum(name_hash, status_hash, bounty, fines, unserved, evaded, busted));
                numCars++;
            }
        }
    }
    int i = numCars;
    while (i < GetWidth() * GetHeight()) {
        i++;
        FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_CAR_%d", i), GetLocalizedString(0x73AF0386));
        FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE_%d", i), "");
        FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE2_%d", i), "");
        FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE3_%d", i), "");
        FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE4_%d", i), "");
        FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE5_%d", i), "");
        FEPrintf(GetPackageName(), FEngHashString("RAPSHEET_VALUE6_%d", i), "");
    }
    SetInitialPosition(0);
    RefreshHeader();
}
void uiRapSheetVD::RefreshHeader() {
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    HighScoresDatabase* scores = prof->GetHighScores();
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0x1FFFB988, GetLocalizedString(0x6031106E), prof->GetProfileName());
    FEPrintf(GetPackageName(), 0x1FFFB989, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
    unsigned int prefName = stable->GetPreferedCarName();
    if (prefName != 0) { FEPrintf(GetPackageName(), 0x1FFFB98A, GetLocalizedString(0xFBAF89DF), GetLocalizedString(stable->GetPreferedCarName())); }
    else { FEPrintf(GetPackageName(), 0x1FFFB98A, GetLocalizedString(0xFBAF89DF), GetLocalizedString(0x73AF0386)); }
    unsigned int prevCar = scores->GetPreviouslyPursuedCarNameHash();
    if (prevCar != 0) { FEPrintf(GetPackageName(), 0x1FFFB98B, GetLocalizedString(0x074A86E1), GetLocalizedString(scores->GetPreviouslyPursuedCarNameHash())); }
    else { FEPrintf(GetPackageName(), 0x1FFFB98B, GetLocalizedString(0x074A86E1), GetLocalizedString(0x73AF0386)); }
    ArrayScrollerMenu::RefreshHeader();
}
