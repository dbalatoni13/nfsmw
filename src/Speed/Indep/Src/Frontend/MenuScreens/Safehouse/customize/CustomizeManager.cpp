#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"
#include "Speed/Indep/Src/Ecstasy/eStreamingPack.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/World/CarSkin.hpp"

extern const float gTradeInFactor;

void CarCustomizeManager::TakeControl(eCustomizeEntryPoint entry_point, FECarRecord *tuning_car) {
    if (!g_bCustomizeManagerHasControl) {
        FEDatabase->SetGameMode(eFE_GAME_MODE_CUSTOMIZE);
        g_bCustomizeManagerHasControl = true;
#ifndef EA_BUILD_A124
        for (int i = 0; i < 3; i++) {
            Showcase::FromColor[i] = nullptr;
        }
#endif
        TheTempColoredPart = nullptr;
        EntryPoint = entry_point;
        TuningCar = tuning_car;
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        PreviewRecord = *stable->GetCustomizationRecordByHandle(TuningCar->Customization);
        Attrib::Gen::pvehicle vehicle(TuningCar->VehicleKey, 0, nullptr);
        stable->WriteRecordIntoPhysics(TuningCar->Handle, vehicle);
        ThePVehicle = vehicle;
        RideInfo info;
        stable->BuildRideForPlayer(TuningCar->Handle, 0, &info);
        CarViewer::SetRideInfo(&info, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
        NumPartsInCart = 0;
    }
}

void CarCustomizeManager::RelinquishControl() {
#ifndef EA_BUILD_A124
    FEDatabase->ClearGameMode(eFE_GAME_MODE_CUSTOMIZE);
    for (int i = 0; i < 3; i++) {
        delete Showcase::FromColor[i];
        Showcase::FromColor[i] = nullptr;
    }
#endif
    ClearTempColoredPart();
    g_bCustomizeManagerHasControl = false;
}

// UNSOLVED
bool CarCustomizeManager::CanTradeIn(SelectablePart *part) {
    if (!part->IsPerformancePkg()) {
        int slot = part->GetSlotID();
        if (slot <= 0x73) {
            if (slot >= 0x63) {
                return false;
            }
            if (slot < 0x4c) {
                return true;
            }
            if (slot <= 0x53) {
                return false;
            }
            if (slot == 0x5b) {
                return false;
            }
        } else if (slot == 0x7b) {
            return false;
        } else if (slot < 0x7b) {
            return true;
        } else if (slot > 0x87) {
            return true;
        } else if (slot < 0x83) {
            return true;
        }
    }
    return false;
}

void CarCustomizeManager::AddToCart(SelectablePart *part) {
    ShoppingCartItem *old_item = IsPartTypeInCart(part);
    SelectablePart *trade_in = nullptr;
    if (old_item != nullptr) {
        if (CanTradeIn(part) && (old_item->GetTradeInPart() != nullptr)) {
            trade_in = new ("SelectablePart 9", 0) SelectablePart(old_item->GetTradeInPart());
        }
        RemoveFromCart(old_item);
    } else {
        if (!part->IsPerformancePkg()) {
            if (CanTradeIn(part)) {
                CarPart *installed_car_part = GetInstalledCarPart(part->GetSlotID());
                if (installed_car_part != nullptr) {
                    int price = 0;
                    trade_in =
                        new ("SelectablePart 10", 0) SelectablePart(installed_car_part, part->GetSlotID(), installed_car_part->GetUpgradeLevel(),
                                                                    Physics::Upgrades::PUT_MAX, false, CPS_INSTALLED, price, false);
                    trade_in->SetPrice(GetPartPrice(trade_in));
                }
            }
        }
    }
    SelectablePart *buying_part = new ("SelectablePart 11", 0) SelectablePart(part);
    ShoppingCart.AddTail(new ("ShoppingCartItem", 0) ShoppingCartItem(buying_part, trade_in));
    NumPartsInCart++;
}

bool CarCustomizeManager::RemoveFromCart(ShoppingCartItem *item) {
    if (item != nullptr) {
        item->Remove();
        delete item;
        NumPartsInCart--;
        return true;
    }
    return false;
}

ShoppingCartItem *CarCustomizeManager::IsPartTypeInCart(SelectablePart *to_find) {
    if (to_find == nullptr) {
        return nullptr;
    }

    for (ShoppingCartItem *item = ShoppingCart.GetHead(); item != ShoppingCart.EndOfList(); item = item->GetNext()) {
        SelectablePart *part = item->GetBuyingPart();
        if (to_find->IsPerformancePkg()) {
            if (part->GetPhysicsType() == to_find->GetPhysicsType()) {
                return item;
            }
        } else {
            if (part->GetSlotID() == to_find->GetSlotID()) {
                return item;
            }
        }
    }

    return nullptr;
}

ShoppingCartItem *CarCustomizeManager::IsPartTypeInCart(uint32 slot_id) {
    SelectablePart test_part(nullptr, slot_id, 0, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
    return IsPartTypeInCart(&test_part);
}

ShoppingCartItem *CarCustomizeManager::IsPartTypeInCart(Physics::Upgrades::Type type) {
    SelectablePart test_part(nullptr, 0, 0, type, true, CPS_AVAILABLE, 0, false);
    return IsPartTypeInCart(&test_part);
}

ShoppingCartItem *CarCustomizeManager::IsPartInCart(SelectablePart *to_find) {
    for (ShoppingCartItem *item = ShoppingCart.GetHead(); item != ShoppingCart.EndOfList(); item = item->GetNext()) {
        SelectablePart *part = item->GetBuyingPart();
        if (to_find->IsPerformancePkg()) {
            if (part->GetPhysicsType() == to_find->GetPhysicsType() && part->GetUpgradeLevel() == to_find->GetUpgradeLevel()) {
                return item;
            }
        } else {
            if (part->GetPart() == to_find->GetPart() && part->GetSlotID() == to_find->GetSlotID()) {
                return item;
            }
        }
    }
    return nullptr;
}

CarPart *CarCustomizeManager::GetActivePartFromSlot(uint32 slot_id) {
    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id);
    if (item != nullptr) {
        return item->GetBuyingPart()->GetPart();
    }
    return gCarCustomizeManager.GetInstalledCarPart(slot_id);
}

// UNSOLVED
int CarCustomizeManager::GetCartTotal(eCustomizeCartTotals type) {
    int price = 0;
    for (ShoppingCartItem *item = ShoppingCart.GetHead(); item != ShoppingCart.EndOfList(); item = item->GetNext()) {
        if (!item->IsActive())
            continue;
        if (type == 0 || type == 2) {
            if (!item->GetBuyingPart()->IsPerformancePkg()) {
                int slot = item->GetBuyingPart()->GetSlotID();
                if (slot == 0x72)
                    continue;
                if ((slot >= 0x4f && slot <= 0x52) || (slot >= 0x85 && slot <= 0x87))
                    continue;
            }
            if (!IsInBackRoom()) {
                price += item->GetPartPrice();
            } else {
                price += 1;
            }
        }
        if (type == 1 || (type == 2 && !IsInBackRoom())) {
            if (item->GetTradeInPart() != nullptr) {
                price -= static_cast<int>(static_cast<float>(item->GetTradeInPrice()) * gTradeInFactor);
            }
        }
    }
    return price;
}

void CarCustomizeManager::Checkout() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECareerRecord *career_record = stable->GetCareerRecordByHandle(TuningCar->CareerHandle);
    for (ShoppingCartItem *item = ShoppingCart.GetHead(); item != ShoppingCart.EndOfList(); item = item->GetNext()) {
        if (item->IsActive()) {
            SelectablePart *part = item->GetBuyingPart();
            if (IsCareerMode() && item->GetBuyingPart()->GetSlotID() != CARSLOTID_DECAL_RIGHT_DOOR_RNUMBER) {
                if (!IsInBackRoom()) {
                    int cost = item->GetPartPrice() - item->GetTradeInPrice();
                    if (cost >= 0) {
                        FEDatabase->GetCareerSettings()->SpendCash(cost);
                    } else {
                        FEDatabase->GetCareerSettings()->AwardCash(-cost);
                    }
                } else {
                    if (item->GetBuyingPart()->IsPerformancePkg()) {
                        TheFEMarkerManager.UtilizeMarker(item->GetBuyingPart()->GetPhysicsType());
                    } else {
                        TheFEMarkerManager.UtilizeMarker(item->GetBuyingPart()->GetSlotID());
                    }
                }
            }
            if (part->IsPerformancePkg()) {
                InstallPerfPkg(part->GetPhysicsType(), part->GetUpgradeLevel());
            } else {
                InstallPart(part->GetSlotID(), part->GetPart());
                UpdateHeatOnVehicle(part, career_record);
            }
        }
    }
    ShoppingCart.DeleteAllElements();
    ResetPreview();
}

bool CarCustomizeManager::DoesCartHaveActiveParts() {
    for (ShoppingCartItem *item = ShoppingCart.GetHead(); item != ShoppingCart.EndOfList(); item = item->GetNext()) {
        if ((item->GetBuyingPart() != nullptr) && !item->GetBuyingPart()->IsPerformancePkg()) {
            switch (item->GetBuyingPart()->GetSlotID()) {
                case CARSLOTID_VINYL_COLOUR0_0:
                case CARSLOTID_VINYL_COLOUR0_1:
                case CARSLOTID_VINYL_COLOUR0_2:
                case CARSLOTID_VINYL_COLOUR0_3:
                    continue;
                case CARSLOTID_HUD_BACKING_COLOUR:
                case CARSLOTID_HUD_NEEDLE_COLOUR:
                case CARSLOTID_HUD_CHARACTER_COLOUR:
                    continue;
            }
        }
        if (item->IsActive())
            return true;
    }
    return false;
}

int CarCustomizeManager::GetPartPrice(SelectablePart *part) {
    int price = 0;
    if ((part != nullptr) && !IsInBackRoom()) {
        if (part->IsPerformancePkg()) {
            int level = GetMaxPackages(part->GetPhysicsType());
            level -= GetNumPackages(part->GetPhysicsType()) - part->GetUpgradeLevel();
            price = UnlockSystem::GetPerfPackageCost(GetUnlockFilter(), part->GetPhysicsType(), level, 0);
        } else {
            switch (part->GetSlotID()) {
                case CARSLOTID_VINYL_COLOUR0_0:
                case CARSLOTID_VINYL_COLOUR0_1:
                case CARSLOTID_VINYL_COLOUR0_2:
                case CARSLOTID_VINYL_COLOUR0_3:
                    break;
                case CARSLOTID_HUD_BACKING_COLOUR:
                case CARSLOTID_HUD_NEEDLE_COLOUR:
                case CARSLOTID_HUD_CHARACTER_COLOUR:
                    break;
                default:
                    price = UnlockSystem::GetCarPartCost(GetUnlockFilter(), part->GetSlotID(), part->GetPart(), 0);
            }
        }
    }
    return price;
}

void CarCustomizeManager::SetTempColoredPart(SelectablePart *part) {
    if (TheTempColoredPart != nullptr) {
        delete TheTempColoredPart;
    }
    TheTempColoredPart = part;
}

void CarCustomizeManager::ClearTempColoredPart() {
    if (TheTempColoredPart != nullptr) {
        delete TheTempColoredPart;
    }
    TheTempColoredPart = nullptr;
}

CarPart *CarCustomizeManager::GetStockCarPart(uint32 slot_id) {
    RideInfo info;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    stable->BuildRideForPlayer(TuningCar->Handle, 0, &info);
    info.SetStockParts();
    return info.GetPart(slot_id);
}

void CarCustomizeManager::ResetToStockCarParts() {
    RideInfo info;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    stable->BuildRideForPlayer(TuningCar->Handle, 0, &info);
    info.SetStockParts();
    PreviewRecord.WriteRideIntoRecord(&info);
    CarViewer::SetRideInfo(&info, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
}

void CarCustomizeManager::ResetPreview() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    PreviewRecord = *stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    RideInfo info;
    stable->BuildRideForPlayer(TuningCar->Handle, 0, &info);
    PreviewRecord.WriteRecordIntoRide(&info);
    CarViewer::SetRideInfo(&info, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);

    for (ShoppingCartItem *item = ShoppingCart.GetHead(); item != ShoppingCart.EndOfList(); item = item->GetNext()) {
        SelectablePart *part = item->GetBuyingPart();
        if (part->IsPerformancePkg()) {
            PreviewPerfPkg(part->GetPhysicsType(), part->GetUpgradeLevel());
        } else {
            PreviewPart(part->GetSlotID(), part->GetPart());
        }
    }
}

void CarCustomizeManager::PreviewPart(int slot, CarPart *part) {
    PreviewRecord.SetInstalledPart(slot, part);
    RideInfo info;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    stable->BuildRideForPlayer(TuningCar->Handle, 0, &info);
    PreviewRecord.WriteRecordIntoRide(&info);
    CarViewer::SetRideInfo(&info, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
}

void CarCustomizeManager::InstallPart(int slot, CarPart *part) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *actual_record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    actual_record->SetInstalledPart(slot, part);
    PreviewPart(slot, part);
}

CarPart *CarCustomizeManager::GetInstalledCarPart(int slot_id) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *actual_record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    return actual_record->GetInstalledPart(TuningCar->GetType(), slot_id);
}

void CarCustomizeManager::PreviewPerfPkg(Physics::Upgrades::Type part_type, int level) {
    if (level == PPL_LEVEL_JUNKMAN) {
        PreviewRecord.SetInstalledJunkman(part_type, true);
        PreviewRecord.WriteRecordIntoPhysics(ThePVehicle);

    } else {
        PreviewRecord.SetInstalledPhysics(part_type, level);
        PreviewRecord.WriteRecordIntoPhysics(ThePVehicle);
    }
}

void CarCustomizeManager::InstallPerfPkg(Physics::Upgrades::Type part_type, int level) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *actual_record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    if (level == PPL_LEVEL_JUNKMAN) {
        actual_record->SetInstalledJunkman(part_type, true);
        if (!actual_record->WriteRecordIntoPhysics(ThePVehicle)) {
            actual_record->SetInstalledJunkman(part_type, false);
            actual_record->WriteRecordIntoPhysics(ThePVehicle);
        }
    } else {
        actual_record->SetInstalledPhysics(part_type, level);
        if (!actual_record->WriteRecordIntoPhysics(ThePVehicle)) {
            actual_record->SetInstalledPhysics(part_type, 0);
            actual_record->WriteRecordIntoPhysics(ThePVehicle);
        }
    }
    PreviewPerfPkg(part_type, level);
}

bool CarCustomizeManager::IsJunkmanInstalled(Physics::Upgrades::Type type) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *actual_record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    return actual_record->GetInstalledJunkman(type);
}

int CarCustomizeManager::GetInstalledPerfPkg(Physics::Upgrades::Type type) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *actual_record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    return actual_record->GetInstalledPhysics()->Part[type];
}

int CarCustomizeManager::GetMaxPackages(Physics::Upgrades::Type type) {
    switch (type) {
        case Physics::Upgrades::PUT_TIRES:
            return 3;
        case Physics::Upgrades::PUT_BRAKES:
            return 4;
        case Physics::Upgrades::PUT_CHASSIS:
            return 3;
        case Physics::Upgrades::PUT_TRANSMISSION:
            return 4;
        case Physics::Upgrades::PUT_ENGINE:
            return 4;
        case Physics::Upgrades::PUT_INDUCTION:
            return 3;
        case Physics::Upgrades::PUT_NOS:
            return 3;
        default:
            return -1;
    }
}

int CarCustomizeManager::GetNumPackages(Physics::Upgrades::Type type) {
    return Physics::Upgrades::GetMaxLevel(ThePVehicle, type);
}

void CarCustomizeManager::MaxOutPerformance() {
    for (int i = 0; i < Physics::Upgrades::PUT_MAX; i++) {
        Physics::Upgrades::Type cur_type = static_cast<Physics::Upgrades::Type>(i);
        int max_level = GetNumPackages(cur_type);
        int unlocked_level = 0;

        for (int j = 0; j < max_level; j++) {
            SelectablePart test(nullptr, 0, j + 1, cur_type, true, CPS_AVAILABLE, 0, false);
            int unlock_level = GetMaxPackages(cur_type) - GetNumPackages(cur_type) + j + 1;
            if (!IsPartLocked(&test, unlock_level)) {
                unlocked_level = j + 1;
            }
        }

        if (unlocked_level == 0) {
            continue;
        }

        ShoppingCartItem *existing = IsPartTypeInCart(cur_type);
        if (existing != nullptr) {
            RemoveFromCart(existing);
        }

        SelectablePart *pkg = new ("SelectablePart 13", 0) SelectablePart(nullptr, 0, unlocked_level, cur_type, true, CPS_AVAILABLE, 0, false);

        uint32 status = CPS_AVAILABLE;
        int blah_level = GetMaxPackages(cur_type) - GetNumPackages(cur_type) + unlocked_level + 1;
        if (IsPartLocked(pkg, blah_level)) {
            status = CPS_LOCKED;
        } else if (IsPartNew(pkg, blah_level)) {
            status = CPS_NEW;
        }

        if (IsPartInstalled(pkg)) {
            status = status | CPS_INSTALLED;
        } else if (IsPartInCart(pkg) != nullptr) {
            status = status | CPS_IN_CART;
        }

        pkg->SetPartState(status);
        pkg->SetPrice(gCarCustomizeManager.GetPartPrice(pkg));

        AddToCart(pkg);
        delete pkg;
    }
}

float CarCustomizeManager::GetPerformanceRating(ePerformanceRatingType type, bool preview) {
    Physics::Info::Performance performance;
    if (preview) {
        Physics::Info::EstimatePerformance(ThePVehicle, performance);
    } else {
        Attrib::Gen::pvehicle vehicle(TuningCar->VehicleKey, 0, nullptr);
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        stable->WriteRecordIntoPhysics(TuningCar->Handle, vehicle);
        Physics::Info::EstimatePerformance(vehicle, performance);
    }
    switch (type) {
        case PRT_TOP_SPEED:
            return performance.TopSpeed;
        case PRT_HANDLING:
            return performance.Handling;
        case PRT_ACCELERATION:
            return performance.Acceleration;
        default:
            return 0.0f;
    }
}

void CarCustomizeManager::UpdateHeatOnVehicle(SelectablePart *part, FECareerRecord *record) {
    if ((part != nullptr) && (record != nullptr) && !part->IsPerformancePkg() && IsCareerMode()) {

        float mod;
        if (IsInBackRoom()) {
            mod = 0.75f;
        } else {
            mod = 1.0f;
        }

        switch (part->GetSlotID()) {
            case CARSLOTID_SPOILER:
                record->AdjustHeatOnSpoilerApplied(mod);
                return;
            case CARSLOTID_HOOD:
                record->AdjustHeatOnHoodApplied(mod);
                return;
            case CARSLOTID_ROOF:
                record->AdjustHeatOnRoofScoopApplied(mod);
                return;
            case CARSLOTID_FRONT_WHEEL:
                record->AdjustHeatOnRimApplied(mod);
                return;
            case CARSLOTID_WINDOW_TINT:
                record->AdjustHeatOnWindowTintApplied(mod);
                return;
            case CARSLOTID_BASE_PAINT:
                record->AdjustHeatOnPaintApplied(mod);
                return;
            case CARSLOTID_PAINT_RIM:
                record->AdjustHeatOnRimPaintApplied(mod);
                return;
            case CARSLOTID_VINYL_LAYER0:
                record->AdjustHeatOnVinylApplied(mod);
                return;
            case CARSLOTID_BODY:
            case CARSLOTID_DECAL_LEFT_DOOR_LNUMBER: // TODO: ?? is this a bug?
                record->AdjustHeatOnBodyKitApplied(mod);
                return;
            case CARSLOTID_DECAL_FRONT_WINDOW_TEX0:
            case CARSLOTID_DECAL_REAR_WINDOW_TEX0:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX0:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX1:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX2:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX3:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX4:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX5:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX0:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX1:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX2:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX3:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX4:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX5:
            case CARSLOTID_DECAL_LEFT_QUARTER_TEX0:
            case CARSLOTID_DECAL_RIGHT_QUARTER_TEX0:
                record->AdjustHeatOnDecalApplied(mod);
                return;
        }
    }
}

// UNSOLVED
eUnlockFilters CarCustomizeManager::GetUnlockFilter() {
    if (FEDatabase->IsCareerMode()) {
        if (IsInBackRoom()) {
            return UNLOCK_BACKROOM;
        }
        return UNLOCK_CAREER_MODE;
    } else {
        if (FEDatabase->IsOnlineCustomizeMode()) {
            return UNLOCK_ONLINE;
        } else if (FEDatabase->IsQuickRaceMode()) {
            return UNLOCK_QUICK_RACE;
        }
        return UNLOCK_QUICK_RACE;
    }
}

uint32 CarCustomizeManager::GetUnlockHash(eCustomizeCategory cat, int upgrade_lvl) {
    char *funk = nullptr;
    switch (cat) {
        case CC_BODY_KIT:
            funk = "PARTS_BODYKITS";
            break;
        case CC_SPOILERS:
            funk = "PARTS_SPOILERS";
            break;
        case CC_HOODS:
            funk = "PARTS_HOODS";
            break;
        case CC_ROOF_SCOOPS:
            funk = "PARTS_ROOFSCOOPS";
            break;
        case CC_RIM_BRAND_STOCK:
        case CC_RIM_BRAND_5_ZIGEN:
        case CC_RIM_BRAND_ADR:
        case CC_RIM_BRAND_BBS:
        case CC_RIM_BRAND_ENKEI:
        case CC_RIM_BRAND_KONIG:
        case CC_RIM_BRAND_LOWENHART:
        case CC_RIM_BRAND_RACING_HART:
        case CC_RIM_BRAND_OZ:
        case CC_RIM_BRAND_VOLK:
        case CC_RIM_BRAND_ROJA:
            funk = "PARTS_RIMS";
            break;
        case CC_ENGINE:
            funk = "PERF_ENGINE";
            break;
        case CC_TRANSMISSION:
            funk = "PERF_TRANSMISSION";
            break;
        case CC_SUSPENSION:
            funk = "PERF_SUSPENSION";
            break;
        case CC_NITROUS:
            funk = "PERF_NITROUS";
            break;
        case CC_TIRES:
            funk = "PERF_TIRES";
            break;
        case CC_BRAKES:
            funk = "PERF_BRAKES";
            break;
        case CC_FORCED_INDUCTION:
            if (IsTurbo())
                funk = "PERF_TURBO";
            else
                funk = "PERF_SUPERCHARGER";
            break;
        case CC_PAINT:
            funk = "VISUAL_PAINT";
            break;
        case CC_RIM_PAINT:
            funk = "VISUAL_RIMPAINT";
            break;
        case CC_WINDOW_TINT:
            funk = "VISUAL_WINDOWTINT";
            break;
        case CC_NUMBERS:
            funk = "VISUAL_NUMBERS";
            break;
        case CC_CUSTOM_HUD:
            funk = "VISUAL_HUDS";
            break;
        case CC_VINYL_GROUP_FLAME:
        case CC_VINYL_GROUP_TRIBAL:
        case CC_VINYL_GROUP_STRIPE:
        case CC_VINYL_GROUP_RACING_FLAG:
        case CC_VINYL_GROUP_NATIONAL_FLAG:
        case CC_VINYL_GROUP_BODY:
        case CC_VINYL_GROUP_UNIQUE:
        case CC_VINYL_GROUP_CONTEST:
            funk = "VISUAL_VINYLS";
            break;
        case CC_DECAL_WINDSHIELD:
        case CC_DECAL_REAR_WINDOW:
        case CC_DECAL_LEFT_DOOR:
        case CC_DECAL_RIGHT_DOOR:
        case CC_DECAL_LEFT_QP:
        case CC_DECAL_RIGHT_QP:
        case CC_DECAL_SLOT_1:
        case CC_DECAL_SLOT_2:
        case CC_DECAL_SLOT_3:
        case CC_DECAL_SLOT_4:
        case CC_DECAL_SLOT_5:
        case CC_DECAL_SLOT_6:
            funk = "VISUAL_DECALS";
            break;
    }
    if ((funk != nullptr) && upgrade_lvl != 0) {
        char label[100];
        FEngSNPrintf(label, 100, "CUSTOMIZATION_%s_%d", funk, upgrade_lvl);
        uint32 returnHash = FEngHashString(label);
        if (DoesStringExist(returnHash)) {
            return returnHash;
        }
    }
    return 0x9bb9ccc3;
}

bool CarCustomizeManager::IsPartInstalled(SelectablePart *part) {
    if (part != nullptr) {
        if (part->IsPerformancePkg()) {
            if (part->IsJunkmanPart()) {
                return IsJunkmanInstalled(part->GetPhysicsType());
            } else {
                int level = GetInstalledPerfPkg(part->GetPhysicsType());
                if (part->GetUpgradeLevel() == level) {
                    return true;
                }
            }
        } else {
            CarPart *installed = GetInstalledCarPart(part->GetSlotID());
            if (installed == part->GetPart()) {
                return true;
            }
        }
    }
    return false;
}

bool CarCustomizeManager::IsPartLocked(SelectablePart *part, int perf_unlock_level) {
    if (part->IsPerformancePkg()) {
        return !UnlockSystem::IsPerfPackageUnlocked(GetUnlockFilter(), part->GetPhysicsType(), perf_unlock_level, 0, IsInBackRoom());
    } else {
        switch (part->GetSlotID()) {
            case CARSLOTID_DECAL_FRONT_WINDOW_TEX0:
            case CARSLOTID_DECAL_REAR_WINDOW_TEX0:
                return !UnlockSystem::IsUnlockableUnlocked(GetUnlockFilter(), UNLOCKABLE_DECAL_WINDSHIELD, 1, 0, IsInBackRoom());
            case CARSLOTID_DECAL_LEFT_DOOR_TEX0:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX1:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX2:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX3:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX4:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX5:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX0:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX1:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX2:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX3:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX4:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX5:
                return !UnlockSystem::IsUnlockableUnlocked(GetUnlockFilter(), UNLOCKABLE_DECAL_LEFT_DOOR, 2, 0, IsInBackRoom());
            case CARSLOTID_DECAL_LEFT_QUARTER_TEX0:
            case CARSLOTID_DECAL_RIGHT_QUARTER_TEX0:
                return !UnlockSystem::IsUnlockableUnlocked(GetUnlockFilter(), UNLOCKABLE_DECAL_LEFT_QP, 3, 0, IsInBackRoom());
            default:
                return !UnlockSystem::IsCarPartUnlocked(GetUnlockFilter(), part->GetSlotID(), part->GetPart(), 0, IsInBackRoom());
        }
    }
}

bool CarCustomizeManager::IsPartNew(SelectablePart *part, int perf_unlock_level) {
    eUnlockableEntity ent;
    if (part->IsPerformancePkg()) {
        extern int foo; // TODO: unknown
        ent = MapPerfPkgToUnlockable(part->GetPhysicsType());
        return UnlockSystem::IsUnlockableNew(GetUnlockFilter(), ent, perf_unlock_level);
    } else {
        extern int foo; // TODO: unknown
        ent = MapCarPartToUnlockable(part->GetSlotID(), part->GetPart());
        return UnlockSystem::IsUnlockableNew(GetUnlockFilter(), ent, part->GetUpgradeLevel());
    }
}

// UNSOLVED
bool CarCustomizeManager::IsCategoryNew(uint32 cat) {
    bool answer;
    eUnlockableEntity titty;

    switch (cat) {
        case CC_PARTS: {
            for (int i = CC_BODY_KIT; i <= CC_ROOF_SCOOPS; i++) {
                if (IsCategoryNew(i))
                    return true;
            }
            return false;
        }
        case CC_PERFORMANCE: {
            for (int i = CC_ENGINE; i <= CC_FORCED_INDUCTION; i++) {
                if (IsCategoryNew(i))
                    return true;
            }
            return false;
        }
        case CC_VISUAL: {
            for (int i = CC_PAINT; i <= CC_CUSTOM_HUD; i++) {
                if (IsCategoryNew(i))
                    return true;
            }
            return false;
        }
        case CC_BODY_KIT:
            titty = UNLOCKABLE_THING_BODY_KIT;
            break;
        case CC_SPOILERS:
            titty = UNLOCKABLE_THING_SPOILERS;
            break;
        case CC_RIM_BRANDS: {
            for (int i = CC_RIM_BRAND_5_ZIGEN; i <= CC_RIM_BRAND_ROJA; i++) {
                if (IsCategoryNew(i))
                    return true;
            }
            return false;
        }
        case CC_HOODS:
            titty = UNLOCKABLE_THING_HOODS;
            break;
        case CC_ROOF_SCOOPS:
            titty = UNLOCKABLE_THING_ROOF_SCOOPS;
            break;
        case CC_CUSTOM_HUD:
            titty = UNLOCKABLE_THING_CUSTOM_HUD;
            break;
        case CC_ENGINE:
            titty = UNLOCKABLE_THING_PUT_ENGINE;
            break;
        case CC_TRANSMISSION:
            titty = UNLOCKABLE_THING_PUT_TRANSMISSION;
            break;
        case CC_SUSPENSION:
            titty = UNLOCKABLE_THING_PUT_CHASSIS;
            break;
        case CC_NITROUS:
            titty = UNLOCKABLE_THING_PUT_NOS;
            break;
        case CC_TIRES:
            titty = UNLOCKABLE_THING_PUT_TIRES;
            break;
        case CC_BRAKES:
            titty = UNLOCKABLE_THING_PUT_BRAKES;
            break;
        case CC_FORCED_INDUCTION:
            titty = UNLOCKABLE_THING_PUT_INDUCTION;
            break;
        case CC_PAINT:
            titty = UNLOCKABLE_THING_PAINTABLE_BODY;
            break;
        case CC_VINYL_TYPES: {
            for (int i = CC_VINYL_GROUP_FLAME; i <= CC_VINYL_GROUP_CONTEST; i++) {
                if (IsCategoryNew(i))
                    return true;
            }
            return false;
        }
        case CC_RIM_PAINT:
            titty = UNLOCKABLE_THING_PAINTABLE_RIMS;
            break;
        case CC_WINDOW_TINT:
            titty = UNLOCKABLE_THING_WINDOW_TINT;
            break;
        case CC_DECAL_LOCATION:
            if (IsCategoryNew(CC_DECAL_WINDSHIELD))
                return true;
            if (IsCategoryNew(CC_DECAL_LEFT_QP))
                return true;
            if (IsCategoryNew(CC_DECAL_LEFT_DOOR))
                return true;
            return false;
        case CC_NUMBERS:
            titty = UNLOCKABLE_DECAL_NUMBERS;
            break;
        case CC_VINYL_GROUP_FLAME:
            titty = static_cast<eUnlockableEntity>(0x23);
            break;
        case CC_VINYL_GROUP_TRIBAL:
            titty = static_cast<eUnlockableEntity>(0x24);
            break;
        case CC_VINYL_GROUP_STRIPE:
            titty = static_cast<eUnlockableEntity>(0x25);
            break;
        case CC_VINYL_GROUP_RACING_FLAG:
            titty = static_cast<eUnlockableEntity>(0x26);
            break;
        case CC_VINYL_GROUP_NATIONAL_FLAG:
            titty = static_cast<eUnlockableEntity>(0x27);
            break;
        case CC_VINYL_GROUP_BODY:
            titty = static_cast<eUnlockableEntity>(0x28);
            break;
        case CC_VINYL_GROUP_UNIQUE:
            titty = static_cast<eUnlockableEntity>(0x29);
            break;
        case CC_VINYL_GROUP_CONTEST:
            titty = static_cast<eUnlockableEntity>(0x2a);
            break;
        case CC_RIM_BRAND_5_ZIGEN:
            titty = static_cast<eUnlockableEntity>(0x19);
            break;
        case CC_RIM_BRAND_ADR:
            titty = static_cast<eUnlockableEntity>(0x1a);
            break;
        case CC_RIM_BRAND_BBS:
            titty = static_cast<eUnlockableEntity>(0x1b);
            break;
        case CC_RIM_BRAND_ENKEI:
            titty = static_cast<eUnlockableEntity>(0x1c);
            break;
        case CC_RIM_BRAND_KONIG:
            titty = static_cast<eUnlockableEntity>(0x1d);
            break;
        case CC_RIM_BRAND_LOWENHART:
            titty = static_cast<eUnlockableEntity>(0x1e);
            break;
        case CC_RIM_BRAND_RACING_HART:
            titty = static_cast<eUnlockableEntity>(0x1f);
            break;
        case CC_RIM_BRAND_OZ:
            titty = static_cast<eUnlockableEntity>(0x20);
            break;
        case CC_RIM_BRAND_VOLK:
            titty = static_cast<eUnlockableEntity>(0x21);
            break;
        case CC_RIM_BRAND_ROJA:
            titty = static_cast<eUnlockableEntity>(0x22);
            break;
        case CC_DECAL_WINDSHIELD:
        case CC_DECAL_REAR_WINDOW:
            titty = UNLOCKABLE_DECAL_WINDSHIELD;
            break;
        case CC_DECAL_LEFT_DOOR:
        case CC_DECAL_RIGHT_DOOR:
        case CC_DECAL_SLOT_1:
        case CC_DECAL_SLOT_2:
        case CC_DECAL_SLOT_3:
        case CC_DECAL_SLOT_4:
        case CC_DECAL_SLOT_5:
        case CC_DECAL_SLOT_6:
            titty = UNLOCKABLE_DECAL_LEFT_DOOR;
            break;
        case CC_DECAL_LEFT_QP:
        case CC_DECAL_RIGHT_QP:
            titty = UNLOCKABLE_DECAL_LEFT_QP;
            break;
        default:
            return true;
    }

    answer = UnlockSystem::IsUnlockableNew(GetUnlockFilter(), titty, -2);
    return answer;
}

bool CarCustomizeManager::IsCategoryLocked(uint32 cat, bool backroom) {
    int level = 0;
    eUnlockableEntity titty;

    switch (cat) {
        case CC_PARTS: {
            for (int i = CC_BODY_KIT; i <= CC_ROOF_SCOOPS; i++) {
                if (!IsCategoryLocked(i, backroom))
                    return false;
            }
            return true;
        }
        case CC_PERFORMANCE: {
            for (int i = CC_ENGINE; i <= CC_FORCED_INDUCTION; i++) {
                if (!IsCategoryLocked(i, backroom))
                    return false;
            }
            return true;
        }
        case CC_VISUAL: {
            for (int i = CC_PAINT; i <= CC_CUSTOM_HUD; i++) {
                if (!IsCategoryLocked(i, backroom))
                    return false;
            }
            return true;
        }
        case CC_BODY_KIT:
            titty = UNLOCKABLE_THING_BODY_KIT;
            break;
        case CC_SPOILERS:
            titty = UNLOCKABLE_THING_SPOILERS;
            break;
        case CC_RIM_BRANDS: {
            for (int i = CC_RIM_BRAND_5_ZIGEN; i <= CC_RIM_BRAND_ROJA; i++) {
                if (!IsRimCategoryLocked(i, backroom))
                    return false;
            }
            return true;
        }
        case CC_HOODS:
            titty = UNLOCKABLE_THING_HOODS;
            break;
        case CC_ROOF_SCOOPS:
            titty = UNLOCKABLE_THING_ROOF_SCOOPS;
            break;
        case CC_CUSTOM_HUD:
            titty = UNLOCKABLE_THING_CUSTOM_HUD;
            break;
        case CC_ENGINE:
            if (backroom && !CanInstallJunkman(Physics::Upgrades::PUT_ENGINE))
                return true;
            titty = UNLOCKABLE_THING_PUT_ENGINE;
            break;
        case CC_TRANSMISSION:
            if (backroom && !CanInstallJunkman(Physics::Upgrades::PUT_TRANSMISSION))
                return true;
            titty = UNLOCKABLE_THING_PUT_TRANSMISSION;
            break;
        case CC_SUSPENSION:
            if (backroom && !CanInstallJunkman(Physics::Upgrades::PUT_CHASSIS))
                return true;
            titty = UNLOCKABLE_THING_PUT_CHASSIS;
            break;
        case CC_NITROUS:
            if (backroom && !CanInstallJunkman(Physics::Upgrades::PUT_NOS))
                return true;
            titty = UNLOCKABLE_THING_PUT_NOS;
            break;
        case CC_TIRES:
            if (backroom && !CanInstallJunkman(Physics::Upgrades::PUT_TIRES))
                return true;
            titty = UNLOCKABLE_THING_PUT_TIRES;
            break;
        case CC_BRAKES:
            if (backroom && !CanInstallJunkman(Physics::Upgrades::PUT_BRAKES))
                return true;
            titty = UNLOCKABLE_THING_PUT_BRAKES;
            break;
        case CC_FORCED_INDUCTION:
            if (backroom && !CanInstallJunkman(Physics::Upgrades::PUT_INDUCTION))
                return true;
            titty = UNLOCKABLE_THING_PUT_INDUCTION;
            break;
        case CC_PAINT:
            titty = UNLOCKABLE_THING_PAINTABLE_BODY;
            break;
        case CC_VINYL_TYPES: {
            for (int i = CC_VINYL_GROUP_FLAME; i <= CC_VINYL_GROUP_CONTEST; i++) {
                if (!IsVinylCategoryLocked(i, backroom))
                    return false;
            }
            return true;
        }
        case CC_RIM_PAINT:
            titty = UNLOCKABLE_THING_PAINTABLE_RIMS;
            break;
        case CC_WINDOW_TINT:
            titty = UNLOCKABLE_THING_WINDOW_TINT;
            break;
        case CC_DECAL_LOCATION:
            if (!IsCategoryLocked(CC_DECAL_WINDSHIELD, backroom))
                return false;
            if (!IsCategoryLocked(CC_DECAL_LEFT_QP, backroom))
                return false;
            if (!IsCategoryLocked(CC_DECAL_LEFT_DOOR, backroom))
                return false;
            return true;
        case CC_NUMBERS:
            titty = UNLOCKABLE_DECAL_NUMBERS;
            break;
        case CC_VINYL_GROUP_FLAME:
        case CC_VINYL_GROUP_TRIBAL:
        case CC_VINYL_GROUP_STRIPE:
        case CC_VINYL_GROUP_RACING_FLAG:
        case CC_VINYL_GROUP_NATIONAL_FLAG:
        case CC_VINYL_GROUP_BODY:
        case CC_VINYL_GROUP_UNIQUE:
        case CC_VINYL_GROUP_CONTEST:
            return IsVinylCategoryLocked(cat, backroom);
        case CC_RIM_BRAND_5_ZIGEN:
        case CC_RIM_BRAND_ADR:
        case CC_RIM_BRAND_BBS:
        case CC_RIM_BRAND_ENKEI:
        case CC_RIM_BRAND_KONIG:
        case CC_RIM_BRAND_LOWENHART:
        case CC_RIM_BRAND_RACING_HART:
        case CC_RIM_BRAND_OZ:
        case CC_RIM_BRAND_VOLK:
        case CC_RIM_BRAND_ROJA:
            return IsRimCategoryLocked(cat, backroom);
        case CC_DECAL_WINDSHIELD:
        case CC_DECAL_REAR_WINDOW:
            level = 1;
            titty = UNLOCKABLE_DECAL_WINDSHIELD;
            break;
        case CC_DECAL_LEFT_DOOR:
        case CC_DECAL_RIGHT_DOOR:
        case CC_DECAL_SLOT_1:
        case CC_DECAL_SLOT_2:
        case CC_DECAL_SLOT_3:
        case CC_DECAL_SLOT_4:
        case CC_DECAL_SLOT_5:
        case CC_DECAL_SLOT_6:
            level = 2;
            titty = UNLOCKABLE_DECAL_LEFT_DOOR;
            break;
        case CC_DECAL_LEFT_QP:
        case CC_DECAL_RIGHT_QP:
            level = 3;
            titty = UNLOCKABLE_DECAL_LEFT_QP;
            break;
        default:
            return true;
    }

    if (backroom) {
        return !UnlockSystem::IsBackroomAvailable(GetUnlockFilter(), titty, level);
    } else {
        return !UnlockSystem::IsUnlockableUnlocked(GetUnlockFilter(), titty, level, 0, false);
    }
}

bool CarCustomizeManager::IsRimCategoryLocked(uint32 cat, bool backroom) {
    uint32 brand_name = 0;
    switch (cat) {
        case CC_RIM_BRAND_5_ZIGEN:
            brand_name = 0x352d08d1;
            break;
        case CC_RIM_BRAND_ADR:
            brand_name = 0x9136;
            break;
        case CC_RIM_BRAND_BBS:
            brand_name = 0x9536;
            break;
        case CC_RIM_BRAND_ENKEI:
            brand_name = 0x2b77feb;
            break;
        case CC_RIM_BRAND_KONIG:
            brand_name = 0x324ac97;
            break;
        case CC_RIM_BRAND_LOWENHART:
            brand_name = 0x48e25793;
            break;
        case CC_RIM_BRAND_RACING_HART:
            brand_name = 0xdd544a02;
            break;
        case CC_RIM_BRAND_OZ:
            brand_name = 0x648;
            break;
        case CC_RIM_BRAND_VOLK:
            brand_name = 0x1e6a3b;
            break;
        case CC_RIM_BRAND_ROJA:
            brand_name = 0x1c386b;
            break;
    }
    bTList<SelectablePart> list;
    GetCarPartList(CARSLOTID_FRONT_WHEEL, list, brand_name);
    bool locked = true;
    SelectablePart *part = list.GetHead();
    while (part != list.EndOfList()) {
        if (part->GetPart()->GetBrandNameHash() == brand_name && !IsPartLocked(part, 0)) {
            locked = false;
            break;
        }
        part = part->GetNext();
    }
    list.DeleteAllElements();
    if (backroom && !locked) {
        locked = true;
        if (TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_RIMS, 0) > 0) {
            locked = false;
        }
    }
    return locked;
}

bool CarCustomizeManager::IsVinylCategoryLocked(uint32 cat, bool backroom) {
    uint32 group = 0;
    switch (cat) {
        case CC_VINYL_GROUP_FLAME:
            break;
        case CC_VINYL_GROUP_TRIBAL:
            group = 1;
            break;
        case CC_VINYL_GROUP_STRIPE:
            group = 2;
            break;
        case CC_VINYL_GROUP_RACING_FLAG:
            group = 3;
            break;
        case CC_VINYL_GROUP_NATIONAL_FLAG:
            group = 4;
            break;
        case CC_VINYL_GROUP_BODY:
            group = 5;
            break;
        case CC_VINYL_GROUP_UNIQUE:
            group = 6;
            break;
        case CC_VINYL_GROUP_CONTEST:
            group = 7;
            break;
    }
    bTList<SelectablePart> list;
    GetCarPartList(CARSLOTID_VINYL_LAYER0, list, group);
    bool locked = true;
    SelectablePart *part = list.GetHead();
    while (part != list.EndOfList()) {
        if (part->GetPart()->GetGroupNumber() == group && !IsPartLocked(part, 0)) {
            locked = false;
            break;
        }
        part = static_cast<SelectablePart *>(part->GetNext());
    }
    list.DeleteAllElements();
    if (backroom && !locked) {
        locked = true;
        if (TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_VINYL, 0) > 0) {
            locked = false;
        }
    }
    return locked;
}

// UNSOLVED: GetCarTypeInfo shouldn't inline
int CarCustomizeManager::GetMinInnerRadius() {
    CarTypeInfo *info = GetCarTypeInfo(TuningCar->GetType());
    if (info != nullptr) {
        return info->GetWheelInnerRadiusMin();
    }
    return 0;
}

// UNSOLVED: GetCarTypeInfo shouldn't inline
int CarCustomizeManager::GetMaxInnerRadius() {
    CarTypeInfo *info = GetCarTypeInfo(TuningCar->GetType());
    if (info != nullptr) {
        return info->GetWheelInnerRadiusMax();
    }
    return 0;
}

// UNSOLVED
void CarCustomizeManager::GetCarPartList(int car_slot, bTList<SelectablePart> &the_list, unsigned int param) {
    CarType car_type = CARTYPE_NONE;
    if (gCarCustomizeManager.TuningCar != nullptr) {
        car_type = gCarCustomizeManager.GetTuningCarType();
    }
    CarPart *part = CarPartDB.NewGetFirstCarPart(car_type, car_slot, 0, CARPARTLEVEL_ANY);
    eUnlockableEntity unlock_ent = MapCarPartToUnlockable(car_slot, nullptr);
    for (; part != nullptr; part = CarPartDB.NewGetNextCarPart(part, car_type, car_slot, 0, CARPARTLEVEL_ANY)) {
        switch (car_slot) {
            case CARSLOTID_BODY: {
                int lod = 1;
                bool has_solid = false;
                int name_hash = part->GetModelNameHash(0, lod);
                if (name_hash && (StreamingSolidPackLoader.GetStreamingEntry(name_hash) != nullptr)) {
                    has_solid = true;
                }
                if (!has_solid) {
                    continue;
                }
                break;
            }
            case CARSLOTID_FRONT_WHEEL:
                if (param != 0) {
                    if (part->GetBrandNameHash() != param) {
                        continue;
                    }
                }
                break;
            case CARSLOTID_VINYL_LAYER0: {
                bool exists = StreamingTexturePackLoader.GetStreamingEntry(GetVinylLayerHash(part, car_type, 1)) != nullptr;
                bool is_collectors = part->GetBrandNameHash() == bStringHash("SPECIAL");
                if (!exists) {
                    continue;
                }
                if ((part->GetGroupNumber()) != param) {
                    continue;
                }
                if (is_collectors) {
                    if (!GetIsCollectorsEdition()) {
                        continue;
                    }
                }
                break;
            }
        }

        SelectablePart *sel_part;
        if (unlock_ent == UNLOCKABLE_DECAL_LEFT_DOOR || unlock_ent == UNLOCKABLE_DECAL_RIGHT_DOOR || unlock_ent == UNLOCKABLE_DECAL_LEFT_QP) {
            int level = 0;
            if (unlock_ent == UNLOCKABLE_DECAL_LEFT_DOOR) {
                level = 2;
            } else if (unlock_ent == UNLOCKABLE_DECAL_LEFT_QP) {
                level = 3;
            } else if (unlock_ent == UNLOCKABLE_DECAL_WINDSHIELD) {
                level = 1;
            }
            if (IsInBackRoom() && !UnlockSystem::IsUnlockableUnlocked(UNLOCK_CAREER_MODE, unlock_ent, level, 0, true)) {
                continue;
            }
            sel_part = new ("SelectablePart 12", 0) SelectablePart(part, car_slot, level, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
        } else {
            if (IsInBackRoom()) {
                if (!UnlockSystem::IsCarPartUnlocked(UNLOCK_CAREER_MODE, car_slot, part, 0, true)) {
                    continue;
                }
            } else if (!FEDatabase->GetCareerSettings()->HasBeatenCareer() && part->GetUpgradeLevel() == Physics::Upgrades::PUT_MAX) {
                continue;
            }
            sel_part = new ("SelectablePart 12", 0)
                SelectablePart(part, car_slot, part->GetUpgradeLevel(), Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
        }

        uint32 status = CPS_AVAILABLE;
        if (IsPartLocked(sel_part, 0)) {
            status = CPS_LOCKED;
        } else if (IsPartNew(sel_part, 0)) {
            status = CPS_NEW;
        }
        if (IsPartInstalled(sel_part)) {
            status = status | CPS_INSTALLED;
        } else if (IsPartInCart(sel_part) != nullptr) {
            status = status | CPS_IN_CART;
        }
        sel_part->SetPartState(status);
        sel_part->SetPrice(GetPartPrice(sel_part));
        the_list.AddTail(sel_part);
    }
}

void CarCustomizeManager::GetPerformancePartsList(Physics::Upgrades::Type type, bTList<SelectablePart> &the_list) {
    int num_pkgs = Physics::Upgrades::GetMaxLevel(this->ThePVehicle, type);
    for (int i = 0; i < num_pkgs; i++) {
        SelectablePart *pkg = new ("SelectablePart 13", 0) SelectablePart(nullptr, 0, i + 1, type, true, CPS_AVAILABLE, 0, false);
        int unlock_level = GetMaxPackages(type) - GetNumPackages(type) + i + 1;
        uint32 status = CPS_AVAILABLE;
        if (IsPartLocked(pkg, unlock_level)) {
            status = CPS_LOCKED;
        } else if (IsPartNew(pkg, unlock_level)) {
            status = CPS_NEW;
        }
        if (IsPartInstalled(pkg)) {
            status = status | CPS_INSTALLED;
        } else if (IsPartInCart(pkg) != nullptr) {
            status = status | CPS_IN_CART;
        }
        pkg->SetPartState(status);
        pkg->SetPrice(gCarCustomizeManager.GetPartPrice(pkg)); // gCustomizeManager instead of this
        the_list.AddTail(pkg);
    }
}

bool CarCustomizeManager::CanInstallJunkman(Physics::Upgrades::Type type) {
    return Physics::Upgrades::CanInstallJunkman(ThePVehicle, type);
}

bool CarCustomizeManager::IsCareerMode() {
    return FEDatabase->IsCareerMode() || g_bTestCareerCustomization;
}

bool CarCustomizeManager::IsTurbo() {
    Attrib::Gen::pvehicle vehicle(TuningCar->VehicleKey, 0, nullptr);
    Physics::Upgrades::SetLevel(vehicle, Physics::Upgrades::PUT_INDUCTION, 1);
    return Physics::Info::InductionType(vehicle) == 1;
}

float CarCustomizeManager::GetActualHeat() {
    if (TuningCar == nullptr) {
        return 0.0f;
    }
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECareerRecord *record = stable->GetCareerRecordByHandle(TuningCar->CareerHandle);
    if (record != nullptr) {
        return record->GetVehicleHeat();
    }
    return 0.0f;
}

float CarCustomizeManager::GetPreviewHeat(SelectablePart *part) {
    if (this->TuningCar == nullptr) {
        return 0.0f;
    }
    FECareerRecord temp_record;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECareerRecord *career_record = stable->GetCareerRecordByHandle(TuningCar->CareerHandle);
    if (career_record == nullptr) {
        return -1.0f;
    }
    temp_record.SetVehicleHeat(career_record->GetVehicleHeat());
    if ((part != nullptr) && part->GetPart() != GetInstalledCarPart(part->GetSlotID())) {
        UpdateHeatOnVehicle(part, &temp_record);
    }
    for (ShoppingCartItem *item = ShoppingCart.GetHead(); item != ShoppingCart.EndOfList(); item = item->GetNext()) {
        if (part != nullptr) {
            if (part->GetSlotID() != item->GetBuyingPart()->GetSlotID() && item->IsActive()) {
                UpdateHeatOnVehicle(item->GetBuyingPart(), &temp_record);
            }
        } else if (item->IsActive()) {
            UpdateHeatOnVehicle(item->GetBuyingPart(), &temp_record);
        }
    }
    return temp_record.GetVehicleHeat();
}

int CarCustomizeManager::GetNumCustomizeMarkers() {
    if (g_bTestCareerCustomization) {
        return 1;
    }
    return TheFEMarkerManager.GetNumCustomizeMarkers();
}

bool CarCustomizeManager::IsCastrolCar() {
    if (TuningCar->GetType() == CARTYPE_FORDGT) {
        return gEasterEggs.IsEasterEggUnlocked(EASTER_EGG_CASTROL);
    }
    return false;
}

bool CarCustomizeManager::IsRotaryCar() {
    switch (TuningCar->GetType()) {
        case CARTYPE_RX7:
        case CARTYPE_RX8:
            return true;
        default:
            return false;
    }
}

bool CarCustomizeManager::IsHeroCar() {
    return TuningCar->GetType() == CARTYPE_BMWM3GTRE46;
}

float CarCustomizeManager::GetCartHeat() {
    if (DoesCartHaveActiveParts()) {
        if (IsCareerMode()) {
            FECareerRecord temp_record;
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            FECareerRecord *career_record = stable->GetCareerRecordByHandle(TuningCar->CareerHandle);
            if (career_record == nullptr) {
                return -1.0f;
            }
            temp_record.SetVehicleHeat(career_record->GetVehicleHeat());
            for (ShoppingCartItem *item = ShoppingCart.GetHead(); item != ShoppingCart.EndOfList(); item = item->GetNext()) {
                if (item->IsActive()) {
                    UpdateHeatOnVehicle(item->GetBuyingPart(), &temp_record);
                }
            }
            return temp_record.GetVehicleHeat();
        }
    }
    return GetActualHeat();
}
