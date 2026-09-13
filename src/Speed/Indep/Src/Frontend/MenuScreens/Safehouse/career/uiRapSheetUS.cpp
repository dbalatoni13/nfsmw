#include "uiRapSheetUS.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"

void RapSheetUSArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        FEngSetLanguageHash(pItemName, static_cast<RapSheetUSDatum *>(datum)->getItemName());
        FEPrintf(pUnserved, "%$d", static_cast<RapSheetUSDatum *>(datum)->getNumUnserved());
        FEPrintf(pTotal, "%$d", static_cast<RapSheetUSDatum *>(datum)->getTotalUnserved());
    }
}
uiRapSheetUS::uiRapSheetUS(ScreenConstructorData *sd) : ArrayScrollerMenu(sd, 1, 8, false), view_unserved(true) {
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEString *pItemName = FEngFindString(GetPackageName(), FEngHashString("INFRACTION_TYPE_%d", i + 1));
        FEString *pUnserved = FEngFindString(GetPackageName(), FEngHashString("UNSERVED_QTY_%d", i + 1));
        FEString *pTotal = FEngFindString(GetPackageName(), FEngHashString("TOTAL_QTY_%d", i + 1));
        AddSlot(new ("RapSheetUSArraySlot", 0) RapSheetUSArraySlot(pItemName, pItemName, pUnserved, pTotal));
    }
    Setup();
}

void uiRapSheetUS::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    ArrayScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case __PAD_BUTTON5__:
            ToggleView();
            break;
        case FEHASH_EXITCOMPLETE:
            cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false);
            break;
    }
}

void uiRapSheetUS::Setup() {
    ClearData();
    uint16 num;
    uint16 total;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);

    num = stable->GetNumInfraction(GInfractionManager::kInfraction_Speeding, view_unserved);
    total = num + stable->GetNumInfraction(GInfractionManager::kInfraction_Speeding, !view_unserved);
    AddDatum(new ("RapSheetUSDatum", 0) RapSheetUSDatum(0x676B575C, num, total));

    num = stable->GetNumInfraction(GInfractionManager::kInfraction_Racing, view_unserved);
    total = num + stable->GetNumInfraction(GInfractionManager::kInfraction_Racing, !view_unserved);
    AddDatum(new ("RapSheetUSDatum", 0) RapSheetUSDatum(0x81705AC5, num, total));

    num = stable->GetNumInfraction(GInfractionManager::kInfraction_Reckless, view_unserved);
    total = num + stable->GetNumInfraction(GInfractionManager::kInfraction_Reckless, !view_unserved);
    AddDatum(new ("RapSheetUSDatum", 0) RapSheetUSDatum(0x0E9D1CB6, num, total));

    num = stable->GetNumInfraction(GInfractionManager::kInfraction_Assault, view_unserved);
    total = num + stable->GetNumInfraction(GInfractionManager::kInfraction_Assault, !view_unserved);
    AddDatum(new ("RapSheetUSDatum", 0) RapSheetUSDatum(0x1536B1FA, num, total));

    num = stable->GetNumInfraction(GInfractionManager::kInfraction_HitAndRun, view_unserved);
    total = num + stable->GetNumInfraction(GInfractionManager::kInfraction_HitAndRun, !view_unserved);
    AddDatum(new ("RapSheetUSDatum", 0) RapSheetUSDatum(0xAAF89AB3, num, total));

    num = stable->GetNumInfraction(GInfractionManager::kInfraction_Damage, view_unserved);
    total = num + stable->GetNumInfraction(GInfractionManager::kInfraction_Damage, !view_unserved);
    AddDatum(new ("RapSheetUSDatum", 0) RapSheetUSDatum(0x706C0F0D, num, total));

    num = stable->GetNumInfraction(GInfractionManager::kInfraction_Resist, view_unserved);
    total = num + stable->GetNumInfraction(GInfractionManager::kInfraction_Resist, !view_unserved);
    AddDatum(new ("RapSheetUSDatum", 0) RapSheetUSDatum(0xAD524B30, num, total));

    num = stable->GetNumInfraction(GInfractionManager::kInfraction_OffRoad, view_unserved);
    total = num + stable->GetNumInfraction(GInfractionManager::kInfraction_OffRoad, !view_unserved);
    AddDatum(new ("RapSheetUSDatum", 0) RapSheetUSDatum(0xF9748B0B, num, total));

    FEngSetLanguageHash(GetPackageName(), 0x9D974DF3, view_unserved ? 0xC225D554 : 0x6A1151D1);
    RefreshHeader();
}

void uiRapSheetUS::RefreshHeader() {
    UserProfile &prof = *FEDatabase->GetUserProfile(0);

    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof.GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0x1FFFB988, GetLocalizedString(0x6031106E), prof.GetProfileName());

    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);

    FEPrintf(GetPackageName(), 0x1FFFB989, GetLocalizedString(view_unserved ? 0xBDFE114C : 0xAD0B7F09),
             stable->GetTotalNumInfractions(view_unserved));
    FEPrintf(GetPackageName(), 0x1FFFB98A, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
    FEPrintf(GetPackageName(), 0x1FFFB98B, GetLocalizedString(view_unserved ? 0x1FF24DD3 : 0x1E424873), stable->GetTotalFines(view_unserved));
    FEPrintf(GetPackageName(), 0x2ECAFA80, GetLocalizedString(view_unserved ? 0x8422B22A : 0x3177BB0D),
             stable->GetTotalNumInfractions(view_unserved));
    FEPrintf(GetPackageName(), 0xBBE88932, GetLocalizedString(view_unserved ? 0x8422B22A : 0x3177BB0D),
             stable->GetTotalNumInfractions(view_unserved));

    ArrayScrollerMenu::RefreshHeader();
}

void uiRapSheetUS::ToggleView() {
    view_unserved = !view_unserved;
    Setup();
}
