#include "uiInfractions.hpp"

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

PostPursuitInfractionsScreen::PostPursuitInfractionsScreen(ScreenConstructorData *sd)
    : MenuScreen(sd),                //
      WorkingCareerRecord(nullptr),  //
      bStrikeLimitReached(false),    //
      BustedTexture(0),              //
      bFirstTimeBusted(false) {
    if (!FEDatabase->GetCareerSettings()->HasBeenBustedOnce()) {
        if (TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0) <= 0) {
            for (int i = 0; i <= 3; i++) {
                TheFEMarkerManager.AddMarkerToInventory(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0);
            }
        }
        bFirstTimeBusted = true;
    }
    FEDatabase->GetCareerSettings()->SetBeenBustedOnce();

    const u32 FEObj_BustedStamp = 0x2347122A;
    FEngSetInvisible(GetPackageName(), FEObj_BustedStamp);
    BustedTexture = CalcBustedTexture();
    FEngSetTextureHash(GetPackageName(), FEObj_BustedStamp, BustedTexture);
    eLoadStreamingTexture(BustedTexture, TextureLoadedCallback, reinterpret_cast<uintptr_t>(this), 0);

    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *record = stable->GetCarRecordByHandle(FEDatabase->GetCareerSettings()->GetCurrentCar());
    WorkingCareerRecord = stable->GetCareerRecordByHandle(record->CareerHandle);

    FEInfractionsData scott_says_i_should_call_this_previous_infractions_and_phil_needs_to_have_it_spelled_correctly(
        GInfractionManager::Get().GetInfractions());

    int this_pursuit_cost = scott_says_i_should_call_this_previous_infractions_and_phil_needs_to_have_it_spelled_correctly.GetFineValue();
    const u32 FEObj_THISPURSUITCOST = 0xBD66334A;
    FEPrintf(GetPackageName(), FEObj_THISPURSUITCOST, "%$d", this_pursuit_cost);

    int num_infractions_pursuit = scott_says_i_should_call_this_previous_infractions_and_phil_needs_to_have_it_spelled_correctly.NumInfractions();
    const u32 FEObj_NUMBEROFINFRACTIONSTHISPURSUIT = 0xB967F64D;
    FEPrintf(GetPackageName(), FEObj_NUMBEROFINFRACTIONSTHISPURSUIT, "%d", num_infractions_pursuit);

    int infraction_total_cost = WorkingCareerRecord->GetInfractions(true).GetFineValue();
    const u32 FEObj_UNSERVEDINFRACTIONSCOST = 0xA4C79522;
    FEPrintf(GetPackageName(), FEObj_UNSERVEDINFRACTIONSCOST, "%$d", infraction_total_cost - this_pursuit_cost);

    int total_unserved_number = WorkingCareerRecord->GetInfractions(true).NumInfractions();
    const u32 FEObj_NUMBEROFINFRACTIONSUNSERVED = 0x5344F2A6;
    FEPrintf(GetPackageName(), FEObj_NUMBEROFINFRACTIONSUNSERVED, "%d", total_unserved_number - num_infractions_pursuit);

    bHasMarker = TheFEMarkerManager.HasMarker(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0);
    FEPrintf(GetPackageName(), 0x5B875870, "%d", TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0));
    FEPrintf(GetPackageName(), 0xEA8AECD9, "%d", TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0));
    if (!bHasMarker) {
        const u32 FEObj_Button1Text = 0xF9363F30;
        const u32 GREY = 0x163C76;
        const u32 FEObj_MARKER = 0x6B6973C1;
        const u32 FEObj_Button1 = 0xB8A7C6CC;
        FEngSetScript(GetPackageName(), FEObj_Button1Text, GREY, true);
        FEngSetScript(GetPackageName(), FEObj_MARKER, GREY, true);
        FEngSetScript(GetPackageName(), 0x39F11E5C, GREY, true);
        FEngDisableButton(GetPackageName(), FEObj_Button1);
    } else {
        const u32 FEObj_NORMAL = 0x6EBBFB68;
        FEngSetScript(GetPackageName(), 0x39F11E5C, FEObj_NORMAL, true);
    }

    AmountToPay = WorkingCareerRecord->GetInfractions(true).GetFineValue();
    const u32 FEObj_TOTALCOSTDATA = 0x854AF1F4;
    FEPrintf(GetPackageName(), FEObj_TOTALCOSTDATA, "%$d", AmountToPay);

    AmountPlayerHas = FEDatabase->GetCareerSettings()->GetCash();
    const u32 FEObj_CASHDATA = 0x1930B057;
    FEPrintf(GetPackageName(), FEObj_CASHDATA, "%$d", AmountPlayerHas);
}

PostPursuitInfractionsScreen::~PostPursuitInfractionsScreen() {
    uint32 texture = BustedTexture;
    eUnloadStreamingTexture(&texture, 1);
    WaitForResourceLoadingComplete();
}

void PostPursuitInfractionsScreen::NotifyBustedTextureLoaded() {
    const u32 FEObj_BustedStamp = 0x2347122A;
    FEngSetVisible(GetPackageName(), FEObj_BustedStamp);
}

uint32 PostPursuitInfractionsScreen::CalcBustedTexture() {
    switch (GetCurrentLanguage()) {
        case eLANGUAGE_FRENCH:
            return 0xB419F122;
        case eLANGUAGE_GERMAN:
            return 0xB419F3C3;
        case eLANGUAGE_ITALIAN:
            return 0xB419FE23;
        case eLANGUAGE_SPANISH:
            return 0xB41A2829;
        case eLANGUAGE_DUTCH:
            return 0xB419E912;
        case eLANGUAGE_SWEDISH:
            return 0xB41A2914;
        case eLANGUAGE_DANISH:
            return 0xB419E678;
        case eLANGUAGE_POLISH:
            return 0xB41A1B50;
        case eLANGUAGE_FINNISH:
            return 0xB419F002;
        case eLANGUAGE_KOREAN:
            return 0xB41A0611;
        case eLANGUAGE_CHINESE:
            return 0xB419E319;
        case eLANGUAGE_JAPANESE:
            return 0xB41A0000;
        case eLANGUAGE_THAI:
            return 0xB41A2B62;
        default:
            return 0xB419EC5F;
    }
}

void PostPursuitInfractionsScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0x35F8620B:
            if (bFirstTimeBusted) {
                FEngSetCurrentButton(GetPackageName(), FEHASH_BUTTON1);
                DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK, 0xB4EDEB6D, 0x9C14B5F1);
            } else {
                FEngSetCurrentButton(GetPackageName(), FEHASH_BUTTON2);
            }
            break;
        case 0x0C407210: {
            bool payFine = pobj->NameHash == FEHASH_BUTTON2;
            bool useMarker = pobj->NameHash == FEHASH_BUTTON1;
            if (payFine) {
                FEDatabase->GetCareerSettings()->SpendCash(AmountToPay);
                bStrikeLimitReached = WorkingCareerRecord->TheImpoundData.NotifyBusted();
                WorkingCareerRecord->ServeAllIncractions();
                WorkingCareerRecord->SetVehicleHeat(1.0f);
            } else if (useMarker) {
                TheFEMarkerManager.UtilizeMarker(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0);
                int numMarkers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0);
                FEPrintf(GetPackageName(), 0x5B875870, "%d", numMarkers);
                FEPrintf(GetPackageName(), 0xEA8AECD9, "%d", numMarkers);
                if (numMarkers <= 0) {
                    FEngSetScript(GetPackageName(), 0x6B6973C1, 0x163C76, true);
                    FEngSetScript(GetPackageName(), 0x39F11E5C, 0x163C76, true);
                }
                WorkingCareerRecord->WaiveIncractions(GInfractionManager::Get().GetInfractions());
            }
            bool cantAfford = false;
            if (!useMarker) {
                cantAfford = AmountToPay > AmountPlayerHas;
            }
            FEImpoundData::eImpoundReasons reason = FEImpoundData::IMPOUND_REASON_NONE;
            uint32 reasonHash = 0;
            if (bStrikeLimitReached) {
                reasonHash = 0x78F0E298;
                reason = FEImpoundData::IMPOUND_REASON_STRIKE_LIMIT_REACHED;
            } else if (cantAfford) {
                reasonHash = 0x1ECFFA6E;
                reason = FEImpoundData::IMPOUND_REASON_INSUFFICIENT_FUNDS;
            }
            if (reasonHash != 0) {
                WorkingCareerRecord->TheImpoundData.BecomeImpounded(reason);
                DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK, 0x34DC1BEC, reasonHash);
            } else {
                if (useMarker) {
                    cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, 0, 0);
                } else {
                    cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0x100, 0, 0);
                }
            }
            break;
        }
        case 0x34DC1BEC:
            cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0x200, 0, 0);
            break;
    }
}




inline MenuScreen *PostPursuitInfractionsScreen::Create(ScreenConstructorData *sd) {
    return new ("PostPursuitInfractionsScreen", 0) PostPursuitInfractionsScreen(sd);
}

inline void PostPursuitInfractionsScreen::TextureLoadedCallback(uint32 arg) {
    reinterpret_cast<PostPursuitInfractionsScreen *>(arg)->NotifyBustedTextureLoaded();
}
