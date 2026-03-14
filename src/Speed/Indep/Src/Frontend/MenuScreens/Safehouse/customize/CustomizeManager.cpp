// OWNED BY zFeOverlay AGENT
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

namespace Physics {
namespace Upgrades {
    typedef int Type;
    int GetMaxLevel(const void *pvehicle, Type type);
    bool CanInstallJunkman(const void *pvehicle, Type type);
}
}

struct CarTypeInfo;
extern CarTypeInfo *GetCarTypeInfoFromHash(unsigned int hash);

extern int g_bTestCareerCustomization;

int CarCustomizeManager::GetNumPackages(GRace::Type type) {
    return Physics::Upgrades::GetMaxLevel(&ThePVehicle, static_cast<int>(type));
}

bool CarCustomizeManager::CanInstallJunkman(GRace::Type type) {
    return Physics::Upgrades::CanInstallJunkman(&ThePVehicle, static_cast<int>(type));
}

bool CarCustomizeManager::IsCareerMode() {
    return FEDatabase->IsCareerMode() || g_bTestCareerCustomization;
}

bool CarCustomizeManager::IsHeroCar() {
    return TuningCar->GetType() == 0x29;
}
