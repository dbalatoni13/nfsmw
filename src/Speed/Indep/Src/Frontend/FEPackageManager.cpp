#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"

FEPackageManager *FEPackageManager::mInstance;

FEPackageManager *FEPackageManager::Get() {
    return mInstance;
}