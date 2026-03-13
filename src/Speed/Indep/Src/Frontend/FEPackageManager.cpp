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

unsigned int FEngGetActiveScreensChecksum() {
    return FEPackageManager::Get()->GetActiveScreensChecksum();
}

FEPackage *FEPackageManager::FindPackage(const char *pkg_name) {
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (!data) {
        return nullptr;
    }
    return data->GetPackage();
}

void *FEPackageManager::GetPackageData(const char *pkg_name) {
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (!data) {
        return nullptr;
    }
    return data->GetDataChunk();
}

MenuScreen *FEPackageManager::FindScreen(const char *pkg_name) {
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (!data) {
        return nullptr;
    }
    return data->GetScreen();
}

FEPackageData *FEPackageManager::FindFEPackageData(bChunk *chunk) {
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->GetChunk() == chunk) {
            return f;
        }
    }
    return nullptr;
}