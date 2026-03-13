#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"

FEPackageManager *FEPackageManager::mInstance;

FEPackageManager *FEPackageManager::Get() {
    return mInstance;
}

void FEPackageManager::ErrorTick() {
    BroadcastMessage(0xD0678849);
}

void FEPackageManager::Tick() {
    BroadcastMessage(0xC98356BA);
}