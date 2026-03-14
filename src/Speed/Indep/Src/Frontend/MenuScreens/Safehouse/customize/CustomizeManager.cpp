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
    char GetGroupNumber() { return GroupNumber_UpgradeLevel & 0x1f; }
    int HasAppliedAttribute(unsigned int namehash);
    const char *GetAppliedAttributeString(unsigned int namehash, const char *default_string);
    float GetAppliedAttributeFParam(unsigned int namehash, float default_value);
    int GetAppliedAttributeIParam(unsigned int namehash, int default_value);
    unsigned int GetAppliedAttributeUParam(unsigned int namehash, unsigned int default_value);
    unsigned int GetBrandNameHash();
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

ShoppingCartItem *CarCustomizeManager::IsPartTypeInCart(Physics::Upgrades::Type type) {
    SelectablePart test_part(nullptr, 0, 0, static_cast<GRace::Type>(static_cast<int>(type)), true, CPS_AVAILABLE, 0, false);
    return IsPartTypeInCart(&test_part);
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
            if (slot >= 0x4f) {
                if (slot <= 0x52) continue;
                if (slot <= 0x87) {
                    if (slot >= 0x85) continue;
                }
            }
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

extern eUnlockableEntity MapCarPartToUnlockable(int slot_id, CarPart *part);
extern eUnlockableEntity MapPerfPkgToUnlockable(Physics::Upgrades::Type type);
extern unsigned int FEngHashString(const char *fmt, ...);
extern bool DoesStringExist(unsigned int hash);
extern int FEngSNPrintf(char *buf, int size, const char *fmt, ...);
extern void bMemSet(void *dst, int value, unsigned int size);

struct CarPartDatabase {
    CarType GetCarType(unsigned int model_hash);
    CarPart *GetCarPartByIndex(int index);
    int GetPartIndex(CarPart *part);
    CarPart *NewGetCarPart(CarType cartype, int slot, unsigned int part_name_hash, CarPart *fallback, int index);
    CarPart *NewGetFirstCarPart(CarType car_type, int car_slot_id, unsigned int car_part_namehash, int upg_level);
    CarPart *NewGetNextCarPart(CarPart *car_part, CarType car_type, int car_slot_id, unsigned int car_part_namehash, int upg_level);
};
extern CarPartDatabase CarPartDB;

namespace Physics {
namespace Upgrades {
extern float GetHeat(Attrib::Gen::pvehicle pvehicle, Type type, int level);
}
}

bool CarCustomizeManager::IsPartInstalled(SelectablePart *part) {
    if (part) {
        if (!part->IsPerformancePkg()) {
            CarPart *installed = GetInstalledCarPart(part->GetSlotID());
            if (installed == part->GetPart()) {
                return true;
            }
        } else {
            if (part->IsJunkmanPart()) {
                return IsJunkmanInstalled(static_cast<Physics::Upgrades::Type>(static_cast<int>(part->GetPhysicsType())));
            }
            int lvl = GetInstalledPerfPkg(static_cast<Physics::Upgrades::Type>(static_cast<int>(part->GetPhysicsType())));
            if (static_cast<int>(part->GetUpgradeLevel()) == lvl) {
                return true;
            }
        }
    }
    return false;
}

bool CarCustomizeManager::IsPartLocked(SelectablePart *part, int perf_unlock_level) {
    bool unlocked;
    if (part->IsPerformancePkg()) {
        eUnlockFilters filter = GetUnlockFilter();
        bool backroom = CustomizeIsInBackRoom();
        unlocked = UnlockSystem::IsPerfPackageUnlocked(filter, static_cast<Physics::Upgrades::Type>(static_cast<int>(part->GetPhysicsType())), perf_unlock_level, 0, backroom);
    } else {
        int slot = part->GetSlotID();
        if (slot < 0x69) {
            if (slot > 0x62) {
                eUnlockFilters filter = GetUnlockFilter();
                bool backroom = CustomizeIsInBackRoom();
                unlocked = UnlockSystem::IsUnlockableUnlocked(filter, static_cast<eUnlockableEntity>(0x2e), 2, 0, backroom);
            } else if (slot == 0x53 || slot == 0x5b) {
                eUnlockFilters filter = GetUnlockFilter();
                bool backroom = CustomizeIsInBackRoom();
                unlocked = UnlockSystem::IsUnlockableUnlocked(filter, static_cast<eUnlockableEntity>(0x2c), 1, 0, backroom);
            } else {
                eUnlockFilters filter = GetUnlockFilter();
                bool backroom = CustomizeIsInBackRoom();
                unlocked = UnlockSystem::IsCarPartUnlocked(filter, part->GetSlotID(), part->GetPart(), 0, backroom);
            }
        } else if (slot > 0x6a) {
            if (slot < 0x71) {
                eUnlockFilters filter = GetUnlockFilter();
                bool backroom = CustomizeIsInBackRoom();
                unlocked = UnlockSystem::IsUnlockableUnlocked(filter, static_cast<eUnlockableEntity>(0x2e), 2, 0, backroom);
            } else if (slot == 0x73 || slot == 0x7b) {
                eUnlockFilters filter = GetUnlockFilter();
                bool backroom = CustomizeIsInBackRoom();
                unlocked = UnlockSystem::IsUnlockableUnlocked(filter, static_cast<eUnlockableEntity>(0x30), 3, 0, backroom);
            } else {
                eUnlockFilters filter = GetUnlockFilter();
                bool backroom = CustomizeIsInBackRoom();
                unlocked = UnlockSystem::IsCarPartUnlocked(filter, part->GetSlotID(), part->GetPart(), 0, backroom);
            }
        } else {
            eUnlockFilters filter = GetUnlockFilter();
            bool backroom = CustomizeIsInBackRoom();
            unlocked = UnlockSystem::IsCarPartUnlocked(filter, part->GetSlotID(), part->GetPart(), 0, backroom);
        }
    }
    return unlocked ^ true;
}

bool CarCustomizeManager::IsPartNew(SelectablePart *part, int perf_unlock_level) {
    eUnlockableEntity ent;
    eUnlockFilters filter;
    if (!part->IsPerformancePkg()) {
        ent = MapCarPartToUnlockable(part->GetSlotID(), part->GetPart());
        filter = GetUnlockFilter();
        perf_unlock_level = static_cast<int>(part->GetUpgradeLevel());
    } else {
        ent = MapPerfPkgToUnlockable(static_cast<Physics::Upgrades::Type>(static_cast<int>(part->GetPhysicsType())));
        filter = GetUnlockFilter();
    }
    return UnlockSystem::IsUnlockableNew(filter, ent, perf_unlock_level);
}

bool CarCustomizeManager::IsCategoryNew(unsigned int cat) {
    eUnlockableEntity titty;

    if (cat == 0x406) {
        titty = static_cast<eUnlockableEntity>(0x27);
    } else if (cat < 0x407) {
        if (cat == 0x207) {
            titty = static_cast<eUnlockableEntity>(9);
        } else if (cat < 0x208) {
            if (cat == 0x201) {
                titty = static_cast<eUnlockableEntity>(8);
            } else if (cat < 0x202) {
                if (cat == 0x103) {
                    for (unsigned int i = 0x702; i < 0x70c; i++) {
                        if (IsCategoryNew(i)) return true;
                    }
                    return false;
                }
                if (cat < 0x104) {
                    if (cat == 0x101) {
                        titty = static_cast<eUnlockableEntity>(0xb);
                    } else {
                        if (cat != 0x102) return true;
                        titty = static_cast<eUnlockableEntity>(0xc);
                    }
                } else if (cat == 0x104) {
                    titty = static_cast<eUnlockableEntity>(0xe);
                } else {
                    if (cat != 0x105) return true;
                    titty = static_cast<eUnlockableEntity>(0xf);
                }
            } else if (cat == 0x204) {
                titty = static_cast<eUnlockableEntity>(10);
            } else if (cat < 0x205) {
                if (cat == 0x202) {
                    titty = static_cast<eUnlockableEntity>(7);
                } else {
                    if (cat != 0x203) return true;
                    titty = static_cast<eUnlockableEntity>(6);
                }
            } else if (cat == 0x205) {
                titty = static_cast<eUnlockableEntity>(4);
            } else {
                if (cat != 0x206) return true;
                titty = static_cast<eUnlockableEntity>(5);
            }
        } else if (cat == 0x306) {
            titty = static_cast<eUnlockableEntity>(0x2b);
        } else if (cat < 0x307) {
            if (cat == 0x303) {
                titty = static_cast<eUnlockableEntity>(0x18);
            } else if (cat < 0x304) {
                if (cat != 0x301) {
                    if (cat == 0x302) {
                        for (unsigned int i = 0x402; i <= 0x409; i++) {
                            if (IsCategoryNew(i)) return true;
                        }
                    }
                    return true;
                }
                titty = static_cast<eUnlockableEntity>(0x17);
            } else {
                if (cat != 0x304) {
                    if (cat != 0x305) return true;
                    if (IsCategoryNew(0x501)) return true;
                    if (IsCategoryNew(0x505)) return true;
                    if (IsCategoryNew(0x503)) return true;
                    return false;
                }
                titty = static_cast<eUnlockableEntity>(0x12);
            }
        } else if (cat == 0x403) {
            titty = static_cast<eUnlockableEntity>(0x24);
        } else if (cat < 0x404) {
            if (cat == 0x307) {
                titty = static_cast<eUnlockableEntity>(0x11);
            } else {
                if (cat != 0x402) return true;
                titty = static_cast<eUnlockableEntity>(0x23);
            }
        } else if (cat == 0x404) {
            titty = static_cast<eUnlockableEntity>(0x25);
        } else {
            if (cat != 0x405) return true;
            titty = static_cast<eUnlockableEntity>(0x26);
        }
    } else if (cat == 0x702) {
        titty = static_cast<eUnlockableEntity>(0x19);
    } else if (cat < 0x703) {
        if (cat < 0x505) {
            if (cat < 0x503) {
                if (cat == 0x409) {
                    titty = static_cast<eUnlockableEntity>(0x2a);
                } else if (cat < 0x40a) {
                    if (cat == 0x407) {
                        titty = static_cast<eUnlockableEntity>(0x28);
                    } else {
                        if (cat != 0x408) return true;
                        titty = static_cast<eUnlockableEntity>(0x29);
                    }
                } else {
                    if (cat < 0x501) return true;
                    titty = static_cast<eUnlockableEntity>(0x2c);
                }
                goto common;
            }
        } else {
            if (cat < 0x507) {
                titty = static_cast<eUnlockableEntity>(0x30);
                goto common;
            }
            if (cat > 0x606) return true;
            if (cat < 0x601) return true;
        }
        titty = static_cast<eUnlockableEntity>(0x2e);
    } else if (cat == 0x708) {
        titty = static_cast<eUnlockableEntity>(0x1f);
    } else if (cat < 0x709) {
        if (cat == 0x705) {
            titty = static_cast<eUnlockableEntity>(0x1c);
        } else if (cat < 0x706) {
            if (cat == 0x703) {
                titty = static_cast<eUnlockableEntity>(0x1a);
            } else {
                if (cat != 0x704) return true;
                titty = static_cast<eUnlockableEntity>(0x1b);
            }
        } else if (cat == 0x706) {
            titty = static_cast<eUnlockableEntity>(0x1d);
        } else {
            if (cat != 0x707) return true;
            titty = static_cast<eUnlockableEntity>(0x1e);
        }
    } else if (cat == 0x70b) {
        titty = static_cast<eUnlockableEntity>(0x22);
    } else {
        if (cat > 0x70b) {
            if (cat == 0x802) {
                for (unsigned int i = 0x201; i < 0x208; i++) {
                    if (IsCategoryNew(i)) return true;
                }
                return false;
            }
            if (cat < 0x803) {
                if (cat != 0x801) return true;
                for (unsigned int i = 0x101; i < 0x106; i++) {
                    if (IsCategoryNew(i)) return true;
                }
                return false;
            }
            if (cat != 0x803) return true;
            for (unsigned int i = 0x301; i < 0x308; i++) {
                if (IsCategoryNew(i)) return true;
            }
            return false;
        }
        if (cat == 0x709) {
            titty = static_cast<eUnlockableEntity>(0x20);
        } else {
            if (cat != 0x70a) return true;
            titty = static_cast<eUnlockableEntity>(0x21);
        }
    }
common:
    eUnlockFilters filter = GetUnlockFilter();
    return UnlockSystem::IsUnlockableNew(filter, titty, -2);
}

bool CarCustomizeManager::IsCategoryLocked(unsigned int cat, bool backroom) {
    eUnlockableEntity titty;
    int level = 0;

    if (cat == 0x305) {
        if (!IsCategoryLocked(0x501, backroom)) return false;
        if (!IsCategoryLocked(0x505, backroom)) return false;
        if (!IsCategoryLocked(0x503, backroom)) return false;
        return true;
    }
    if (cat < 0x306) {
        if (cat == 0x203) {
            if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(2))) return true;
            titty = static_cast<eUnlockableEntity>(6);
        } else if (cat < 0x204) {
            if (cat == 0x104) {
                titty = static_cast<eUnlockableEntity>(0xe);
            } else if (cat < 0x105) {
                if (cat == 0x102) {
                    titty = static_cast<eUnlockableEntity>(0xc);
                } else if (cat > 0x102) {
                    for (unsigned int i = 0x702; i < 0x70c; i++) {
                        if (!IsRimCategoryLocked(i, backroom)) return false;
                    }
                    return true;
                } else if (cat == 0x101) {
                    titty = static_cast<eUnlockableEntity>(0xb);
                } else {
                    return true;
                }
            } else if (cat == 0x201) {
                if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(4))) return true;
                titty = static_cast<eUnlockableEntity>(8);
            } else if (cat < 0x202) {
                if (cat != 0x105) return true;
                titty = static_cast<eUnlockableEntity>(0xf);
            } else {
                if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(3))) return true;
                titty = static_cast<eUnlockableEntity>(7);
            }
        } else if (cat == 0x207) {
            if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(5))) return true;
            titty = static_cast<eUnlockableEntity>(9);
        } else if (cat < 0x208) {
            if (cat == 0x205) {
                if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(0))) return true;
                titty = static_cast<eUnlockableEntity>(4);
            } else if (cat < 0x206) {
                if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(6))) return true;
                titty = static_cast<eUnlockableEntity>(10);
            } else {
                if (backroom && !CanInstallJunkman(static_cast<Physics::Upgrades::Type>(1))) return true;
                titty = static_cast<eUnlockableEntity>(5);
            }
        } else {
            if (cat == 0x302) {
                for (unsigned int i = 0x402; i < 0x40a; i++) {
                    if (!IsVinylCategoryLocked(i, backroom)) return false;
                }
                return true;
            }
            if (cat < 0x303) {
                if (cat != 0x301) return true;
                titty = static_cast<eUnlockableEntity>(0x17);
            } else if (cat == 0x303) {
                titty = static_cast<eUnlockableEntity>(0x18);
            } else {
                if (cat != 0x304) return true;
                titty = static_cast<eUnlockableEntity>(0x12);
            }
        }
    } else if (cat < 0x507) {
        if (cat > 0x504) {
            level = 3;
            titty = static_cast<eUnlockableEntity>(0x30);
        } else if (cat < 0x40a) {
            if (cat > 0x401) {
                return IsVinylCategoryLocked(cat, backroom);
            }
            if (cat == 0x306) {
                titty = static_cast<eUnlockableEntity>(0x2b);
            } else {
                if (cat != 0x307) return true;
                titty = static_cast<eUnlockableEntity>(0x11);
            }
        } else if (cat < 0x501) {
            return true;
        } else if (cat < 0x503) {
            level = 1;
            titty = static_cast<eUnlockableEntity>(0x2c);
        } else {
            level = 2;
            titty = static_cast<eUnlockableEntity>(0x2e);
        }
    } else {
        if (cat > 0x70b) {
            if (cat == 0x802) {
                for (unsigned int i = 0x201; i < 0x208; i++) {
                    if (!IsCategoryLocked(i, backroom)) return false;
                }
                return true;
            }
            if (cat < 0x803) {
                if (cat != 0x801) return true;
                for (unsigned int i = 0x101; i < 0x106; i++) {
                    if (!IsCategoryLocked(i, backroom)) return false;
                }
                return true;
            }
            if (cat != 0x803) return true;
            for (unsigned int i = 0x301; i < 0x308; i++) {
                if (!IsCategoryLocked(i, backroom)) return false;
            }
            return true;
        }
        if (cat > 0x701) {
            return IsRimCategoryLocked(cat, backroom);
        }
        if (cat > 0x606) return true;
        if (cat < 0x601) return true;
        level = 2;
        titty = static_cast<eUnlockableEntity>(0x2e);
    }

    eUnlockFilters filter = GetUnlockFilter();
    if (!backroom) {
        return !UnlockSystem::IsUnlockableUnlocked(filter, titty, level, 0, false);
    } else {
        return !UnlockSystem::IsBackroomAvailable(filter, titty, level);
    }
}

bool CarCustomizeManager::IsRimCategoryLocked(unsigned int cat, bool backroom) {
    int marker_param = -1;
    if (cat == 0x703) marker_param = 3;
    else if (cat == 0x704) marker_param = 4;
    else if (cat == 0x705) marker_param = 5;
    else if (cat == 0x706) marker_param = 6;
    else if (cat == 0x707) marker_param = 7;
    if (marker_param == -1) return false;
    bTList<SelectablePart> parts;
    GetCarPartList(0x35, parts, 0);
    SelectablePart *sp = parts.GetHead();
    while (sp != parts.EndOfList()) {
        unsigned int brand = sp->GetPart()->GetAppliedAttributeUParam(0xebb03e66, 0u);
        if ((brand >> 5) == cat) {
            if (backroom) {
                eUnlockFilters filter = GetUnlockFilter();
                if (UnlockSystem::IsCarPartUnlocked(filter, sp->GetSlotID(), sp->GetPart(), 0, true)) {
                    return false;
                }
            } else {
                return false;
            }
        }
        sp = static_cast<SelectablePart *>(sp->GetNext());
    }
    if (!backroom) return true;
    return !TheFEMarkerManager.HasMarker(FEMarkerManager::MARKER_RIMS, marker_param);
}

bool CarCustomizeManager::IsVinylCategoryLocked(unsigned int cat, bool backroom) {
    unsigned int vinyl_group = cat;
    int marker_param = -1;
    if (vinyl_group == 1) marker_param = 1;
    else if (vinyl_group == 2) marker_param = 2;
    if (marker_param == -1) return false;
    bTList<SelectablePart> parts;
    GetCarPartList(0x28, parts, 0);
    SelectablePart *sp = parts.GetHead();
    while (sp != parts.EndOfList()) {
        if ((sp->GetPart()->GetAppliedAttributeUParam(0xebb03e66, 0u) & 0x1f) == vinyl_group
            && sp->GetPartState() != CPS_LOCKED) {
            return false;
        }
        sp = static_cast<SelectablePart *>(sp->GetNext());
    }
    if (!backroom) return true;
    return !TheFEMarkerManager.HasMarker(FEMarkerManager::MARKER_VINYL, marker_param);
}

void CarCustomizeManager::UpdateHeatOnVehicle(SelectablePart *part, FECareerRecord *record) {
    if (!part) return;
    if (part->IsPerformancePkg()) {
        Physics::Upgrades::Type ptype = static_cast<Physics::Upgrades::Type>(static_cast<int>(part->GetPhysicsType()));
        int level = static_cast<int>(part->GetUpgradeLevel());
        if (part->IsJunkmanPart()) {
            level = GetMaxPackages(ptype) + 1;
        }
        int installed = GetInstalledPerfPkg(ptype);
        if (level > installed) {
            record->SetVehicleHeat(record->GetVehicleHeat() + Physics::Upgrades::GetHeat(ThePVehicle, ptype, level));
        }
    } else {
        int slot = part->GetSlotID();
        CarPart *installed = GetInstalledCarPart(slot);
        CarPart *buying = part->GetPart();
        if (buying && buying != installed) {
            record->SetVehicleHeat(record->GetVehicleHeat() + 2.0f);
        }
    }
}

unsigned int CarCustomizeManager::GetUnlockHash(eCustomizeCategory cat, int upgrade_lvl) {
    unsigned int returnHash = 0;
    switch (cat) {
        case 0x100: returnHash = FEngHashString("MARKER_BODYKIT"); break;
        case 0x101: returnHash = FEngHashString("MARKER_SPOILER"); break;
        case 0x103: returnHash = FEngHashString("MARKER_RIM"); break;
        case 0x104: returnHash = FEngHashString("MARKER_HOOD"); break;
        case 0x105: returnHash = FEngHashString("MARKER_ROOFSCOOP"); break;
        case 0x201: returnHash = FEngHashString("MARKER_PAINT"); break;
        case 0x202: returnHash = FEngHashString("MARKER_VINYL"); break;
        case 0x203: returnHash = FEngHashString("MARKER_DECAL"); break;
        case 0x208: returnHash = FEngHashString("MARKER_NUMBERS"); break;
        case 0x301: returnHash = FEngHashString("MARKER_WINDOW_TINT"); break;
        case 0x302: returnHash = FEngHashString("MARKER_NEON"); break;
        case 0x801: returnHash = FEngHashString("MARKER_PERFORMANCE"); break;
        default: break;
    }
    if (returnHash != 0) {
        if (DoesStringExist(returnHash)) {
            return returnHash;
        }
    }
    return 0x9bb9ccc3;
}

void CarCustomizeManager::GetCarPartList(int car_slot, bTList<SelectablePart> &the_list, unsigned int param) {
    CarType cartype = TuningCar->GetType();
    CarPart *part = CarPartDB.NewGetNextCarPart(nullptr, cartype, car_slot, 0, -1);
    while (part) {
        eUnlockableEntity unlockable = MapCarPartToUnlockable(car_slot, part);
        bool should_add = false;
        if (car_slot == 0x2e || car_slot == 0x2c || car_slot == 0x30) {
            int level = 0;
            if (car_slot == 0x2e) level = 2;
            else if (car_slot == 0x30) level = 3;
            else if (car_slot == 0x2c) level = 1;
            bool br = CustomizeIsInBackRoom();
            if (!br || UnlockSystem::IsUnlockableUnlocked(UNLOCK_CAREER_MODE, unlockable, level, 0, true)) {
                should_add = true;
            }
        } else {
            bool br = CustomizeIsInBackRoom();
            if (!br) {
                if ((FEDatabase->GetGameMode() & 0x4000) != 0 || (part->GetAppliedAttributeUParam(0xebb03e66, 0u) >> 5) != 7) {
                    should_add = true;
                }
            } else {
                if (UnlockSystem::IsCarPartUnlocked(UNLOCK_CAREER_MODE, car_slot, part, 0, true)) {
                    should_add = true;
                }
            }
        }
        if (should_add) {
            SelectablePart *sp = new SelectablePart(part, car_slot, part->GetAppliedAttributeUParam(0xebb03e66, 0u) >> 5, static_cast<GRace::Type>(7), false, CPS_AVAILABLE, 0, false);
            eCustomizePartState state = CPS_AVAILABLE;
            if (IsPartLocked(sp, 0)) {
                state = CPS_LOCKED;
            } else if (IsPartNew(sp, 0)) {
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
        }
        part = CarPartDB.NewGetNextCarPart(part, cartype, car_slot, 0, -1);
    }
}

void CarCustomizeManager::GetPerformancePartsList(Physics::Upgrades::Type type, bTList<SelectablePart> &the_list) {
    int max_level = GetMaxPackages(type);
    if (max_level > 0) {
        int i = 0;
        do {
            int level = i + 1;
            SelectablePart *sp = new SelectablePart(nullptr, 0, static_cast<unsigned int>(level), static_cast<GRace::Type>(static_cast<int>(type)), true, CPS_AVAILABLE, 0, false);
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

float CarCustomizeManager::GetPreviewHeat(SelectablePart *part) {
    if (!DoesCartHaveActiveParts() || !IsCareerMode()) {
        return GetActualHeat();
    }
    FECareerRecord temp_record;
    FECareerRecord *career_record = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(TuningCar->CareerHandle);
    if (!career_record) return 0.0f;
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

float CarCustomizeManager::GetCartHeat() {
    if (!DoesCartHaveActiveParts() || !IsCareerMode()) {
        return GetActualHeat();
    }
    FECareerRecord temp_record;
    FECareerRecord *career_record = FEDatabase->GetPlayerCarStable(0)->GetCareerRecordByHandle(TuningCar->CareerHandle);
    if (!career_record) return 0.0f;
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

void CarCustomizeManager::MaxOutPerformance() {
    for (int i = 0; i < 7; i++) {
        Physics::Upgrades::Type type = static_cast<Physics::Upgrades::Type>(i);
        int max_level = GetMaxPackages(type);
        int installed = GetInstalledPerfPkg(type);
        for (int level = installed + 1; level <= max_level; level++) {
            SelectablePart *sp = new SelectablePart(nullptr, 0, static_cast<unsigned int>(level), static_cast<GRace::Type>(static_cast<int>(type)), true, CPS_AVAILABLE, 0, false);
            AddToCart(sp);
        }
    }
}
