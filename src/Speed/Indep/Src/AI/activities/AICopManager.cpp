#include "AICopManager.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"

AICopManager::~AICopManager() {
    if (mMessSpawnCop) {
        Hermes::Handler::Destroy(mMessSpawnCop);
    }
    if (mMessSetAutoSpawn) {
        Hermes::Handler::Destroy(mMessSetAutoSpawn);
    }
    if (mMessSetCopsEnabled) {
        Hermes::Handler::Destroy(mMessSetCopsEnabled);
    }
    if (mMessBreakerStopCops) {
        Hermes::Handler::Destroy(mMessBreakerStopCops);
    }
    if (mMessForcePursuitStart) {
        Hermes::Handler::Destroy(mMessForcePursuitStart);
    }
    RemoveTask(mSimulateTask);
    if (mActionQ) {
        delete mActionQ;
        mActionQ = nullptr;
    }
    if (mSpeech) {
        mSpeech->Release();
    }
}

Sim::IActivity *AICopManager::Construct(Sim::Param params) {
    return new AICopManager(Sim::Param(params));
}
