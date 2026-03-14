// OWNED BY zFeOverlay AGENT
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"

namespace Physics {
namespace Upgrades {
    bool CanInstallJunkman(const Attrib::Gen::pvehicle &pvehicle, Type type);
    void SetLevel(Attrib::Gen::pvehicle &pvehicle, Type type, int level);
}
}

extern CarTypeInfo *GetCarTypeInfoFromHash(unsigned int hash);

extern FEMarkerManager TheFEMarkerManager;

extern int g_bTestCareerCustomization;
extern int g_bCustomizeManagerHasControl;
extern SelectablePart *_8Showcase_FromColor;
extern float gTradeInFactor;
extern int CustomizeIsInBackRoom();
extern CarPart *GetCarPart(RideInfo *ride, unsigned int slot_id);

struct CarPartAttribute;
struct CarPart {
    unsigned short PartNameHashBot;         // offset 0x0, size 0x2
    unsigned short PartNameHashTop;         // offset 0x2, size 0x2
    char PartID;                            // offset 0x4, size 0x1
    unsigned char GroupNumber_UpgradeLevel;  // offset 0x5, size 0x1
    char BaseModelNameHashSelector;         // offset 0x6, size 0x1
    unsigned char CarTypeNameHashIndex;     // offset 0x7, size 0x1
    unsigned short NameOffset;              // offset 0x8, size 0x2
    unsigned short AttributeTableOffset;    // offset 0xA, size 0x2
    unsigned short ModelNameHashTableOffset; // offset 0xC, size 0x2

    const char *GetName();
    unsigned int GetCarTypeNameHash();
    unsigned int GetPartNameHash();
    char GetPartID();
    char GetUpgradeLevel();
};

int CarCustomizeManager::GetNumPackages(Physics::Upgrades::Type type) {
    return Physics::Upgrades::GetMaxLevel(ThePVehicle, type);
}

bool CarCustomizeManager::CanInstallJunkman(Physics::Upgrades::Type type) {
    return Physics::Upgrades::CanInstallJunkman(ThePVehicle, type);
}

bool CarCustomizeManager::IsCareerMode() {
    return FEDatabase->IsCareerMode() || g_bTestCareerCustomization;
}

bool CarCustomizeManager::IsHeroCar() {
    return TuningCar->GetType() == 0x29;
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

int CarCustomizeManager::GetMaxPackages(Physics::Upgrades::Type type) {
    switch (type) {
        case 0: return 3;
        case 1: return 4;
        case 2: return 3;
        case 3: return 4;
        case 4: return 4;
        case 5: return 3;
        case 6: return 3;
        default: return -1;
    }
}

int CarCustomizeManager::GetInstalledPerfPkg(Physics::Upgrades::Type type) {
    FEPlayerCarDB *db = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *record = db->GetCustomizationRecordByHandle(TuningCar->Customization);
    const Physics::Upgrades::Package *pkg = &record->InstalledPhysics;
    return pkg->Part[type];
}

void CarCustomizeManager::TakeControl(eCustomizeEntryPoint entry_point, FECarRecord *tuning_car) {
    if (!g_bCustomizeManagerHasControl) {
        FEDatabase->SetGameMode(eFE_GAME_MODE_CUSTOMIZE);
        g_bCustomizeManagerHasControl = 1;
        for (int i = 0; i < 3; i++) {
            (&_8Showcase_FromColor)[i] = nullptr;
        }
        NumPartsInCart = 0;
        EntryPoint = entry_point;
        TuningCar = tuning_car;
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        FECustomizationRecord *src = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
        PreviewRecord = *src;
        Attrib::Gen::pvehicle pveh(TuningCar->VehicleKey, 0, nullptr);
        stable->WriteRecordIntoPhysics(TuningCar->Handle, pveh);
        ThePVehicle = pveh;
        RideInfo ride;
        ride.Init(static_cast<CarType>(-1), static_cast<CarRenderUsage>(0), 0, 0);
        stable->BuildRideForPlayer(TuningCar->Handle, 0, &ride);
        CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
        TheTempColoredPart = nullptr;
    }
}

void CarCustomizeManager::RelinquishControl() {
    FEDatabase->ClearGameMode(eFE_GAME_MODE_CUSTOMIZE);
    for (int i = 0; i < 3; i++) {
        delete (&_8Showcase_FromColor)[i];
        (&_8Showcase_FromColor)[i] = nullptr;
    }
    ClearTempColoredPart();
    g_bCustomizeManagerHasControl = 0;
}

bool CarCustomizeManager::CanTradeIn(SelectablePart *part) {
    if (!part->IsPerformancePkg()) {
        int slot = part->GetSlotID();
        if (slot < 0x74) {
            if (slot < 99 && (slot < 0x4c || (slot > 0x53 && slot != 0x5b))) {
                return true;
            }
        } else if (slot != 0x7b) {
            if (slot < 0x7b) {
                return true;
            }
            if (slot > 0x87) {
                return true;
            }
            if (slot < 0x83) {
                return true;
            }
        }
    }
    return false;
}

void CarCustomizeManager::AddToCart(SelectablePart *part) {
    ShoppingCartItem *existing = IsPartTypeInCart(part);
    SelectablePart *trade_in = nullptr;
    if (!existing) {
        if (!part->IsPerformancePkg()) {
            if (CanTradeIn(part)) {
                CarPart *installed = GetInstalledCarPart(part->GetSlotID());
                if (installed) {
                    trade_in = new SelectablePart(installed, part->GetSlotID(),
                        installed->GetUpgradeLevel(), static_cast<GRace::Type>(7), false,
                        CPS_INSTALLED, 0, false);
                    trade_in->SetPrice(GetPartPrice(trade_in));
                }
            }
        }
    } else {
        if (CanTradeIn(part) && existing->GetTradeInPart()) {
            SelectablePart *old_trade = existing->GetTradeInPart();
            trade_in = new SelectablePart(old_trade);
        }
        RemoveFromCart(existing);
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
    }
    return item != nullptr;
}

ShoppingCartItem *CarCustomizeManager::IsPartTypeInCart(SelectablePart *to_find) {
    for (ShoppingCartItem *item = GetFirstCartItem(); item != reinterpret_cast<ShoppingCartItem *>(&ShoppingCart); item = item->GetNext()) {
        if (item->GetBuyingPart()->GetSlotID() == to_find->GetSlotID()) {
            return item;
        }
    }
    return nullptr;
}

ShoppingCartItem *CarCustomizeManager::IsPartTypeInCart(unsigned int slot_id) {
    for (ShoppingCartItem *item = GetFirstCartItem(); item != reinterpret_cast<ShoppingCartItem *>(&ShoppingCart); item = item->GetNext()) {
        if (item->GetBuyingPart()->GetSlotID() == static_cast<int>(slot_id)) {
            return item;
        }
    }
    return nullptr;
}

ShoppingCartItem *CarCustomizeManager::IsPartTypeInCart(GRace::Type type) {
    for (ShoppingCartItem *item = GetFirstCartItem(); item != reinterpret_cast<ShoppingCartItem *>(&ShoppingCart); item = item->GetNext()) {
        if (item->GetBuyingPart()->GetPhysicsType() == type) {
            return item;
        }
    }
    return nullptr;
}

ShoppingCartItem *CarCustomizeManager::IsPartInCart(SelectablePart *to_find) {
    for (ShoppingCartItem *item = GetFirstCartItem(); item != reinterpret_cast<ShoppingCartItem *>(&ShoppingCart); item = item->GetNext()) {
        if (item->GetBuyingPart()->GetPart() == to_find->GetPart() &&
            item->GetBuyingPart()->GetSlotID() == to_find->GetSlotID()) {
            return item;
        }
    }
    return nullptr;
}

CarPart *CarCustomizeManager::GetActivePartFromSlot(unsigned int slot_id) {
    ShoppingCartItem *item = IsPartTypeInCart(slot_id);
    if (!item) {
        return GetInstalledCarPart(slot_id);
    }
    return item->GetBuyingPart()->GetPart();
}

int CarCustomizeManager::GetCartTotal(eCustomizeCartTotals type) {
    int total = 0;
    for (ShoppingCartItem *item = GetFirstCartItem(); item != reinterpret_cast<ShoppingCartItem *>(&ShoppingCart); item = item->GetNext()) {
        if (!item->IsActive()) continue;
        if (type == 0 || type == 2) {
            SelectablePart *buy = item->GetBuyingPart();
            if (!buy->IsPerformancePkg()) {
                int slot = buy->GetSlotID();
                if (slot == 0x72) continue;
                if ((slot >= 0x4f && slot <= 0x52) || (slot >= 0x85 && slot <= 0x87)) continue;
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
        if (!item->IsActive()) continue;
        SelectablePart *buy = item->GetBuyingPart();
        if (IsCareerMode() && buy->GetSlotID() != 0x72) {
            if (!CustomizeIsInBackRoom()) {
                int trade_val = 0;
                if (item->GetTradeInPart()) {
                    trade_val = static_cast<int>(static_cast<float>(item->GetTradeInPart()->GetPrice()) * gTradeInFactor);
                }
                int cost = buy->GetPrice() - trade_val;
                if (cost < 0) {
                    FEDatabase->GetCareerSettings()->AddCash(-cost);
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
    for (ShoppingCartItem *item = GetFirstCartItem(); item != reinterpret_cast<ShoppingCartItem *>(&ShoppingCart); item = item->GetNext()) {
        SelectablePart *buy = item->GetBuyingPart();
        if (buy && !buy->IsPerformancePkg()) {
            int slot = buy->GetSlotID();
            if ((slot >= 0x4f && slot <= 0x52) || (slot >= 0x85 && slot <= 0x87)) continue;
        }
        if (item->IsActive()) return true;
    }
    return false;
}

int CarCustomizeManager::GetPartPrice(SelectablePart *part) {
    if (!part || CustomizeIsInBackRoom()) return 0;
    if (!part->IsPerformancePkg()) {
        int slot = part->GetSlotID();
        if ((slot >= 0x4f && slot <= 0x52) || (slot >= 0x85 && slot <= 0x87)) return 0;
        eUnlockFilters filter = GetUnlockFilter();
        return UnlockSystem::GetCarPartCost(filter, slot, part->GetPart(), 0);
    } else {
        Physics::Upgrades::Type ptype = static_cast<Physics::Upgrades::Type>(static_cast<int>(part->GetPhysicsType()));
        int max_pkgs = GetMaxPackages(ptype);
        int num_pkgs = GetNumPackages(ptype);
        int level = part->GetUpgradeLevel();
        eUnlockFilters filter = GetUnlockFilter();
        return UnlockSystem::GetPerfPackageCost(filter, ptype, max_pkgs - (num_pkgs - level), 0);
    }
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
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    RideInfo ride;
    ride.Init(static_cast<CarType>(-1), static_cast<CarRenderUsage>(0), 0, 0);
    stable->BuildRideForPlayer(TuningCar->Handle, 0, &ride);
    ride.SetStockParts();
    return ride.GetPart(slot_id);
}

void CarCustomizeManager::ResetToStockCarParts() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    RideInfo ride;
    ride.Init(static_cast<CarType>(-1), static_cast<CarRenderUsage>(0), 0, 0);
    stable->BuildRideForPlayer(TuningCar->Handle, 0, &ride);
    ride.SetStockParts();
    PreviewRecord.WriteRideIntoRecord(&ride);
    CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
}

void CarCustomizeManager::ResetPreview() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *src = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    PreviewRecord = *src;
    RideInfo ride;
    ride.Init(static_cast<CarType>(-1), static_cast<CarRenderUsage>(0), 0, 0);
    stable->BuildRideForPlayer(TuningCar->Handle, 0, &ride);
    PreviewRecord.WriteRecordIntoRide(&ride);
    CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_LOAD_CAR, eCARVIEWER_PLAYER1_CAR);
    for (ShoppingCartItem *item = GetFirstCartItem(); item != reinterpret_cast<ShoppingCartItem *>(&ShoppingCart); item = item->GetNext()) {
        SelectablePart *buy = item->GetBuyingPart();
        if (!buy->IsPerformancePkg()) {
            PreviewPart(buy->GetSlotID(), buy->GetPart());
        } else {
            PreviewPerfPkg(static_cast<Physics::Upgrades::Type>(static_cast<int>(buy->GetPhysicsType())), buy->GetUpgradeLevel());
        }
    }
}

void CarCustomizeManager::PreviewPart(int slot_id, CarPart *part) {
    PreviewRecord.SetInstalledPart(slot_id, part);
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    RideInfo ride;
    ride.Init(static_cast<CarType>(-1), static_cast<CarRenderUsage>(0), 0, 0);
    stable->BuildRideForPlayer(TuningCar->Handle, 0, &ride);
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
        PreviewRecord.InstalledPhysics.Junkman |= (1 << part_type);
        PreviewRecord.WriteRecordIntoPhysics(ThePVehicle);
    } else {
        PreviewRecord.InstalledPhysics.Part[part_type] = level;
        PreviewRecord.WriteRecordIntoPhysics(ThePVehicle);
    }
}

void CarCustomizeManager::InstallPerfPkg(Physics::Upgrades::Type part_type, int level) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    if (level == 7) {
        unsigned int mask = 1 << part_type;
        record->InstalledPhysics.Junkman |= mask;
        if (!record->WriteRecordIntoPhysics(ThePVehicle)) {
            record->InstalledPhysics.Junkman &= ~mask;
            record->WriteRecordIntoPhysics(ThePVehicle);
        }
    } else {
        record->InstalledPhysics.Part[part_type] = level;
        if (!record->WriteRecordIntoPhysics(ThePVehicle)) {
            record->InstalledPhysics.Part[part_type] = 0;
            record->WriteRecordIntoPhysics(ThePVehicle);
        }
    }
    PreviewPerfPkg(part_type, level);
}

bool CarCustomizeManager::IsJunkmanInstalled(Physics::Upgrades::Type type) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECustomizationRecord *record = stable->GetCustomizationRecordByHandle(TuningCar->Customization);
    return (record->InstalledPhysics.Junkman & (1 << type)) != 0;
}

eUnlockFilters CarCustomizeManager::GetUnlockFilter() {
    if (FEDatabase->IsCareerMode()) {
        if (CustomizeIsInBackRoom()) {
            return static_cast<eUnlockFilters>(10);
        }
        return static_cast<eUnlockFilters>(2);
    }
    if ((FEDatabase->GetGameMode() & 0x28) == 0x28) {
        return static_cast<eUnlockFilters>(4);
    }
    return static_cast<eUnlockFilters>(1);
}

bool CarCustomizeManager::IsTurbo() {
    Attrib::Gen::pvehicle pveh(TuningCar->VehicleKey, 0, nullptr);
    Physics::Upgrades::SetLevel(pveh, static_cast<Physics::Upgrades::Type>(5), 1);
    return Physics::Info::InductionType(pveh) == 1;
}

float CarCustomizeManager::GetActualHeat() {
    if (!TuningCar) return 0.0f;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECareerRecord *career = stable->GetCareerRecordByHandle(TuningCar->CareerHandle);
    if (!career) return 0.0f;
    return career->GetVehicleHeat();
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
        case 0: return perf.TopSpeed;
        case 1: return perf.Handling;
        case 2: return perf.Acceleration;
        default: return perf.Handling;
    }
}
