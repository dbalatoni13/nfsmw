#include "uiRapSheetUS.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
int FEPrintf(FEString* text, const char* fmt, ...);
void FEngSetLanguageHash(FEString* text, unsigned int hash);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
const char* GetLocalizedString(unsigned int hash);
void RapSheetUSDatum::NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) {}
void RapSheetUSArraySlot::Update(ArrayDatum* datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        RapSheetUSDatum* d = static_cast<RapSheetUSDatum*>(datum);
        FEngSetLanguageHash(pItemName, d->getItemName());
        FEPrintf(pUnserved, "%d", d->getNumUnserved());
        FEPrintf(pTotal, "%d", d->getTotalUnserved());
    }
}
uiRapSheetUS::uiRapSheetUS(ScreenConstructorData* sd)
    : ArrayScrollerMenu(sd, 1, 8, false) //
    , view_unserved(true)
{
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEString* pName = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_ITEM_%d", i + 1));
        FEString* pUns = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE_%d", i + 1));
        FEString* pTot = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE2_%d", i + 1));
        AddSlot(new(__FILE__, __LINE__) RapSheetUSArraySlot(reinterpret_cast<FEObject*>(pName), pName, pUns, pTot));
    }
    Setup();
}
uiRapSheetUS::~uiRapSheetUS() {}
void uiRapSheetUS::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
    case 0xC519BFC4: ToggleView(); break;
    case 0xE1FDE1D1: cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false); break;
    }
}
void uiRapSheetUS::ToggleView() { view_unserved = !view_unserved; Setup(); }
void uiRapSheetUS::Setup() {
    ClearData();
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    { unsigned short num = stable->GetNumInfraction(GInfractionManager::kInfraction_Speeding, view_unserved); unsigned short total = (num + stable->GetNumInfraction(GInfractionManager::kInfraction_Speeding, !view_unserved)) & 0xFFFF; AddDatum(new(__FILE__, __LINE__) RapSheetUSDatum(0x676B575C, num, total)); }
    { unsigned short num = stable->GetNumInfraction(GInfractionManager::kInfraction_Racing, view_unserved); unsigned short total = (num + stable->GetNumInfraction(GInfractionManager::kInfraction_Racing, !view_unserved)) & 0xFFFF; AddDatum(new(__FILE__, __LINE__) RapSheetUSDatum(0x81705AC5, num, total)); }
    { unsigned short num = stable->GetNumInfraction(GInfractionManager::kInfraction_Reckless, view_unserved); unsigned short total = (num + stable->GetNumInfraction(GInfractionManager::kInfraction_Reckless, !view_unserved)) & 0xFFFF; AddDatum(new(__FILE__, __LINE__) RapSheetUSDatum(0x0E9D1CB6, num, total)); }
    { unsigned short num = stable->GetNumInfraction(GInfractionManager::kInfraction_Assault, view_unserved); unsigned short total = (num + stable->GetNumInfraction(GInfractionManager::kInfraction_Assault, !view_unserved)) & 0xFFFF; AddDatum(new(__FILE__, __LINE__) RapSheetUSDatum(0x1536B1FA, num, total)); }
    { unsigned short num = stable->GetNumInfraction(GInfractionManager::kInfraction_HitAndRun, view_unserved); unsigned short total = (num + stable->GetNumInfraction(GInfractionManager::kInfraction_HitAndRun, !view_unserved)) & 0xFFFF; AddDatum(new(__FILE__, __LINE__) RapSheetUSDatum(0xAAF89AB3, num, total)); }
    { unsigned short num = stable->GetNumInfraction(GInfractionManager::kInfraction_Damage, view_unserved); unsigned short total = (num + stable->GetNumInfraction(GInfractionManager::kInfraction_Damage, !view_unserved)) & 0xFFFF; AddDatum(new(__FILE__, __LINE__) RapSheetUSDatum(0x706C0F0D, num, total)); }
    { unsigned short num = stable->GetNumInfraction(GInfractionManager::kInfraction_Resist, view_unserved); unsigned short total = (num + stable->GetNumInfraction(GInfractionManager::kInfraction_Resist, !view_unserved)) & 0xFFFF; AddDatum(new(__FILE__, __LINE__) RapSheetUSDatum(0xAD524B30, num, total)); }
    { unsigned short num = stable->GetNumInfraction(GInfractionManager::kInfraction_OffRoad, view_unserved); unsigned short total = (num + stable->GetNumInfraction(GInfractionManager::kInfraction_OffRoad, !view_unserved)) & 0xFFFF; AddDatum(new(__FILE__, __LINE__) RapSheetUSDatum(0xF9748B0B, num, total)); }
    unsigned int label_hash = view_unserved ? 0xC225D554 : 0x6A1151D1;
    FEngSetLanguageHash(GetPackageName(), 0x9D974DF3, label_hash);
    RefreshHeader();
}
void uiRapSheetUS::RefreshHeader() {
    const char* packageName = GetPackageName();
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    FEPlayerCarDB* stable = FEDatabase->GetPlayerCarStable(0);
    FEPrintf(packageName, 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    FEPrintf(packageName, 0x1FFFB988, GetLocalizedString(0x6031106E), prof->GetProfileName());
    unsigned int infract_string = 0xAD0B7F09;
    if (view_unserved) {
        infract_string = 0xBDFE114C;
    }
    FEPrintf(packageName, 0x1FFFB989, GetLocalizedString(infract_string), stable->GetTotalNumInfractions(view_unserved));
    FEPrintf(packageName, 0x1FFFB98A, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
    unsigned int fine_string = 0x1E424873;
    if (view_unserved) {
        fine_string = 0x1FF24DD3;
    }
    FEPrintf(packageName, 0x1FFFB98B, GetLocalizedString(fine_string), stable->GetTotalFines(view_unserved));
    unsigned int total_string = 0x3177BB0D;
    if (view_unserved) {
        total_string = 0x8422B22A;
    }
    FEPrintf(packageName, 0x2ECAFA80, GetLocalizedString(total_string), stable->GetTotalNumInfractions(view_unserved));
    total_string = 0x3177BB0D;
    if (view_unserved) {
        total_string = 0x8422B22A;
    }
    FEPrintf(packageName, 0xBBE88932, GetLocalizedString(total_string), stable->GetTotalNumInfractions(view_unserved));
    ArrayScrollerMenu::RefreshHeader();
}
