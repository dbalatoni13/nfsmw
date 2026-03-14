// OWNED BY zFeOverlay AGENT
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"

namespace Physics {
namespace Upgrades {
    typedef int Type;
    int GetMaxLevel(const void *pvehicle, Type type);
    bool CanInstallJunkman(const void *pvehicle, Type type);
}
}

// total size: 0xD0
struct CarTypeInfo {
    char pad[0x5C];                    // offset 0x0
    char WheelOuterRadius;             // offset 0x5C
    char WheelInnerRadiusMin;          // offset 0x5D
    char WheelInnerRadiusMax;          // offset 0x5E
};

typedef int CarType;
extern CarTypeInfo *GetCarTypeInfo(CarType type);

struct FEMarkerManager {
    int GetNumCustomizeMarkers();
};
extern FEMarkerManager TheFEMarkerManager;

struct FEPlayerCarDB {
    void *GetCustomizationRecordByHandle(unsigned char handle);
};

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
    if (!g_bTestCareerCustomization) {
        return TheFEMarkerManager.GetNumCustomizeMarkers();
    }
    return 1;
}

bool CarCustomizeManager::IsCastrolCar() {
    if (TuningCar->GetType() != 0x34) {
        return false;
    }
    return gEasterEggs.IsEasterEggUnlocked(EASTER_EGG_CASTROL);
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
    if (!info)
        return 0;
    return info->WheelInnerRadiusMin;
}

int CarCustomizeManager::GetMaxInnerRadius() {
    CarTypeInfo *info = GetCarTypeInfo(TuningCar->GetType());
    if (!info)
        return 0;
    return info->WheelInnerRadiusMax;
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
    FEPlayerCarDB *db = static_cast<FEPlayerCarDB *>(FEDatabase->GetPlayerCarDB());
    unsigned char handle = TuningCar->GetHandle();
    void *record = db->GetCustomizationRecordByHandle(handle);
    int *installed = reinterpret_cast<int *>(reinterpret_cast<char *>(record) + 0x118);
    return installed[type];
}
