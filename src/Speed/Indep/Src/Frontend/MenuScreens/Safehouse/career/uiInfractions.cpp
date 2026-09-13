#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiInfractions.hpp"

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"

PostPursuitInfractionsScreen::PostPursuitInfractionsScreen(ScreenConstructorData *sd)
    : MenuScreen(sd), WorkingCareerRecord(nullptr), bStrikeLimitReached(false), BustedTexture(0), bFirstTimeBusted(false) {
    if (!FEDatabase->GetCareerSettings()->HasBeenBustedOnce()) {
        if (TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0) < 1) {
            for (int i = 0; i < 4; i++) {
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
    eLoadStreamingTexture(BustedTexture, TextureLoadedCallback, reinterpret_cast<unsigned int>(this), 0);

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

    int infraction_total_cost = WorkingCareerRecord->GetInfractions(true).GetFineValue() - this_pursuit_cost;
    const u32 FEObj_UNSERVEDINFRACTIONSCOST = 0xA4C79522;
    FEPrintf(GetPackageName(), FEObj_UNSERVEDINFRACTIONSCOST, "%$d", infraction_total_cost);

    int total_unserved_number = WorkingCareerRecord->GetInfractions(true).NumInfractions() - num_infractions_pursuit;
    const u32 FEObj_NUMBEROFINFRACTIONSUNSERVED = 0x5344F2A6;
    FEPrintf(GetPackageName(), FEObj_NUMBEROFINFRACTIONSUNSERVED, "%d", total_unserved_number);

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
        const u32 FEObj_NORMAL = FEHASH_NORMAL;
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
    eUnloadStreamingTexture(BustedTexture);
    WaitForResourceLoadingComplete();
}

void PostPursuitInfractionsScreen::NotifyBustedTextureLoaded() {
    const u32 FEObj_BustedStamp = 0x2347122A;
    FEngSetVisible(GetPackageName(), FEObj_BustedStamp);
}

uint32 PostPursuitInfractionsScreen::CalcBustedTexture() {
    switch (GetCurrentLanguage()) {
        case eLANGUAGE_FRENCH:
            return 0xb419f122;
        case eLANGUAGE_GERMAN:
            return 0xb419f3c3;
        case eLANGUAGE_ITALIAN:
            return 0xb419fe23;
        case eLANGUAGE_SPANISH:
            return 0xb41a2829;
        case eLANGUAGE_DUTCH:
            return 0xb419e912;
        case eLANGUAGE_SWEDISH:
            return 0xb41a2914;
        case eLANGUAGE_DANISH:
            return 0xb419e678;
        case eLANGUAGE_POLISH:
            return 0xb41a1b50;
        case eLANGUAGE_FINNISH:
            return 0xb419f002;
        case eLANGUAGE_KOREAN:
            return 0xb41a0611;
        case eLANGUAGE_CHINESE:
            return 0xb419e319;
        case eLANGUAGE_JAPANESE:
            return 0xb41a0000;
        case eLANGUAGE_THAI:
            return 0xb41a2b62;
        default:
            return 0xb419ec5f;
    }
}

void PostPursuitInfractionsScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case FEHASH_INITCOMPLETE: {
            const u32 FEObj_Button2 = 0xB8A7C6CC;
            const u32 FEObj_Button1 = 0xB8A7C6CD;
            if (bFirstTimeBusted) {
                FEngSetCurrentButton(GetPackageName(), FEObj_Button2);
                DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, 0x417b2601, dialog_message_cancelled, 0x9c14b5f1);
            } else {
                FEngSetCurrentButton(GetPackageName(), FEObj_Button1);
            }
            break;
        }
        case __BUTTON_PRESSED__: {
            bool paid_with_cash = (pobj->NameHash == 0xB8A7C6CD);
            bool paid_with_marker = (pobj->NameHash == 0xB8A7C6CC);
            bool not_enough_cash;
            bool busted_by_cross;

            if (paid_with_cash) {
                FEDatabase->GetCareerSettings()->SpendCash(AmountToPay);
                bStrikeLimitReached = WorkingCareerRecord->TheImpoundData.NotifyBusted();
                WorkingCareerRecord->ServeAllIncractions();
                WorkingCareerRecord->SetVehicleHeat(1.0f);
            } else if (paid_with_marker) {
                TheFEMarkerManager.UtilizeMarker(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0);
                int num_markers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0);
                FEPrintf(GetPackageName(), 0x5b875870, "%d", num_markers);
                FEPrintf(GetPackageName(), 0xea8aecd9, "%d", num_markers);
                if (num_markers <= 0) {
                    const u32 GREY = 0x163c76;
                    const u32 FEObj_MARKER = 0x6b6973c1;
                    FEngSetScript(GetPackageName(), FEObj_MARKER, GREY, true);
                    FEngSetScript(GetPackageName(), 0x39f11e5c, GREY, true);
                }
                WorkingCareerRecord->WaiveIncractions(GInfractionManager::Get().GetInfractions());
            }

            not_enough_cash = false;
            if (!paid_with_marker) {
                not_enough_cash = (AmountToPay > AmountPlayerHas);
            }

            FEImpoundData::eImpoundReasons impound_reason = FEImpoundData::IMPOUND_REASON_NONE;
            unsigned int message_hash = 0;
            if (bStrikeLimitReached) {
                message_hash = 0x78f0e298;
                impound_reason = FEImpoundData::IMPOUND_REASON_STRIKE_LIMIT_REACHED;
            } else if (not_enough_cash) {
                message_hash = 0x1ecffa6e;
                impound_reason = FEImpoundData::IMPOUND_REASON_INSUFFICIENT_FUNDS;
            }

            if (message_hash != 0) {
                WorkingCareerRecord->TheImpoundData.BecomeImpounded(impound_reason);
                DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, 0x417b2601, 0x34dc1bec, message_hash);
            } else {
                if (paid_with_marker) {
                    cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0, 0, false);
                } else {
                    cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0x100, 0, false);
                }
            }
            break;
        }
        case 0x34dc1bec:
            cFEng::Get()->QueuePackageSwitch("Car_Select.fng", 0x200, 0, false);
            break;
    }
}
