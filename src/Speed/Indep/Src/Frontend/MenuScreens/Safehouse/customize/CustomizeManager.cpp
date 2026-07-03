#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp"
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
        g_bCustomizeManagerHasControl = 1;
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
        // TODO: ???
        delete static_cast<CarPart *>(Showcase::FromColor[i]);
        Showcase::FromColor[i] = nullptr;
    }
#endif
    ClearTempColoredPart();
    g_bCustomizeManagerHasControl = 0;
}

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
    ShoppingCartItem *existing = IsPartTypeInCart(part);
    SelectablePart *trade_in = nullptr;
    if (existing) {
        if (CanTradeIn(part) && existing->TradeIn) {
            trade_in = new SelectablePart(existing->TradeIn);
        }
        RemoveFromCart(existing);
    } else {
        if (!part->IsPerformancePkg()) {
            if (CanTradeIn(part)) {
                CarPart *installed = GetInstalledCarPart(part->GetSlotID());
                if (installed) {
                    trade_in = new SelectablePart(installed, part->GetSlotID(), installed->GetUpgradeLevel(), Physics::Upgrades::PUT_MAX, false,
                                                  CPS_INSTALLED, 0, false);
                    trade_in->SetPrice(GetPartPrice(trade_in));
                }
            }
        }
    }
    SelectablePart *to_buy = new SelectablePart(part);
    to_buy->SetInCart();
    ShoppingCartItem *item = new ShoppingCartItem(to_buy, trade_in);
    ShoppingCart.AddTail(item);
    NumPartsInCart++;
}

bool CarCustomizeManager::RemoveFromCart(ShoppingCartItem *item) {
    if (item) {
        item->Remove();
        delete item;
        NumPartsInCart--;
        return true;
    }
    return false;
}

ShoppingCartItem *CarCustomizeManager::IsPartTypeInCart(SelectablePart *to_find) {
    if (!to_find)
        return nullptr;
    ShoppingCartItem *end = reinterpret_cast<ShoppingCartItem *>(&ShoppingCart);
    ShoppingCartItem *item = GetFirstCartItem();
    while (true) {
        if (item == end) {
            break;
        }
        SelectablePart *buying = item->ToBuy;
        if (to_find->IsPerformancePkg()) {
            if (buying->GetPhysicsType() == to_find->GetPhysicsType()) {
                return item;
            }
        } else {
            if (buying->GetSlotID() == to_find->GetSlotID()) {
                return item;
            }
        }
        item = static_cast<ShoppingCartItem *>(item->Next);
    }
    return nullptr;
}

ShoppingCartItem *CarCustomizeManager::IsPartTypeInCart(uint32 slot_id) {
    SelectablePart temp(nullptr, slot_id, 0, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
    return IsPartTypeInCart(&temp);
}

ShoppingCartItem *CarCustomizeManager::IsPartTypeInCart(Physics::Upgrades::Type type) {
    SelectablePart test_part(nullptr, 0, 0, type, true, CPS_AVAILABLE, 0, false);
    return IsPartTypeInCart(&test_part);
}

ShoppingCartItem *CarCustomizeManager::IsPartInCart(SelectablePart *to_find) {
    ShoppingCartItem *end = reinterpret_cast<ShoppingCartItem *>(&ShoppingCart);
    ShoppingCartItem *item = GetFirstCartItem();
    while (true) {
        if (item == end) {
            break;
        }
        SelectablePart *buyPart = item->ToBuy;
        if (to_find->IsPerformancePkg()) {
            if (buyPart->GetPhysicsType() == to_find->GetPhysicsType() && buyPart->GetUpgradeLevel() == to_find->GetUpgradeLevel()) {
                return item;
            }
        } else {
            if (buyPart->GetPart() == to_find->GetPart() && buyPart->GetSlotID() == to_find->GetSlotID()) {
                return item;
            }
        }
        item = static_cast<ShoppingCartItem *>(item->Next);
    }
    return nullptr;
}

CarPart *CarCustomizeManager::GetActivePartFromSlot(uint32 slot_id) {
    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id);
    if (item) {
        return item->ToBuy->GetPart();
    }
    return gCarCustomizeManager.GetInstalledCarPart(slot_id);
}

int CarCustomizeManager::GetCartTotal(eCustomizeCartTotals type) {
    int total = 0;
    for (ShoppingCartItem *item = GetFirstCartItem(); item != ShoppingCart.EndOfList(); item = item->GetNext()) {
        if (!item->IsActive())
            continue;
        if (type == 0 || type == 2) {
            SelectablePart *buy = item->GetBuyingPart();
            if (!buy->IsPerformancePkg()) {
                int slot = buy->GetSlotID();
                if (slot == 0x72)
                    continue;
                if ((slot >= 0x4f && slot <= 0x52) || (slot >= 0x85 && slot <= 0x87))
                    continue;
            }
            if (!CustomizeIsInBackRoom()) {
                total += buy->GetPrice();
            } else {
                total += 1;
            }
        }
        if (type == 1 || (type == 2 && !CustomizeIsInBackRoom())) {
            int trade_val = 0;
            if (item->GetTradeInPart()) {
                trade_val = static_cast<int>(static_cast<float>(item->GetTradeInPart()->GetPrice()) * gTradeInFactor);
            }
            total -= trade_val;
        }
    }
    return total;
}

void CarCustomizeManager::Checkout() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECareerRecord *career = stable->GetCareerRecordByHandle(TuningCar->CareerHandle);
    for (ShoppingCartItem *item = GetFirstCartItem(); item != reinterpret_cast<ShoppingCartItem *>(&ShoppingCart); item = item->GetNext()) {
        if (!item->IsActive())
            continue;
        SelectablePart *buy = item->GetBuyingPart();
        if (IsCareerMode() && buy->GetSlotID() != 0x72) {
            if (!CustomizeIsInBackRoom()) {
                int trade_val = 0;
                if (item->GetTradeInPart()) {
                    trade_val = static_cast<int>(static_cast<float>(item->GetTradeInPart()->GetPrice()) * gTradeInFactor);
                }
                int cost = buy->GetPrice() - trade_val;
                if (cost < 0) {
                    FEDatabase->GetCareerSettings()->AwardCash(-cost);
                } else {
                    FEDatabase->GetCareerSettings()->SpendCash(cost);
                }
            } else {
                if (!buy->IsPerformancePkg()) {
                    TheFEMarkerManager.UtilizeMarker(buy->GetSlotID());
                } else {
                    TheFEMarkerManager.UtilizeMarker(static_cast<Physics::Upgrades::Type>(static_cast<int>(buy->GetPhysicsType())));
                }
            }
        }
        if (!buy->IsPerformancePkg()) {
            InstallPart(buy->GetSlotID(), buy->GetPart());
            UpdateHeatOnVehicle(buy, career);
        } else {
            InstallPerfPkg(static_cast<Physics::Upgrades::Type>(static_cast<int>(buy->GetPhysicsType())), buy->GetUpgradeLevel());
        }
    }
    EmptyCart();
}

bool CarCustomizeManager::DoesCartHaveActiveParts() {
    ShoppingCartItem *end = reinterpret_cast<ShoppingCartItem *>(&ShoppingCart);
    for (ShoppingCartItem *item = GetFirstCartItem(); item != end; item = item->GetNext()) {
        SelectablePart *buy = item->GetBuyingPart();
        if (buy && !buy->IsPerformancePkg()) {
            int slot = buy->GetSlotID();
            if (slot > 0x4e) {
                if (slot <= 0x52)
                    continue;
                if (slot <= 0x87) {
                    if (slot > 0x84)
                        continue;
                }
            }
        }
        if (item->IsActive())
            return true;
    }
    return false;
}

int CarCustomizeManager::GetPartPrice(SelectablePart *part) {
    int price = 0;
    if (part && !CustomizeIsInBackRoom()) {
        if (part->IsPerformancePkg()) {
            price = GetMaxPackages(part->GetPhysicsType());
            price -= GetNumPackages(part->GetPhysicsType()) - part->GetUpgradeLevel();
            eUnlockFilters filter = GetUnlockFilter();
            price = UnlockSystem::GetPerfPackageCost(filter, part->GetPhysicsType(), price, 0);
        } else {
            if (part->GetSlotID() < 0x4f || part->GetSlotID() > 0x52) {
                if (part->GetSlotID() > 0x87 || part->GetSlotID() < 0x85) {
                    eUnlockFilters filter = GetUnlockFilter();
                    price = UnlockSystem::GetCarPartCost(filter, part->GetSlotID(), part->GetPart(), 0);
                }
            }
        }
    }
    return price;
}

void CarCustomizeManager::SetTempColoredPart(SelectablePart *part) {
    if (TheTempColoredPart) {
        delete TheTempColoredPart;
    }
    TheTempColoredPart = part;
}

void CarCustomizeManager::ClearTempColoredPart() {
    if (TheTempColoredPart) {
        delete TheTempColoredPart;
    }
    TheTempColoredPart = nullptr;
}

CarPart *CarCustomizeManager::GetStockCarPart(unsigned int slot_id) {
    RideInfo ride;
    FEDatabase->GetPlayerCarStable(0)->BuildRideForPlayer(TuningCar->Handle, 0, &ride);
    ride.SetStockParts();
    return ride.GetPart(slot_id);
}

void CarCustomizeManager::ResetToStockCarParts() {
    RideInfo ride;
    FEDatabase->GetPlayerCarStable(0)->BuildRideForPlayer(TuningCar->Handle, 0, &ride);
    ride.SetStockParts();
    PreviewRecord.WriteRideIntoRecord(&ride);
    CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
}

void CarCustomizeManager::ResetPreview() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *src = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    PreviewRecord = *src;
    RideInfo ride;
    stable->BuildRideForPlayer(TuningCar->Handle, 0, &ride);
    PreviewRecord.WriteRecordIntoRide(&ride);
    CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
    ShoppingCartItem *end = reinterpret_cast<ShoppingCartItem *>(&ShoppingCart);
    for (ShoppingCartItem *item = GetFirstCartItem(); item != end; item = item->GetNext()) {
        SelectablePart *buy = item->GetBuyingPart();
        if (buy->IsPerformancePkg()) {
            PreviewPerfPkg(static_cast<Physics::Upgrades::Type>(static_cast<int>(buy->GetPhysicsType())), buy->GetUpgradeLevel());
        } else {
            PreviewPart(buy->GetSlotID(), buy->GetPart());
        }
    }
}

void CarCustomizeManager::PreviewPart(int slot_id, CarPart *part) {
    PreviewRecord.SetInstalledPart(slot_id, part);
    RideInfo ride;
    FEDatabase->GetPlayerCarStable(0)->BuildRideForPlayer(TuningCar->Handle, 0, &ride);
    PreviewRecord.WriteRecordIntoRide(&ride);
    CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
}

void CarCustomizeManager::InstallPart(int slot_id, CarPart *part) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    record->SetInstalledPart(slot_id, part);
    PreviewPart(slot_id, part);
}

CarPart *CarCustomizeManager::GetInstalledCarPart(int slot_id) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    return record->GetInstalledPart(TuningCar->GetType(), slot_id);
}

void CarCustomizeManager::PreviewPerfPkg(Physics::Upgrades::Type part_type, int level) {
    if (level == 7) {
        PreviewRecord.SetInstalledJunkman(part_type, true);
        PreviewRecord.WriteRecordIntoPhysics(ThePVehicle);

    } else {
        PreviewRecord.SetInstalledPhysics(part_type, level);
        PreviewRecord.WriteRecordIntoPhysics(ThePVehicle);
    }
}

void CarCustomizeManager::InstallPerfPkg(Physics::Upgrades::Type part_type, int level) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    if (level == 7) {
        record->SetInstalledJunkman(part_type, true);
        if (!record->WriteRecordIntoPhysics(ThePVehicle)) {
            record->SetInstalledJunkman(part_type, false);
            record->WriteRecordIntoPhysics(ThePVehicle);
        }
    } else {
        record->SetInstalledPhysics(part_type, level);
        if (!record->WriteRecordIntoPhysics(ThePVehicle)) {
            record->SetInstalledPhysics(part_type, 0);
            record->WriteRecordIntoPhysics(ThePVehicle);
        }
    }
    PreviewPerfPkg(part_type, level);
}

bool CarCustomizeManager::IsJunkmanInstalled(Physics::Upgrades::Type type) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    return record->GetInstalledJunkman(type);
}

int CarCustomizeManager::GetInstalledPerfPkg(Physics::Upgrades::Type type) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *actual_record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    return actual_record->GetInstalledPhysics()->Part[type];
}

int CarCustomizeManager::GetMaxPackages(Physics::Upgrades::Type type) {
    switch (type) {
        case 0:
            return 3;
        case 1:
            return 4;
        case 2:
            return 3;
        case 3:
            return 4;
        case 4:
            return 4;
        case 5:
            return 3;
        case 6:
            return 3;
        default:
            return -1;
    }
}

int CarCustomizeManager::GetNumPackages(Physics::Upgrades::Type type) {
    return Physics::Upgrades::GetMaxLevel(ThePVehicle, type);
}

void CarCustomizeManager::MaxOutPerformance() {
    for (int i = 0; i < 7; i++) {
        Physics::Upgrades::Type type = static_cast<Physics::Upgrades::Type>(i);
        int num_packages = GetNumPackages(type);
        int best_level = 0;

        for (int j = 0; j < num_packages; j++) {
            SelectablePart sp(nullptr, 0, j + 1, type, true, CPS_AVAILABLE, 0, false);
            int level_param = GetMaxPackages(type) - GetNumPackages(type) + j + 1;
            if (!IsPartLocked(&sp, level_param)) {
                best_level = j + 1;
            }
        }

        if (best_level != 0) {
            ShoppingCartItem *existing = IsPartTypeInCart(type);
            if (existing) {
                RemoveFromCart(existing);
            }

            SelectablePart *sp = new SelectablePart(nullptr, 0, best_level, type, true, CPS_AVAILABLE, 0, false);

            eCustomizePartState status = CPS_AVAILABLE;
            int level_param = GetMaxPackages(type) - GetNumPackages(type) + best_level + 1;
            if (IsPartLocked(sp, level_param)) {
                status = CPS_LOCKED;
            } else if (IsPartNew(sp, level_param)) {
                status = CPS_NEW;
            }

            if (IsPartInstalled(sp)) {
                status = static_cast<eCustomizePartState>(status | CPS_INSTALLED);
            } else if (IsPartInCart(sp)) {
                status = static_cast<eCustomizePartState>(status | CPS_IN_CART);
            }

            sp->SetPartState(static_cast<unsigned int>(status));
            sp->SetPrice(gCarCustomizeManager.GetPartPrice(sp));

            AddToCart(sp);
            delete sp;
        }
    }
}

float CarCustomizeManager::GetPerformanceRating(ePerformanceRatingType type, bool preview) {
    Physics::Info::Performance perf;
    if (!preview) {
        Attrib::Gen::pvehicle pveh(TuningCar->VehicleKey, 0, nullptr);
        FEDatabase->GetPlayerCarStable(0)->WriteRecordIntoPhysics(TuningCar->Handle, pveh);
        Physics::Info::EstimatePerformance(pveh, perf);
    } else {
        Physics::Info::EstimatePerformance(ThePVehicle, perf);
    }
    switch (type) {
        case 0:
            return perf.TopSpeed;
        case 1:
            return perf.Handling;
        case 2:
            return perf.Acceleration;
        default:
            return perf.Handling;
    }
}

void CarCustomizeManager::UpdateHeatOnVehicle(SelectablePart *part, FECareerRecord *record) {
    if (!part)
        return;
    if (!record)
        return;
    if (part->IsPerformancePkg())
        return;
    if (!IsCareerMode())
        return;

    register float heat_factor = 1.0f;
    if (CustomizeIsInBackRoom()) {
        heat_factor = 0.75f;
    }

    int slot = part->GetSlotID();
    if (slot != 0x53) {
        if (slot < 0x54) {
            if (slot == 0x3f)
                goto call_hood;
            if (slot > 0x3f) {
                if (slot == 0x4c)
                    goto call_paint;
                if (slot > 0x4c) {
                    if (slot == 0x4d)
                        goto call_vinyl;
                    if (slot != 0x4e)
                        return;
                    goto call_rimpaint;
                }
                if (slot != 0x42)
                    return;
                goto call_rim;
            }
            if (slot == 0x2c)
                goto call_spoiler;
            if (slot > 0x2c) {
                if (slot != 0x3e)
                    return;
                goto call_roofscoop;
            }
            if (slot != 0x17)
                return;
            goto call_bodykit;
        }
        if (slot < 0x71) {
            if (slot >= 0x6b)
                goto call_decal;
            if (slot > 0x68) {
                if (slot != 0x69)
                    return;
                goto call_bodykit;
            }
            if (slot >= 0x63)
                goto call_decal;
            if (slot == 0x5b)
                goto call_decal;
            return;
        }
        if (slot == 0x7b)
            goto call_decal;
        if (slot > 0x7b) {
            if (slot != 0x83)
                return;
            goto call_windowtint;
        }
        if (slot != 0x73)
            return;
    }
    goto call_decal;
call_spoiler:
    record->AdjustHeatOnSpoilerApplied(heat_factor);
    return;
call_hood:
    record->AdjustHeatOnHoodApplied(heat_factor);
    return;
call_roofscoop:
    record->AdjustHeatOnRoofScoopApplied(heat_factor);
    return;
call_rim:
    record->AdjustHeatOnRimApplied(heat_factor);
    return;
call_windowtint:
    record->AdjustHeatOnWindowTintApplied(heat_factor);
    return;
call_paint:
    record->AdjustHeatOnPaintApplied(heat_factor);
    return;
call_rimpaint:
    record->AdjustHeatOnRimPaintApplied(heat_factor);
    return;
call_vinyl:
    record->AdjustHeatOnVinylApplied(heat_factor);
    return;
call_bodykit:
    record->AdjustHeatOnBodyKitApplied(heat_factor);
    return;
call_decal:
    record->AdjustHeatOnDecalApplied(heat_factor);
    return;
}

eUnlockFilters CarCustomizeManager::GetUnlockFilter() {
    if (FEDatabase->IsCareerMode()) {
        if (IsInBackRoom()) {
            return UNLOCK_BACKROOM;
        }
        return UNLOCK_CAREER_MODE;
    }
    if (FEDatabase->IsOnlineCustomizeMode()) {
        return UNLOCK_ONLINE;
    }
    return UNLOCK_QUICK_RACE;
}

unsigned int CarCustomizeManager::GetUnlockHash(eCustomizeCategory cat, int upgrade_lvl) {
    const char *name = nullptr;
    switch (cat) {
        case 0x101:
            name = "PARTS_BODYKITS";
            break;
        case 0x102:
            name = "PARTS_SPOILERS";
            break;
        case 0x104:
            name = "PARTS_HOODS";
            break;
        case 0x105:
            name = "PARTS_ROOFSCOOPS";
            break;
        case 0x201:
            name = "PERF_ENGINE";
            break;
        case 0x202:
            name = "PERF_TRANSMISSION";
            break;
        case 0x203:
            name = "PERF_SUSPENSION";
            break;
        case 0x204:
            name = "PERF_NITROUS";
            break;
        case 0x205:
            name = "PERF_TIRES";
            break;
        case 0x206:
            name = "PERF_BRAKES";
            break;
        case 0x207:
            if (IsTurbo())
                name = "PERF_TURBO";
            else
                name = "PERF_SUPERCHARGER";
            break;
        case 0x301:
            name = "VISUAL_PAINT";
            break;
        case 0x303:
            name = "VISUAL_RIMPAINT";
            break;
        case 0x304:
            name = "VISUAL_WINDOWTINT";
            break;
        case 0x306:
            name = "VISUAL_NUMBERS";
            break;
        case 0x307:
            name = "VISUAL_HUDS";
            break;
        case 0x402:
        case 0x403:
        case 0x404:
        case 0x405:
        case 0x406:
        case 0x407:
        case 0x408:
        case 0x409:
            name = "VISUAL_VINYLS";
            break;
        case 0x501:
        case 0x502:
        case 0x503:
        case 0x504:
        case 0x505:
        case 0x506:
        case 0x601:
        case 0x602:
        case 0x603:
        case 0x604:
        case 0x605:
        case 0x606:
            name = "VISUAL_DECALS";
            break;
        case 0x701:
        case 0x702:
        case 0x703:
        case 0x704:
        case 0x705:
        case 0x706:
        case 0x707:
        case 0x708:
        case 0x709:
        case 0x70a:
        case 0x70b:
            name = "PARTS_RIMS";
            break;
    }
    if (name && upgrade_lvl) {
        char buf[100];
        FEngSNPrintf(buf, 100, "CUSTOMIZATION_%s_%d", name, upgrade_lvl);
        unsigned int hash = FEngHashString(buf);
        if (DoesStringExist(hash) == 1) {
            return hash;
        }
    }
    return 0x9bb9ccc3;
}

bool CarCustomizeManager::IsPartInstalled(SelectablePart *part) {
    if (part) {
        if (part->IsPerformancePkg()) {
            if (part->IsJunkmanPart()) {
                return IsJunkmanInstalled(part->GetPhysicsType());
            }
            int lvl = GetInstalledPerfPkg(part->GetPhysicsType());
            if (static_cast<int>(part->GetUpgradeLevel()) == lvl) {
                return true;
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
        int slot = part->GetSlotID();
        switch (slot) {
            case 0x53:
            case 0x5b: {

                return !UnlockSystem::IsUnlockableUnlocked(GetUnlockFilter(), static_cast<eUnlockableEntity>(0x2c), 1, 0, IsInBackRoom());
                break;
            }
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
            case 0x68:
            case 0x6b:
            case 0x6c:
            case 0x6d:
            case 0x6e:
            case 0x6f:
            case 0x70: {
                return !UnlockSystem::IsUnlockableUnlocked(GetUnlockFilter(), static_cast<eUnlockableEntity>(0x2e), 2, 0, IsInBackRoom());
                break;
            }
            case 0x73:
            case 0x7b: {
                return !UnlockSystem::IsUnlockableUnlocked(GetUnlockFilter(), static_cast<eUnlockableEntity>(0x30), 3, 0, IsInBackRoom());
                break;
            }
            default: {
                return !UnlockSystem::IsCarPartUnlocked(GetUnlockFilter(), part->GetSlotID(), part->GetPart(), 0, IsInBackRoom());
                break;
            }
        }
    }
}

bool CarCustomizeManager::IsPartNew(SelectablePart *part, int perf_unlock_level) {
    eUnlockableEntity ent;
    eUnlockFilters filter;
    if (!part->IsPerformancePkg()) {
        ent = MapCarPartToUnlockable(part->GetSlotID(), part->GetPart());
        filter = GetUnlockFilter();
        perf_unlock_level = part->GetUpgradeLevel();
    } else {
        ent = MapPerfPkgToUnlockable(part->GetPhysicsType());
        filter = GetUnlockFilter();
    }
    return UnlockSystem::IsUnlockableNew(filter, ent, perf_unlock_level);
}

bool CarCustomizeManager::IsCategoryNew(unsigned int cat) {
    eUnlockableEntity titty;

    switch (cat) {
        case 0x801: {
            for (unsigned int i = 0x101; i <= 0x105; i++) {
                if (IsCategoryNew(i))
                    return true;
            }
            return false;
        }
        case 0x802: {
            for (unsigned int i = 0x201; i <= 0x207; i++) {
                if (IsCategoryNew(i))
                    return true;
            }
            return false;
        }
        case 0x803: {
            for (unsigned int i = 0x301; i <= 0x307; i++) {
                if (IsCategoryNew(i))
                    return true;
            }
            return false;
        }
        case 0x101:
            titty = static_cast<eUnlockableEntity>(0xb);
            break;
        case 0x102:
            titty = static_cast<eUnlockableEntity>(0xc);
            break;
        case 0x103: {
            for (unsigned int i = 0x702; i <= 0x70b; i++) {
                if (IsCategoryNew(i))
                    return true;
            }
            return false;
        }
        case 0x104:
            titty = static_cast<eUnlockableEntity>(0xe);
            break;
        case 0x105:
            titty = static_cast<eUnlockableEntity>(0xf);
            break;
        case 0x307:
            titty = static_cast<eUnlockableEntity>(0x11);
            break;
        case 0x201:
            titty = static_cast<eUnlockableEntity>(8);
            break;
        case 0x202:
            titty = static_cast<eUnlockableEntity>(7);
            break;
        case 0x203:
            titty = static_cast<eUnlockableEntity>(6);
            break;
        case 0x204:
            titty = static_cast<eUnlockableEntity>(10);
            break;
        case 0x205:
            titty = static_cast<eUnlockableEntity>(4);
            break;
        case 0x206:
            titty = static_cast<eUnlockableEntity>(5);
            break;
        case 0x207:
            titty = static_cast<eUnlockableEntity>(9);
            break;
        case 0x301:
            titty = static_cast<eUnlockableEntity>(0x17);
            break;
        case 0x302: {
            for (unsigned int i = 0x402; i <= 0x409; i++) {
                if (IsCategoryNew(i))
                    return true;
            }
            return false;
        }
        case 0x303:
            titty = static_cast<eUnlockableEntity>(0x18);
            break;
        case 0x304:
            titty = static_cast<eUnlockableEntity>(0x12);
            break;
        case 0x305:
            if (IsCategoryNew(0x501))
                return true;
            if (IsCategoryNew(0x505))
                return true;
            if (IsCategoryNew(0x503))
                return true;
            return false;
        case 0x306:
            titty = static_cast<eUnlockableEntity>(0x2b);
            break;
        case 0x402:
            titty = static_cast<eUnlockableEntity>(0x23);
            break;
        case 0x403:
            titty = static_cast<eUnlockableEntity>(0x24);
            break;
        case 0x404:
            titty = static_cast<eUnlockableEntity>(0x25);
            break;
        case 0x405:
            titty = static_cast<eUnlockableEntity>(0x26);
            break;
        case 0x406:
            titty = static_cast<eUnlockableEntity>(0x27);
            break;
        case 0x407:
            titty = static_cast<eUnlockableEntity>(0x28);
            break;
        case 0x408:
            titty = static_cast<eUnlockableEntity>(0x29);
            break;
        case 0x409:
            titty = static_cast<eUnlockableEntity>(0x2a);
            break;
        case 0x702:
            titty = static_cast<eUnlockableEntity>(0x19);
            break;
        case 0x703:
            titty = static_cast<eUnlockableEntity>(0x1a);
            break;
        case 0x704:
            titty = static_cast<eUnlockableEntity>(0x1b);
            break;
        case 0x705:
            titty = static_cast<eUnlockableEntity>(0x1c);
            break;
        case 0x706:
            titty = static_cast<eUnlockableEntity>(0x1d);
            break;
        case 0x707:
            titty = static_cast<eUnlockableEntity>(0x1e);
            break;
        case 0x708:
            titty = static_cast<eUnlockableEntity>(0x1f);
            break;
        case 0x709:
            titty = static_cast<eUnlockableEntity>(0x20);
            break;
        case 0x70a:
            titty = static_cast<eUnlockableEntity>(0x21);
            break;
        case 0x70b:
            titty = static_cast<eUnlockableEntity>(0x22);
            break;
        case 0x501:
        case 0x502:
            titty = static_cast<eUnlockableEntity>(0x2c);
            break;
        case 0x503:
        case 0x504:
        case 0x601:
        case 0x602:
        case 0x603:
        case 0x604:
        case 0x605:
        case 0x606:
            titty = static_cast<eUnlockableEntity>(0x2e);
            break;
        case 0x505:
        case 0x506:
            titty = static_cast<eUnlockableEntity>(0x30);
            break;
        default:
            return true;
    }

    eUnlockFilters filter = GetUnlockFilter();
    return UnlockSystem::IsUnlockableNew(filter, titty, -2);
}

bool CarCustomizeManager::IsCategoryLocked(unsigned int cat, bool backroom) {
    eUnlockableEntity titty;
    int level = 0;

    switch (cat) {
        case 0x801: {
            for (unsigned int i = 0x101; i <= 0x105; i++) {
                if (!IsCategoryLocked(i, backroom))
                    return false;
            }
            return true;
        }
        case 0x802: {
            for (unsigned int i = 0x201; i <= 0x207; i++) {
                if (!IsCategoryLocked(i, backroom))
                    return false;
            }
            return true;
        }
        case 0x803: {
            for (unsigned int i = 0x301; i <= 0x307; i++) {
                if (!IsCategoryLocked(i, backroom))
                    return false;
            }
            return true;
        }
        case 0x101:
            titty = static_cast<eUnlockableEntity>(0xb);
            break;
        case 0x102:
            titty = static_cast<eUnlockableEntity>(0xc);
            break;
        case 0x103: {
            for (unsigned int i = 0x702; i <= 0x70b; i++) {
                if (!IsRimCategoryLocked(i, backroom))
                    return false;
            }
            return true;
        }
        case 0x104:
            titty = static_cast<eUnlockableEntity>(0xe);
            break;
        case 0x105:
            titty = static_cast<eUnlockableEntity>(0xf);
            break;
        case 0x307:
            titty = static_cast<eUnlockableEntity>(0x11);
            break;
        case 0x201:
            if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(4)))
                return true;
            titty = static_cast<eUnlockableEntity>(8);
            break;
        case 0x202:
            if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(3)))
                return true;
            titty = static_cast<eUnlockableEntity>(7);
            break;
        case 0x203:
            if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(2)))
                return true;
            titty = static_cast<eUnlockableEntity>(6);
            break;
        case 0x204:
            if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(6)))
                return true;
            titty = static_cast<eUnlockableEntity>(10);
            break;
        case 0x205:
            if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(0)))
                return true;
            titty = static_cast<eUnlockableEntity>(4);
            break;
        case 0x206:
            if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(1)))
                return true;
            titty = static_cast<eUnlockableEntity>(5);
            break;
        case 0x207:
            if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(5)))
                return true;
            titty = static_cast<eUnlockableEntity>(9);
            break;
        case 0x301:
            titty = static_cast<eUnlockableEntity>(0x17);
            break;
        case 0x302: {
            for (unsigned int i = 0x402; i <= 0x409; i++) {
                if (!IsVinylCategoryLocked(i, backroom))
                    return false;
            }
            return true;
        }
        case 0x303:
            titty = static_cast<eUnlockableEntity>(0x18);
            break;
        case 0x304:
            titty = static_cast<eUnlockableEntity>(0x12);
            break;
        case 0x305:
            if (!IsCategoryLocked(0x501, backroom))
                return false;
            if (!IsCategoryLocked(0x505, backroom))
                return false;
            if (!IsCategoryLocked(0x503, backroom))
                return false;
            return true;
        case 0x306:
            titty = static_cast<eUnlockableEntity>(0x2b);
            break;
        case 0x402:
        case 0x403:
        case 0x404:
        case 0x405:
        case 0x406:
        case 0x407:
        case 0x408:
        case 0x409:
            return IsVinylCategoryLocked(cat, backroom);
        case 0x702:
        case 0x703:
        case 0x704:
        case 0x705:
        case 0x706:
        case 0x707:
        case 0x708:
        case 0x709:
        case 0x70a:
        case 0x70b:
            return IsRimCategoryLocked(cat, backroom);
        case 0x501:
        case 0x502:
            level = 1;
            titty = static_cast<eUnlockableEntity>(0x2c);
            break;
        case 0x503:
        case 0x504:
        case 0x601:
        case 0x602:
        case 0x603:
        case 0x604:
        case 0x605:
        case 0x606:
            level = 2;
            titty = static_cast<eUnlockableEntity>(0x2e);
            break;
        case 0x505:
        case 0x506:
            level = 3;
            titty = static_cast<eUnlockableEntity>(0x30);
            break;
        default:
            return true;
    }

    eUnlockFilters filter = GetUnlockFilter();
    if (backroom) {
        return !UnlockSystem::IsBackroomAvailable(filter, titty, level);
    } else {
        return !UnlockSystem::IsUnlockableUnlocked(filter, titty, level, 0, false);
    }
}

bool CarCustomizeManager::IsRimCategoryLocked(unsigned int cat, bool backroom) {
    unsigned int brand_name = 0;
    switch (cat) {
        case 0x702:
            brand_name = 0x352d08d1;
            break;
        case 0x703:
            brand_name = 0x9136;
            break;
        case 0x704:
            brand_name = 0x9536;
            break;
        case 0x705:
            brand_name = 0x2b77feb;
            break;
        case 0x706:
            brand_name = 0x324ac97;
            break;
        case 0x707:
            brand_name = 0x48e25793;
            break;
        case 0x708:
            brand_name = 0xdd544a02;
            break;
        case 0x709:
            brand_name = 0x648;
            break;
        case 0x70a:
            brand_name = 0x1e6a3b;
            break;
        case 0x70b:
            brand_name = 0x1c386b;
            break;
    }
    bTList<SelectablePart> list;
    GetCarPartList(0x42, list, brand_name);
    bool locked = true;
    SelectablePart *part = list.GetHead();
    while (part != list.EndOfList()) {
        if (part->GetPart()->GetBrandNameHash() == brand_name && !IsPartLocked(part, 0)) {
            locked = false;
            break;
        }
        part = static_cast<SelectablePart *>(part->GetNext());
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

bool CarCustomizeManager::IsVinylCategoryLocked(unsigned int cat, bool backroom) {
    unsigned int group = 0;
    switch (cat) {
        case 0x402:
            break;
        case 0x403:
            group = 1;
            break;
        case 0x404:
            group = 2;
            break;
        case 0x405:
            group = 3;
            break;
        case 0x406:
            group = 4;
            break;
        case 0x407:
            group = 5;
            break;
        case 0x408:
            group = 6;
            break;
        case 0x409:
            group = 7;
            break;
    }
    bTList<SelectablePart> list;
    GetCarPartList(0x4d, list, group);
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

int CarCustomizeManager::GetMinInnerRadius() {
    CarTypeInfo *info = GetCarTypeInfo(TuningCar->GetType());
    if (info) {
        return info->WheelInnerRadiusMin;
    }
    return 0;
}

int CarCustomizeManager::GetMaxInnerRadius() {
    CarTypeInfo *info = GetCarTypeInfo(TuningCar->GetType());
    if (info) {
        return info->WheelInnerRadiusMax;
    }
    return 0;
}

void CarCustomizeManager::GetCarPartList(int car_slot, bTList<SelectablePart> &the_list, unsigned int param) {
    CarType cartype = static_cast<CarType>(-1);
    if (gCarCustomizeManager.TuningCar) {
        cartype = gCarCustomizeManager.TuningCar->GetType();
    }
    CarPart *part = CarPartDB.NewGetFirstCarPart(cartype, car_slot, 0, -1);
    eUnlockableEntity unlockable = MapCarPartToUnlockable(car_slot, nullptr);
    while (part) {
        int next_slot = car_slot;
        if (car_slot == 0x17) {
            bool valid = false;
            unsigned int modelHash = part->GetModelNameHash(0, 1);
            if (modelHash && StreamingSolidPackLoader.GetStreamingEntry(modelHash)) {
                valid = true;
            }
            if (!valid)
                goto next_part;
        } else if (car_slot == 0x42) {
            if (param != 0) {
                if (part->GetAppliedAttributeUParam(0xebb03e66, 0) != param) {
                    goto next_part;
                }
            }
        } else if (car_slot == 0x4d) {
            unsigned int vinylHash = GetVinylLayerHash(part, cartype, 1);
            eStreamingEntry *streaming = StreamingTexturePackLoader.GetStreamingEntry(vinylHash);
            unsigned int brand = part->GetAppliedAttributeUParam(0xebb03e66, 0);
            unsigned int specialHash = bStringHash("SPECIAL");
            bool isSpecial = brand == specialHash;
            if (!streaming) {
                goto next_part;
            }
            if ((part->GetGroupNumber() & 0x1f) != param) {
                goto next_part;
            }
            if (isSpecial) {
                if (!GetIsCollectorsEdition()) {
                    goto next_part;
                }
            }
        }

        {
            SelectablePart *sp;
            if (unlockable == 0x2e || unlockable == 0x2c || unlockable == 0x30) {
                int level = 0;
                if (unlockable == 0x2e) {
                    level = 2;
                } else if (unlockable == 0x30) {
                    level = 3;
                } else {
                    level = 1;
                }
                if (CustomizeIsInBackRoom() && !UnlockSystem::IsUnlockableUnlocked(UNLOCK_CAREER_MODE, unlockable, level, 0, true)) {
                    goto next_part;
                }
                sp = new SelectablePart(part, car_slot, level, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
            } else {
                if (CustomizeIsInBackRoom()) {
                    if (!UnlockSystem::IsCarPartUnlocked(UNLOCK_CAREER_MODE, car_slot, part, 0, true)) {
                        goto next_part;
                    }
                } else if (!FEDatabase->GetCareerSettings()->HasBeatenCareer() && part->GetUpgradeLevel() == Physics::Upgrades::PUT_MAX) {
                    goto next_part;
                }
                sp = new SelectablePart(part, car_slot, part->GetUpgradeLevel(), Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
            }

            {
                unsigned int state = CPS_AVAILABLE;
                if (IsPartLocked(sp, 0)) {
                    state = CPS_LOCKED;
                } else if (IsPartNew(sp, 0)) {
                    state = CPS_NEW;
                }
                if (IsPartInstalled(sp)) {
                    state = state | CPS_INSTALLED;
                } else if (IsPartInCart(sp)) {
                    state = state | CPS_IN_CART;
                }
                sp->SetPartState(state);
                sp->SetPrice(GetPartPrice(sp));
                the_list.AddTail(sp);
            }
        }
    next_part:
        part = CarPartDB.NewGetNextCarPart(part, cartype, next_slot, 0, -1);
    }
}

void CarCustomizeManager::GetPerformancePartsList(Physics::Upgrades::Type type, bTList<SelectablePart> &the_list) {
    int max_level = GetMaxPackages(type);
    if (max_level > 0) {
        int i = 0;
        do {
            int level = i + 1;
            SelectablePart *sp = new SelectablePart(nullptr, 0, level, type, true, CPS_AVAILABLE, 0, false);
            eCustomizePartState state = CPS_AVAILABLE;
            int remaining = GetMaxPackages(type) - GetNumPackages(type);
            int perf_unlock_level = remaining + i + 1;
            if (IsPartLocked(sp, perf_unlock_level)) {
                state = CPS_LOCKED;
            } else if (IsPartNew(sp, perf_unlock_level)) {
                state = CPS_NEW;
            }
            if (IsPartInstalled(sp)) {
                state = static_cast<eCustomizePartState>(state | CPS_INSTALLED);
            } else if (IsPartInCart(sp)) {
                state = static_cast<eCustomizePartState>(state | CPS_IN_CART);
            }
            sp->SetPartState(state);
            int price = GetPartPrice(sp);
            sp->SetPrice(price);
            the_list.AddTail(sp);
            i = level;
        } while (i < max_level);
    }
}

bool CarCustomizeManager::CanInstallJunkman(Physics::Upgrades::Type type) {
    return Physics::Upgrades::CanInstallJunkman(ThePVehicle, type);
}

bool CarCustomizeManager::IsCareerMode() {
    return FEDatabase->IsCareerMode() || g_bTestCareerCustomization;
}

bool CarCustomizeManager::IsTurbo() {
    Attrib::Gen::pvehicle pveh(TuningCar->VehicleKey, 0, nullptr);
    Physics::Upgrades::SetLevel(pveh, Physics::Upgrades::PUT_INDUCTION, 1);
    return Physics::Info::InductionType(pveh) == 1;
}

float CarCustomizeManager::GetActualHeat() {
    if (!TuningCar)
        return 0.0f;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECareerRecord *career = stable->GetCareerRecordByHandle(TuningCar->CareerHandle);
    if (!career)
        return 0.0f;
    return career->GetVehicleHeat();
}

float CarCustomizeManager::GetPreviewHeat(SelectablePart *part) {
    if (!DoesCartHaveActiveParts() || !IsCareerMode()) {
        return GetActualHeat();
    }
    FECareerRecord temp_record;
    FECareerRecord *career_record = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(TuningCar->CareerHandle);
    if (!career_record)
        return 0.0f;
    float heat = career_record->GetVehicleHeat();
    temp_record.SetVehicleHeat(heat);
    if (part && part->GetPart() != GetInstalledCarPart(part->GetSlotID())) {
        UpdateHeatOnVehicle(part, &temp_record);
    }
    ShoppingCartItem *item = ShoppingCart.GetHead();
    while (item != ShoppingCart.EndOfList()) {
        if (!part) {
            if (item->IsActive()) {
                UpdateHeatOnVehicle(item->GetBuyingPart(), &temp_record);
            }
        } else if (part->GetSlotID() != item->GetBuyingPart()->GetSlotID() && item->IsActive()) {
            UpdateHeatOnVehicle(item->GetBuyingPart(), &temp_record);
        }
        item = static_cast<ShoppingCartItem *>(item->GetNext());
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
    if (TuningCar->GetType() == 0x34) {
        return gEasterEggs.IsEasterEggUnlocked(EASTER_EGG_CASTROL);
    }
    return false;
}

bool CarCustomizeManager::IsRotaryCar() {
    int type = TuningCar->GetType();
    if (type == 0x05 || type == 0x38) {
        return true;
    }
    return false;
}

bool CarCustomizeManager::IsHeroCar() {
    return TuningCar->GetType() == 0x29;
}

float CarCustomizeManager::GetCartHeat() {
    if (DoesCartHaveActiveParts()) {
        if (IsCareerMode()) {
            FECareerRecord temp_record;
            FECareerRecord *career_record = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(TuningCar->CareerHandle);
            if (!career_record)
                return 0.0f;
            float heat = career_record->GetVehicleHeat();
            temp_record.SetVehicleHeat(heat);
            ShoppingCartItem *item = ShoppingCart.GetHead();
            while (item != ShoppingCart.EndOfList()) {
                if (item->IsActive()) {
                    UpdateHeatOnVehicle(item->GetBuyingPart(), &temp_record);
                }
                item = static_cast<ShoppingCartItem *>(item->GetNext());
            }
            return temp_record.GetVehicleHeat();
        }
    }
    return GetActualHeat();
}
