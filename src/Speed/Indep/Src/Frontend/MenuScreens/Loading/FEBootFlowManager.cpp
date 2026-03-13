#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"

BootFlowManager *BootFlowManager::mInstance;

BootFlowManager *BootFlowManager::Get() {
    return mInstance;
}