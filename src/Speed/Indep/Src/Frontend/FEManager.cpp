#include "Speed/Indep/Src/Frontend/FEManager.hpp"

FEManager* FEManager::mInstance;
int FEManager::mPauseRequest;
const char* FEManager::mPauseReason[8];

FEManager* FEManager::Get() {
    return mInstance;
}

eGarageType FEManager::GetGarageType() {
    return mGarageType;
}

void FEManager::RequestUnPauseSimulation(const char* reason) {
    --mPauseRequest;
}
