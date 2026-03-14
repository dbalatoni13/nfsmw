// OWNED BY zFeOverlay AGENT
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"

namespace Physics {
namespace Upgrades {
    int GetMaxLevel(const void *pvehicle, Type type);
    bool CanInstallJunkman(const void *pvehicle, Type type);
}
}

extern CarTypeInfo *GetCarTypeInfo(CarType type);

struct FEMarkerManager {
    int GetNumCustomizeMarkers();
};
extern FEMarkerManager TheFEMarkerManager;

extern int g_bTestCareerCustomization;

int CarCustomizeManager::GetNumPackages(Physics::Upgrades::Type type) {
    return Physics::Upgrades::GetMaxLevel(&ThePVehicle, type);
}

bool CarCustomizeManager::CanInstallJunkman(Physics::Upgrades::Type type) {
    return Physics::Upgrades::CanInstallJunkman(&ThePVehicle, type);
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
