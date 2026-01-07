#include "AICopManager.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"

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

bool AICopManager::IsPendingSupportVehicle(IVehicle *ivehicle) const {
    IPursuitAI *ipv;
    if (ivehicle->QueryInterface(&ipv)) {
        if (ipv->GetSupportGoal().GetValue() != 0) {
            return true;
        }
    }
    return false;
}

eVehicleCacheResult AICopManager::OnQueryVehicleCache(const IVehicle *removethis, const IVehicleCache *whosasking) const {
    if (!IsAttached(removethis)) {
        return VCR_DONTCARE;
    }
    if (!removethis->IsActive() && !removethis->IsLoading() && !IsPendingSupportVehicle(const_cast<IVehicle *>(removethis))) {
        return VCR_DONTCARE;
    }
    if (UTL::COM::ComparePtr(whosasking, ITrafficMgr::Get())) {
        return VCR_WANT;
    }
    if (whosasking == this) {
        return VCR_WANT;
    }
    return VCR_DONTCARE;
}

void AICopManager::OnRemovedVehicleCache(IVehicle *ivehicle) {
    if (ivehicle->IsActive()) {
        if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
            mNumActiveCopHelicopters--;
        } else {
            mNumActiveCopCars--;
        }
    }
}

void AICopManager::OnAttached(IAttachable *pOther) {
    IRoadBlock *iroadblock;
    IPursuit *ipursuit;
    IVehicle *ivehicle;

    if (pOther->QueryInterface(&ivehicle)) {
        mIVehicleList.push_back(ivehicle);
    } else {
        // if (pOther->QueryInterface(&ipursuit)) {
        //     mIPursuitList.push_back(ipursuit);
        // } else {
        //     if (pOther->QueryInterface(&iroadblock)) {
        //         mRoadBlockList.push_back(iroadblock);
        //     } else {
        //     }
        // }
    }
    Activity::OnAttached(pOther);
}
