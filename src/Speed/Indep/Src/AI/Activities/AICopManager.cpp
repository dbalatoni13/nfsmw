#include "AICopManager.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Src/AI/AIRoadBlock.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicleHelicopter.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeReputation.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitsupport.h"
#include "Speed/Indep/Src/Generated/Events/EShowMilestones.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyPlayerRep.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpEscaped.h"
#include "Speed/Indep/Src/Generated/Messages/MReqBackup.h"
#include "Speed/Indep/Src/Generated/Messages/MReqRoadBlock.h"
#include "Speed/Indep/Src/Generated/Messages/MSetCopAutoSpawnMode.h"
#include "Speed/Indep/Src/Generated/Messages/MUnspawnCop.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ICopMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficMgr.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

#include <algorithm>
#include <cfloat>

int ICopMgr::mDisableCops = false;

float AICopManager::mCopMinSpawnDist = 150.0f;
float AICopManager::mCopMaxSpawnDist = 400.0f;

AICopManager *TheOneCopManager = nullptr;

// TODO apply macros everywhere in zAI
UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::Prototype _AIPursuit("AIPursuit", AIPursuit::Construct);
// UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::Prototype _AIRoadBlock("AIRoadBlock", AIRoadBlock::Construct);
UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::Prototype _AICopManager("AICopManager", AICopManager::Construct);

// Just an inflated stack
AICopManager::AICopManager(Sim::Param params)
    : Sim::Activity(2),                                   //
      AISpawnManager(mCopMinSpawnDist, mCopMaxSpawnDist), //
      ICopMgr(this),                                      //
      IVehicleCache(this),                                //
      mMaxActiveCopHelicopters(1),                        //
      mMaxCopHelicopters(1),                              //
      mNumActiveCopCars(0),                               //
      mNumActiveCopHelicopters(0),                        //
      mPursuitsInARow(0),                                 //
      mTotalCopsDestroyed(0),                             //
      mNoNewPursuitsOrCops(false),                        //
      mNumCopsForLatchedRoadblockReq(0),                  //
      mIPursuitWithLatchedRoadblockReq(nullptr),          //
      mPursuitRequestVehicle(nullptr),                    //
      mMaxPatrolCopCars(2),                               //
      mMaxActiveCopCars(8),                               //
      mLockoutTimer(60.0f),                               //
      mHeavySupportDelayTimer(0.0f),                      //
      mMaxCopCars(8),                                     //
      mPlatformBudgetCopCars(8),                          //
      mSpeech(nullptr) {
    this->MakeDebugable(DBG_AI);
    this->mSpeech = Sim::IActivity::CreateInstance("SoundAI", Sim::Param());
    // TODO is the if check in Attach?
    if (this->mSpeech) {
        this->Attach(this->mSpeech);
    }
    this->mMessSetAutoSpawn =
        Hermes::Handler::Create<MSetCopAutoSpawnMode, AICopManager, AICopManager>(this, &AICopManager::MessageSetAutoSpawnMode, "AICopManager", 0);
    this->mMessSetCopsEnabled =
        Hermes::Handler::Create<MSetCopsEnabled, AICopManager, AICopManager>(this, &AICopManager::MessageSetCopsEnabled, "AICopManager", 0);
    this->mMessBreakerStopCops =
        Hermes::Handler::Create<MBreakerStopCops, AICopManager, AICopManager>(this, &AICopManager::MessageBreakerStopCops, "AICopManager", 0);
    this->mMessForcePursuitStart =
        Hermes::Handler::Create<MForcePursuitStart, AICopManager, AICopManager>(this, &AICopManager::MessageForcePursuitStart, "AICopManager", 0);

    this->mIVehicleList.clear();
    this->mIPursuitList.clear();
    this->mIVehicleList.reserve(10);

    this->mSimulateTask = this->AddTask("AICopManager", 0.5f, 0.0f, Sim::TASK_FRAME_FIXED);
    Sim::ProfileTask(this->mSimulateTask, "AICopManager");

    this->mMaxCopCars = UMath::Min(this->mMaxCopCars, this->mPlatformBudgetCopCars);

    if (SkipFEMaxCops > 0 || SkipFEHelicopter > 0) {
        this->mMaxActiveCopCars = UMath::Min(this->mMaxCopCars, SkipFEMaxCops);
        this->mMaxActiveCopHelicopters = UMath::Min(this->mMaxCopHelicopters, SkipFEHelicopter);
    }

    ICopMgr::mDisableCops = SkipFE && SkipFEDisableCops;

    this->mActionQ = new ActionQueue(0, 0x98c7a2f5, "AICopManager", false); // TODO magic
    this->mAttributes = nullptr;
    TheOneCopManager = this;
}

AICopManager::~AICopManager() {
    if (this->mMessSpawnCop) {
        Hermes::Handler::Destroy(this->mMessSpawnCop);
    }
    if (this->mMessSetAutoSpawn) {
        Hermes::Handler::Destroy(this->mMessSetAutoSpawn);
    }
    if (this->mMessSetCopsEnabled) {
        Hermes::Handler::Destroy(this->mMessSetCopsEnabled);
    }
    if (this->mMessBreakerStopCops) {
        Hermes::Handler::Destroy(this->mMessBreakerStopCops);
    }
    if (this->mMessForcePursuitStart) {
        Hermes::Handler::Destroy(this->mMessForcePursuitStart);
    }
    this->RemoveTask(this->mSimulateTask);
    if (this->mActionQ) {
        delete this->mActionQ;
        this->mActionQ = nullptr;
    }
    if (this->mSpeech) {
        this->mSpeech->Release();
    }
}

Sim::IActivity *AICopManager::Construct(Sim::Param params) {
    return new AICopManager(params);
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
    if (!this->IsAttached(removethis)) {
        return VCR_DONTCARE;
    }
    if (!removethis->IsActive() && !removethis->IsLoading() && !this->IsPendingSupportVehicle(const_cast<IVehicle *>(removethis))) {
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
            this->mNumActiveCopHelicopters--;
        } else {
            this->mNumActiveCopCars--;
        }
    }
}

// Functionally matching, reserve shouldn't get inlined for some reason though
void AICopManager::OnAttached(IAttachable *pOther) {
    IRoadBlock *iroadblock;
    IPursuit *ipursuit;
    IVehicle *ivehicle;

    if (pOther->QueryInterface(&ivehicle)) {
        this->mIVehicleList.push_back(ivehicle);
        if (this->mSpeech) {
            this->mSpeech->Attach(ivehicle);
        }
    } else {
        if (pOther->QueryInterface(&ipursuit)) {
            this->mIPursuitList.push_back(ipursuit);
        } else {
            if (pOther->QueryInterface(&iroadblock)) {
                this->mRoadBlockList.push_back(iroadblock);
            } else {
                return;
            }
        }
    }
    this->Activity::OnAttached(pOther);
}

void AICopManager::OnDetached(IAttachable *pOther) {
    IRoadBlock *iroadblock;
    IPursuit *ipursuit;
    IVehicle *ivehicle;

    if (pOther->QueryInterface(&ivehicle)) {
        IVehicle::List::iterator iter = std::find(this->mIVehicleList.begin(), this->mIVehicleList.end(), ivehicle);
        if (iter != this->mIVehicleList.end()) {
            this->mIVehicleList.erase(iter);
        }
        if (this->mSpeech) {
            this->mSpeech->Detach(ivehicle);
        }
    } else {
        if (pOther->QueryInterface(&ipursuit)) {
            this->mIPursuitList.erase(std::find(this->mIPursuitList.begin(), this->mIPursuitList.end(), ipursuit));
        } else {
            if (pOther->QueryInterface(&iroadblock)) {
                this->mRoadBlockList.erase(std::find(this->mRoadBlockList.begin(), this->mRoadBlockList.end(), iroadblock));
            }
        }
    }
    if (UTL::COM::ComparePtr(pOther, this->mSpeech)) {
        this->mSpeech = nullptr;
    }
    this->Activity::OnDetached(pOther);
}

bool AICopManager::VehicleSpawningEnabled(bool isdespawn) {
    if (this->mNoNewPursuitsOrCops && !isdespawn) {
        return false;
    }
    if (this->mLockoutTimer > 0.0f || mDisableCops) {
        return false;
    }
    if (INIS::Exists() && (!isdespawn || !INIS::Get()->IsWorldMomement())) {
        return false;
    }
    return true;
}

IVehicle *AICopManager::GetAvailableCopVehicleByClass(UCrc32 vehicleClass, bool bValidOnesOnly) {
    if (!this->VehicleSpawningEnabled(false)) {
        return nullptr;
    }
    IPerpetrator *perp = nullptr;
    ISimable *simable = IPlayer::First(PLAYER_LOCAL)->GetSimable();
    if (simable) {
        simable->QueryInterface(&perp);
    }
    const char *name = nullptr;
    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->GetVehicleClass() != vehicleClass || ivehicle->IsActive()) {
            continue;
        }
        if (!name) {
            name = ivehicle->GetVehicleName();
            if (bValidOnesOnly && perp && !IsValidPursuitCarName(name)) {
                name = nullptr;
                continue;
            }
        }
        if (ivehicle->IsLoading()) {
            return nullptr;
        }
        return ivehicle;
    }
    if (!name) {
        if (vehicleClass == VehicleClass::CHOPPER) {
            name = "copheli";
        } else {
            if (perp) {
                name = GetRandomValidCopCar();
            }
            if (!name) {
                name = "copmidsize";
            }
        }
    }
    return this->GetAvailableCopVehicleByName(name);
}

IVehicle *AICopManager::GetAvailableCopVehicleByName(const char *name) {
    if (!this->VehicleSpawningEnabled(false)) {
        return nullptr;
    }
    if (!name) {
        return nullptr;
    }
    UCrc32 nameHash = UCrc32(name);
    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (!ivehicle->IsActive() && !this->IsPendingSupportVehicle(ivehicle)) {
            if (UCrc32(ivehicle->GetVehicleName()) != nameHash) {
                continue;
            }
            if (ivehicle->IsLoading()) {
                return nullptr;
            } else {
                return ivehicle;
            }
        }
    }

    UMath::Vector3 initialVec = {0.0f, 1.0f, 0.0f};
    UMath::Vector3 initialPos = {0.0f, 0.0f, 0.0f};
    ISimable *isimable =
        ISimable::CreateInstance("PVehicle", VehicleParams(this, DRIVER_COP, Attrib::StringToKey(name), initialVec, initialPos, 0, nullptr, 0));
    if (isimable) {
        this->Attach(isimable);
        IVehicle *ivehicle;
        if (isimable->QueryInterface(&ivehicle)) {
            ivehicle->GetAIVehiclePtr()->UnSpawn();
            MUnspawnCop(isimable->GetOwnerHandle(), 4).Send("SoundAI");
            if (ivehicle->IsLoading()) {
                return nullptr;
            } else {
                return ivehicle;
            }
        }
    }
    return nullptr;
}

IVehicle *AICopManager::GetActiveCopVehicleFromOutOfView(UCrc32 vehicleClass) {
    if (!this->VehicleSpawningEnabled(false)) {
        return nullptr;
    }
    IVehicle *bestChoice = nullptr;
    float bestScore = 0.0f;
    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->GetVehicleClass() != vehicleClass) {
            continue;
        }
        if (ivehicle->IsActive()) {
            float score;
            if (ivehicle->GetOffscreenTime() > 5.0f) {
                IRenderable *ir;
                if (ivehicle->QueryInterface(&ir)) {
                    score = ir->DistanceToView();
                    if (score > bestScore && score > 100.0f) {
                        bestScore = score;
                        bestChoice = ivehicle;
                    }
                }
            }
        }
    }
    if (bestChoice) {
        this->RemoveActiveCopVehicle(bestChoice);
        MUnspawnCop(bestChoice->GetSimable()->GetOwnerHandle(), 5).Send("SoundAI");
    }
    return bestChoice;
}

IPursuit *AICopManager::GetPursuitActivity(ISimable *itargetSimable) {
    for (Pursuits::const_iterator iter = this->mIPursuitList.begin(); iter != this->mIPursuitList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        AITarget *pursuitTarget = ipursuit->GetTarget();
        if (pursuitTarget && UTL::COM::ComparePtr(pursuitTarget->GetSimable(), itargetSimable)) {
            return ipursuit;
        }
    }

    IPursuit *ipursuitActivity;
    Sim::IActivity *newinstance = Sim::IActivity::CreateInstance("AIPursuit", Sim::Param());
    newinstance->QueryInterface(&ipursuitActivity);
    this->Attach(ipursuitActivity);
    IAttachable *targetattachable;
    if (itargetSimable->QueryInterface(&targetattachable)) {
        targetattachable->Attach(newinstance);
    }

    return ipursuitActivity;
}

void AICopManager::SpawnCop(UMath::Vector3 &InitialPos, UMath::Vector3 &InitialVec, const char *VehicleName, bool InPursuit, bool RoadBlock) {
    this->mSpawnRequests.push_back(SpawnCopRequest(InitialPos, InitialVec, VehicleName, InPursuit, RoadBlock));
}

static bool DoesSpotOverlapCar(const UMath::Vector3 &checkPos, float radius) {
    for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_ALL).begin(); iter != IVehicle::GetList(VEHICLE_ALL).end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->GetPhysicsMode() != PHYSICS_MODE_SIMULATED) {
            continue;
        }
        ISimable *isimable = ivehicle->GetSimable();
        if (isimable && isimable->GetRigidBody()) {
            float vradius = isimable->GetRigidBody()->GetRadius() + radius + 2.0f;
            float distance = UMath::DistanceSquare(checkPos, isimable->GetRigidBody()->GetPosition());
            if (distance < vradius * vradius) {
                return true;
            }
        }
    }
    return false;
}

bool AICopManager::TrySpawnCop(const SpawnCopRequest &request) {
    UMath::Vector3 initialPos = request.InitialPos;
    UMath::Vector3 initialVec = request.InitialVec;
    const char *vehicleName = request.VehicleName;
    this->UpdateCopPriorities(this->mNumActiveCopCars + 1);

    IVehicle *availableCopCar = this->GetAvailableCopVehicleByName(vehicleName);
    IPursuitAI *ipv;
    if (!availableCopCar || !availableCopCar->QueryInterface(&ipv)) {
        return false;
    }
    ISimable *isimable = availableCopCar->GetSimable();
    if (!isimable || !isimable->GetRigidBody() || DoesSpotOverlapCar(initialPos, isimable->GetRigidBody()->GetRadius())) {
        return false;
    }
    initialPos.y += 1.0f;
    bool on_world = availableCopCar->SetVehicleOnGround(initialPos, initialVec);
    availableCopCar->Activate();
    IVehicleAI *ai = availableCopCar->GetAIVehiclePtr();
    ai->SetSpawned();

    if (!on_world) {
        WRoadNav nav;
        nav.InitAtPoint(initialPos, initialVec, false, 0.0f);
        initialPos.y = nav.GetPosition().y + 1.0f;
        ai->EnableSimplePhysics();
    }
    if (request.InPursuit) {
        ISimable *targetSimable = IPlayer::First(PLAYER_LOCAL)->GetSimable();
        IPursuit *ipursuitActivity = this->GetPursuitActivity(targetSimable);

        ipv->StartPursuit(nullptr, targetSimable);
        ipursuitActivity->AddVehicle(availableCopCar);
    } else if (request.InRoadBlock) {
        ipv->StartRoadBlock();
    } else {
        ipv->StartPatrol();
    }
    this->mNumActiveCopCars++;
    return true;
}

bool AICopManager::IsCopSpawnPending() const {
    return this->mSpawnRequests.size() != 0;
}

void AICopManager::UpdateSpawnRequests() {
    for (SpawnList::iterator i = this->mSpawnRequests.begin(); i != this->mSpawnRequests.end();) {
        if (this->TrySpawnCop(*i)) {
            SpawnList::iterator j = i++;
            this->mSpawnRequests.erase(j);
        } else {
            ++i;
        }
    }
}

void AICopManager::MessageSetAutoSpawnMode(const MSetCopAutoSpawnMode &message) {}

void AICopManager::MessageSetCopsEnabled(const MSetCopsEnabled &message) {
    ICopMgr::mDisableCops = message.GetCopsEnabled() == false;
}

void AICopManager::MessageForcePursuitStart(const MForcePursuitStart &message) {
    this->PursueAtHeatLevel(message.GetMinHeatLevel());
}

void AICopManager::SetAllBustedTimersToZero() {
    for (Pursuits::const_iterator iter = this->mIPursuitList.begin(); iter != this->mIPursuitList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        ipursuit->SetBustedTimerToZero();
    }
}

void AICopManager::MessageBreakerStopCops(const MBreakerStopCops &message) {
    this->mBreakerZones.push_back(BreakerZone(message.GetInitialPos(), message.GetRadius(), Sim::GetTime() + message.GetDuration()));
}

void AICopManager::ApplyBreakerZones() {
    if (this->mBreakerZones.empty()) {
        return;
    }
    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->IsDestroyed()) {
            continue;
        }
        IVehicleAI *iai = ivehicle->GetAIVehiclePtr();
        IPursuitAI *ipv;
        if (!iai || !ivehicle->QueryInterface(&ipv)) {
            continue;
        }
        bool in_zone = false;
        for (BreakerList::const_iterator b = this->mBreakerZones.begin(); b != this->mBreakerZones.end(); ++b) {
            UMath::Vector3 position = b->position;
            float dist = UMath::Distancexz(position, ivehicle->GetPosition());
            if (dist < b->radius) {
                in_zone = true;
                break;
            }
        }

        IDamageable *idamageable;
        if (in_zone && ivehicle->QueryInterface(&idamageable)) {
            idamageable->Destroy();
        }
    }
    for (BreakerList::iterator b = this->mBreakerZones.begin(); b != this->mBreakerZones.end();) {
        BreakerList::iterator t = b++;
        if (t->endtime < Sim::GetTime()) {
            this->mBreakerZones.erase(t);
        }
    }
}

bool AICopManager::SpawnPatrolCar() {
    IVehicle *availableCopCar = this->GetAvailableCopVehicleByClass(VehicleClass::CAR, true);
    if (!availableCopCar) {
        return false;
    }
    short segInd = 0;
    char laneInd = 0;
    float timeStep = 0.0f;
    if (this->GetSpawnLocation(segInd, laneInd, timeStep)) {
        IVehicleAI *ivehicleAI = availableCopCar->GetAIVehiclePtr();
        IPursuitAI *ipv;
        if (ivehicleAI->QueryInterface(&ipv)) {
            availableCopCar->Activate();
            ivehicleAI->ResetVehicleToRoadNav(segInd, laneInd, timeStep);
            ivehicleAI->SetSpawned();
            ipv->StartPatrol();
            this->mNumActiveCopCars++;
            return true;
        }
    }
    return false;
}

bool AICopManager::GetSpawnPositionAheadOfTarget(IPursuit *ip, UMath::Vector3 &pos, UMath::Vector3 &forward, float distAhead) {
    AITarget *pursuitTarget = ip->GetTarget();
    if (pursuitTarget) {
        pursuitTarget->GetForwardVector(forward);
        if (distAhead < 0.0f) {
            UMath::Scale(forward, -1.0f);
            distAhead = -distAhead;
        }
        WRoadNav testNav;
        IPerpetrator *iperp;
        if (pursuitTarget->QueryInterface(&iperp)) {
            if (iperp->IsRacing()) {
                testNav.SetRaceFilter(true);
            } else {
                testNav.SetRaceFilter(false);
                testNav.SetTrafficFilter(false);
                testNav.SetCopFilter(true);
            }
        }
        testNav.SetPathType(WRoadNav::kPathCop);
        testNav.SetNavType(WRoadNav::kTypeDirection);
        testNav.InitAtPoint(pursuitTarget->GetPosition(), forward, false, 0.0f);

        if (!testNav.IsValid() && testNav.GetCopFilter()) {
            testNav.SetCopFilter(false);
            testNav.InitAtPoint(pursuitTarget->GetPosition(), forward, false, 0.0f);
        }

        if (testNav.IsValid()) {
            bool checkDist = false;
            testNav.IncNavPosition(distAhead, forward, 0.0f);
            pos = testNav.GetPosition();
            forward = testNav.GetForwardVector();
            if (!GManager::Get().GetIsWarping()) {
                checkDist = GManager::Get().GetStartFreeRoamPursuit() == false;
            }
            if (this->CheckSpawnPosition(pos, false, 0, 0, checkDist)) {
                if (!checkDist) {
                    UMath::Scale(forward, -1.0f);
                }
                return true;
            }
        }
    }
    return false;
}

bool AICopManager::SpawnPursuitCar(IPursuit *ipursuit) {
    IVehicle *availableCopCar = this->GetAvailableCopVehicleByClass(VehicleClass::CAR, true);
    const char *name;
    float distance;
    if (!GManager::Get().GetIsWarping() && !GManager::Get().GetStartFreeRoamPursuit()) {
        distance = mCopMaxSpawnDist - 60.0f;
    } else {
        distance = 80.0f;
    }
    if (!availableCopCar) {
        availableCopCar = this->GetActiveCopVehicleFromOutOfView(VehicleClass::CAR);
        if (!availableCopCar) {
            return false;
        }
    }
    UMath::Vector3 spawnPosition;
    UMath::Vector3 spawnInitialVec;
    if (this->GetSpawnPositionAheadOfTarget(ipursuit, spawnPosition, spawnInitialVec, distance)) {
        availableCopCar->Activate();
        IVehicleAI *ivehicleAI = availableCopCar->GetAIVehiclePtr();
        ivehicleAI->ResetVehicleToRoadPos(spawnPosition, spawnInitialVec);
        ivehicleAI->SetSpawned();

        IPursuitAI *ipai;
        if (ivehicleAI->QueryInterface(&ipai)) {
            ipai->ZeroTimeSinceTargetSeen();
        }
        ipursuit->AddVehicle(availableCopCar);
        this->mNumActiveCopCars++;
        return true;
    }
    AITarget *pursuitTarget = ipursuit->GetTarget();
    if (pursuitTarget) {
        this->SpawnCopCarNow(ipursuit);
    }

    return false;
}

UMath::Vector3 rand_point_in_circle() {
    float angle = Sim::GetRandom()._SimRandom_FloatRange(M_TWOPI);
    float radius = UMath::Sqrt(Sim::GetRandom()._SimRandom_FloatRange(1.0f));

    UMath::Vector3 r;
    r.x = UMath::Sinr(angle) * radius;
    r.y = 0.0f;
    r.z = UMath::Cosr(angle) * radius;

    return r;
}

bool AICopManager::SpawnPursuitCarByName(IPursuit *ipursuit, const char *name) {
    UCrc32 nameHash(name);
    if (!(nameHash == "copheli")) {
        IVehicle *availableCopCar = this->GetAvailableCopVehicleByName(name);
        bool rv = false;
        if (availableCopCar) {
            rv = this->SpawnPursuitIVehicle(ipursuit, availableCopCar);
        }
        return rv;
    } else {
        return this->SpawnPursuitHelicopter(ipursuit);
    }
}

void AICopManager::SpawnVehicleBehindTarget(IPursuit *ipursuit, IVehicle *availableCopCar) {
    availableCopCar->Activate();

    UMath::Vector3 position = IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody()->GetPosition();
    UMath::Vector3 forwardVector;
    IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody()->GetForwardVector(forwardVector);
    UMath::ScaleAdd(forwardVector, -15.0f, position, position);

    availableCopCar->SetVehicleOnGround(position, forwardVector);
    availableCopCar->GetAIVehiclePtr()->SetSpawned();
    if (ipursuit) {
        ipursuit->AddVehicle(availableCopCar);
    } else {
        IPursuitAI *ipv;
        if (availableCopCar->QueryInterface(&ipv)) {
            ipv->StartPatrol();
        }
    }
    this->mNumActiveCopCars++;
}

bool AICopManager::SpawnCopCarNow(IPursuit *ipursuit) {
    IVehicle *availableCopCar = this->GetAvailableCopVehicleByClass(VehicleClass::CAR, false);
    if (availableCopCar) {
        this->SpawnVehicleBehindTarget(ipursuit, availableCopCar);
        return true;
    }
    return false;
}

bool AICopManager::SpawnPursuitHelicopter(IPursuit *ipursuit) {
    if (HeliVehicleActive()) {
        return false;
    }
    IVehicle *availableCopHelicopter = this->GetAvailableCopVehicleByClass(VehicleClass::CHOPPER, false);
    if (!availableCopHelicopter) {
        return false;
    }
    AITarget *pursuitTarget = ipursuit->GetTarget();
    if (pursuitTarget) {
        UMath::Vector3 targetposition = pursuitTarget->GetPosition();
        UMath::Vector3 targetForwardVector;
        pursuitTarget->GetForwardVector(targetForwardVector);
        UMath::Scale(targetForwardVector, -1.0f, targetForwardVector);

        WRoadNav testNav;
        testNav.SetPathType(WRoadNav::kPathCop);
        testNav.SetNavType(WRoadNav::kTypeDirection);
        testNav.InitAtPoint(targetposition, targetForwardVector, false, 0.0f);

        UMath::Vector3 spawnPosition;
        UMath::Vector3 spawnInitialVec;
        if (testNav.IsValid()) {
            testNav.IncNavPosition(250.0f, targetForwardVector, 0.0f);
            spawnPosition = testNav.GetPosition();
            spawnInitialVec = testNav.GetForwardVector();
            UMath::Normalize(spawnInitialVec);
        } else {
            spawnPosition = pursuitTarget->GetPosition();
            pursuitTarget->GetForwardVector(spawnInitialVec);
        }
        spawnPosition.y += 20.0f;
        availableCopHelicopter->Activate();

        UMath::Matrix4 orientMat = Util_GenerateMatrix(spawnInitialVec, nullptr);
        availableCopHelicopter->GetSimable()->GetRigidBody()->PlaceObject(orientMat, spawnPosition);
        availableCopHelicopter->GetAIVehiclePtr()->SetSpawned();
        ipursuit->AddVehicle(availableCopHelicopter);
        this->mNumActiveCopHelicopters++;
        return true;
    }
    return false;
}

RoadblockSetup *PickRoadblockSetup(float widthToCover, int numCarsAvailable, bool bSpikeStrips) {
    RoadblockSetup *best = nullptr;
    float bestScore = FLT_MAX;
    RoadblockSetup *rbs = bSpikeStrips ? SPIKES_RoadblockCandidateList : RoadblockCandidateList;

    // TODO were these used at all?
    int index;
    int bestIndex;
    for (float score = rbs->mMinimumWidthRequired; rbs->mMinimumWidthRequired > 0.1f; rbs++, score = rbs->mMinimumWidthRequired) {
        score = widthToCover - score;
        if (score > 0.0f && numCarsAvailable >= rbs->mRequiredVehicles && score < bestScore) {
            best = rbs;
            bestScore = score;
        }
    }

    return best;
}

DECLARE_CONTAINER_TYPE(AICopManagerCreateRoadBlockVehicles);

bool AICopManager::CreateRoadBlock(IPursuit *ipursuit, int cop_count, IVehicle *ivehicle_chopper, IVehicle::List *suvList) {
    const float kRoadBlockAheadDistance = 250.0f;

    AITarget *target = ipursuit->GetTarget();

    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = nullptr;
    IPerpetrator *iperp;
    if (target->QueryInterface(&iperp)) {
        pursuitLevelAttrib = iperp->GetPursuitLevelAttrib();
    }
    if (pursuitLevelAttrib == nullptr) {
        return false;
    }

    UMath::Vector3 targetForwardVector;
    target->GetForwardVector(targetForwardVector);
    UMath::Vector3 targetPosition = target->GetPosition();

    WRoadNav roadBlockNav;
    roadBlockNav.SetCookieTrail(true);
    roadBlockNav.SetPathType(WRoadNav::kPathCop);
    roadBlockNav.SetNavType(WRoadNav::kTypeDirection);
    roadBlockNav.InitAtPoint(targetPosition, targetForwardVector, false, 0.0f);

    if (!roadBlockNav.IsValid()) {
        return false;
    }

    roadBlockNav.IncNavPosition(kRoadBlockAheadDistance, UMath::Vector3::kZero, 0.0f);

    int nodeIndex = roadBlockNav.GetNodeInd();
    int segmentIndex = roadBlockNav.GetSegmentInd();
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(segmentIndex);

    if ((segment == nullptr) || !segment->IsTrafficAllowed() || segment->IsDecision() || segment->GetLength() < 40.0f) {
        return false;
    }

    const WRoadProfile *profile = roadNetwork.GetSegmentProfile(*segment, nodeIndex);
    if ((profile == nullptr) || profile->fNumZones == 0) {
        return false;
    }

    UMath::Vector3 rightPos = roadBlockNav.GetRightPosition();
    UMath::Vector3 leftPos = roadBlockNav.GetLeftPosition();
    float crW = UMath::Distance(leftPos, rightPos);
    float cr_width = crW + 1.0f;

    int widthIndex = UMath::Min(static_cast<int>(cr_width * 0.25f), 6);

    const int MinCopsForWidth[7] = {2, 2, 3, 3, 4, 4, 5};
    int numCopsToAskFor = MinCopsForWidth[widthIndex] + 1;

    UTL::Std::vector<IVehicle *, _type_AICopManagerCreateRoadBlockVehicles> vehicles;
    vehicles.reserve(numCopsToAskFor);

    if (suvList != nullptr) {
        for (IVehicle *const *iter = suvList->begin(); iter != suvList->end(); ++iter) {
            IVehicle *ivehicle = *iter;
            IPursuitAI *ipv;
            if (ivehicle->QueryInterface(&ipv)) {
                ipv->SetSupportGoal(static_cast<const char *>(nullptr));
            }
            ivehicle->Activate();
            vehicles.push_back(ivehicle);
        }
    } else {
        for (int i = 0; i < numCopsToAskFor; i++) {
            IVehicle *availableCopCar = this->GetAvailableCopVehicleByClass(VehicleClass::CAR, true);
            if (availableCopCar != nullptr) {
                availableCopCar->Activate();
                vehicles.push_back(availableCopCar);
            }
        }
    }

    for (int i = 0; i < static_cast<int>(vehicles.size()); i++) {
        vehicles[i]->GetAIVehiclePtr()->UnSpawn();
        ISimable *isimable = vehicles[i]->GetSimable();
        MUnspawnCop(isimable->GetOwnerHandle(), 3).Send("SoundAI");
    }

    int num_to_grab = numCopsToAskFor - static_cast<int>(vehicles.size());
    for (int i = 0; i < num_to_grab; i++) {
        IVehicle *availableCopCar = this->GetActiveCopVehicleFromOutOfView(VehicleClass::CAR);
        if (availableCopCar != nullptr) {
            vehicles.push_back(availableCopCar);
        }
    }

    int numCopCarsAvailable = static_cast<int>(vehicles.size());
    if (numCopCarsAvailable < numCopsToAskFor - 1) {
        return false;
    }

    float SpikeProb = pursuitLevelAttrib->roadblockspikechance();
    float simProb = Sim::GetRandom()._SimRandom_FloatRange(100.0f);
    bool bWithSpikes = simProb < SpikeProb;
    if (ipursuit->GetPursuitStatus() == PS_COOL_DOWN) {
        bWithSpikes = false;
    }

    RoadblockSetup *rbs = PickRoadblockSetup(cr_width, numCopCarsAvailable, bWithSpikes);
    if (rbs == nullptr) {
        return false;
    }

    float d2left = UMath::DistanceSquare(leftPos, targetPosition);
    float d2right = UMath::DistanceSquare(rightPos, targetPosition);

    UMath::Vector3 offset2Centre;
    UMath::Vector3 *side2use;
    if (d2left > d2right) {
        UMath::Sub(leftPos, rightPos, offset2Centre);
        side2use = &rightPos;
    } else {
        UMath::Sub(rightPos, leftPos, offset2Centre);
        side2use = &leftPos;
    }

    UMath::Scale(offset2Centre, (rbs->mMinimumWidthRequired * 0.5f) / crW);

    UMath::Vector3 CentrePos;
    UMath::Add(*side2use, offset2Centre, CentrePos);

    float xScale = bClamp(cr_width / rbs->mMinimumWidthRequired, 1.0f, 1.14f);

    Sim::IActivity *roadblockActivity = Sim::IActivity::CreateInstance("AIRoadBlock", Sim::Param());
    IRoadBlock *iroadblock;
    roadblockActivity->QueryInterface(&iroadblock);
    this->Attach(iroadblock);
    ipursuit->AddRoadBlock(iroadblock);
    iroadblock->SetPursuit(ipursuit);
    iroadblock->SetRoadBlockCentre(CentrePos, roadBlockNav.GetForwardVector());

    UMath::Matrix4 orientMat = Util_GenerateMatrix(roadBlockNav.GetForwardVector(), nullptr);

    int car_index = 0;
    int rbe_index;
    for (rbe_index = 0; rbe_index < MAX_RB_ELEMENTS; rbe_index++) {
        RoadblockElement &rbe = rbs->mContents[rbe_index];
        if (rbe.mElementType == kNone) {
            break;
        }

        UMath::Vector3 offset = UMath::Vector3Make(rbe.mOffsetX * xScale, 0.0f, rbe.mOffsetZ);
        UMath::Rotate(offset, orientMat, offset);

        UMath::Vector3 facing = roadBlockNav.GetForwardVector();
        UMath::RotateInXZ(rbe.mAngle, facing, facing);

        UMath::Vector3 position;
        UMath::Add(offset, CentrePos, position);

        switch (rbe.mElementType) {
            case kCar: {
                IVehicle *availableCopCar = vehicles[car_index++];
                availableCopCar->Activate();
                IVehicleAI *ivehicleAI = availableCopCar->GetAIVehiclePtr();
                ivehicleAI->ResetVehicleToRoadPos(position, facing);
                ivehicleAI->SetSpawned();
                iroadblock->AddVehicle(availableCopCar);
                this->mNumActiveCopCars++;
                break;
            }
            case kBarrier:
            case kSpikeStrip: {
                IPlaceableScenery *box;
                // TODO hardcoded hashes
                if (rbe.mElementType == kBarrier) {
                    box = IPlaceableScenery::CreateInstance("XO_Sawhorse_1b_00", 0x9663ad06);
                } else {
                    box = IPlaceableScenery::CreateInstance("XO_SpikeBelt_1b_DW_00", 0xca89ef8f);
                    MReqRoadBlock(static_cast<int>(rbe.mOffsetX)).Send("Position");
                }

                if (box != nullptr) {
                    UMath::Vector3 normal;
                    float height;
                    UMath::Matrix4 smackMat;

                    if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(position, height, &normal)) {
                        if (normal.y < 0.0f) {
                            UMath::Scale(normal, -1.0f);
                        }
                        smackMat = Util_GenerateMatrix(facing, &normal);
                    } else {
                        smackMat = Util_GenerateMatrix(facing, nullptr);
                    }

                    iroadblock->AddSmackable(box, rbe.mElementType == kSpikeStrip);
                    ipursuit->NotifySpikeStripDeployed();

                    smackMat.v3.x = position.x;
                    smackMat.v3.y = position.y;
                    smackMat.v3.z = position.z;
                    box->Place(smackMat, true);
                }
                break;
            }
            default:
                break;
        }
    }

    ipursuit->NotifyRoadblockDeployed();

    return true;
}

void AICopManager::RemoveActiveCopVehicle(IVehicle *ivehicle) {
    IVehicleAI *ivehicleAI = ivehicle->GetAIVehiclePtr();
    IPursuit *pursuit = ivehicleAI->GetPursuit();

    if (pursuit) {
        pursuit->RemoveVehicle(ivehicle);
    } else {
        if (ivehicle->GetAIVehiclePtr()->GetRoadBlock()) {
            ivehicle->GetAIVehiclePtr()->GetRoadBlock()->RemoveVehicle(ivehicle);
        }
    }
    bool bIsChopper = ivehicle->GetVehicleClass() == VehicleClass::CHOPPER;
    ivehicleAI->UnSpawn();
    if (bIsChopper) {
        this->mNumActiveCopHelicopters--;
    } else {
        this->mNumActiveCopCars--;
    }
}

void AICopManager::UpdatePatrols() {
    Attrib::Gen::pursuitlevels *pursuitLevelAttrib = GetGlobalPursuitLevelAttrib();

    int wantpatrolcars;
    if (pursuitLevelAttrib) {
        wantpatrolcars = pursuitLevelAttrib->NumPatrolCars();
    } else {
        wantpatrolcars = 1;
    }

    if (this->mIPursuitList.size() > 1) {
        wantpatrolcars = 0;
    } else {
        if (this->mIPursuitList.size() == 1) {
            wantpatrolcars = 0;
            if (GRaceStatus::Exists()) {
                GRaceParameters *raceParms = GRaceStatus::Get().GetRaceParameters();
                if (raceParms && raceParms->GetNumOpponents() > 0) {
                    wantpatrolcars = 1;
                }
            }
        }
    }

    if (!this->mSpawnRequests.empty()) {
        wantpatrolcars = 0;
    }

    int currentpatrolcars = 0;
    static const UCrc32 patrolGoal("AIGoalPatrol");
    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        IVehicleAI *ivehicleAI = ivehicle->GetAIVehiclePtr();
        if (ivehicleAI && ivehicleAI->GetGoalName() == patrolGoal) {
            currentpatrolcars++;
        }
    }

    if (wantpatrolcars > currentpatrolcars) {
        this->SpawnPatrolCar();
    }

    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (!ivehicle->IsActive()) {
            continue;
        }
        IVehicleAI *ivehicleAI = ivehicle->GetAIVehiclePtr();
        IPursuitAI *ipursuitVehicle;
        if (!ivehicleAI->QueryInterface(&ipursuitVehicle)) {
            continue;
        }
        UMath::Vector3 copPosition = ivehicle->GetSimable()->GetPosition();
        bool respawnAvailable = this->RespawnAvailable(copPosition, 0.0f);
        bool canRespawn = ivehicle->GetAIVehiclePtr()->CanRespawn(respawnAvailable);
        bool shouldRespawn = false;
        if (canRespawn && respawnAvailable) {
            shouldRespawn = true;
        }

        bool bIsChopper = ivehicle->GetVehicleClass() == VehicleClass::CHOPPER;
        if (ivehicle->IsDestroyed()) {
            if (canRespawn) {
                float score = ivehicle->GetOffscreenTime();
                if (score > 3.0f) {
                    shouldRespawn = true;
                }
            }
        } else if (shouldRespawn) {
            IPursuit *ipursuit = ivehicleAI->GetPursuit();
            if (ipursuit) {
                UMath::Vector3 targetPosition = ipursuit->GetTarget()->GetPosition();
                UMath::Vector3 targetForward;
                if (ipursuit->GetTarget()->GetSpeed() > 1.0f) {
                    targetForward = ipursuit->GetTarget()->GetLinearVelocity();
                } else {
                    ipursuit->GetTarget()->GetForwardVector(targetForward);
                }
                UMath::Normalize(targetForward);
                UMath::ScaleAdd(targetForward, 75.0f, targetPosition, targetPosition);
                float dist = UMath::Distance(copPosition, targetPosition);
                float compDist = bIsChopper ? 375.0f : 600.0f;
                if (dist < compDist) {
                    shouldRespawn = false;
                }
            }
        }

        bool offWorld = bIsChopper ? false : ivehicle->IsOffWorld();
        bool forceKillRB; // TODO
        bool removeHim = false;
        if (!this->VehicleSpawningEnabled(true) || shouldRespawn) {
            removeHim = true;
        }

        if (removeHim) {
            this->RemoveActiveCopVehicle(ivehicle);
            MUnspawnCop(ivehicle->GetSimable()->GetOwnerHandle(), 6).Send("SoundAI");
        } else {
            if (!offWorld && !ivehicle->IsDestroyed() && !ipursuitVehicle->PursuitRequest()) {
                this->mPursuitRequestVehicle = ivehicle;
                break; // TODO this break is correct but breaks the order
            }
        }
    }
}

const char *ebrakegoals[1] = {"AIGoalPursuit"};
const char *coebrakegoals[2] = {"AIGoalPursuit", "AIGoalPursuit"};
const char *ramgoals[2] = {"AIGoalHeadOnRam", "AIGoalHeadOnRam"};
const char *hrblockgoals[4] = {"AIGoalStaticRoadBlock", "AIGoalStaticRoadBlock", "AIGoalStaticRoadBlock", "AIGoalStaticRoadBlock"};

bool AICopManager::GetHeavySupportVehicles(GroundSupportRequest *gsr) {
    const char *vname;
    const char **vgoals;
    int numVehicles = 0;
    switch (gsr->mHeavySupport->HeavyStrategy) {
        case E_BRAKE:
            numVehicles = 1;
            vgoals = ebrakegoals;
            break;
        case COORDINATED_E_BRAKE:
            numVehicles = 2;
            vgoals = coebrakegoals;
            break;
        case RAM:
            numVehicles = 2;
            vgoals = ramgoals;
            break;
        case HEAVY_ROADBLOCK:
            numVehicles = 4;
            vgoals = hrblockgoals;
            break;
    }
    int BigSuvChance = gsr->mHeavySupport->ChanceBigSUV;
    int index = gsr->mIVehicleList.size();
    for (; index < numVehicles; index++) {
        int rand = Sim::GetRandom()._SimRandom_IntRange(100);
        if (rand < BigSuvChance) {
            vname = "copsuv";
        } else {
            vname = "copsuvl";
        }
        IVehicle *ivehicle = this->GetAvailableCopVehicleByName(vname);
        if (ivehicle) {
            gsr->mIVehicleList.push_back(ivehicle);
            IPursuitAI *ipv;
            if (ivehicle->QueryInterface(&ipv)) {
                ipv->SetSupportGoal(vgoals[index]);
            }
            if (gsr->mHeavySupport->HeavyStrategy == RAM) {
                this->mHeavySupportDelayTimer = 3.0f;
                if (index == numVehicles - 1) {
                    MReqBackup(16).Send("Request"); // TODO 16? what does that mean?
                }
            }
        } else {
            break;
        }
    }
    return this->mHeavySupportDelayTimer < 0.0f && index == numVehicles;
}

bool AICopManager::StartHeavySupport(IPursuit *ipursuit, GroundSupportRequest *gsr) {
    UMath::Vector4 spawnPosition[5];
    UMath::Vector4 spawnInitialVec[5];
    bool GotSpawnPositions = false;
    int cindex = 0;
    float distAdjust = 60.0f;

    for (IVehicle::List::iterator iter = gsr->mIVehicleList.begin(); iter != gsr->mIVehicleList.end(); ++iter) {
        GotSpawnPositions = false;
        IVehicle *availableCopCar = *iter;
        // TODO magic
        for (int j = 0; j < 8; j++) {
            float d2Try = mCopMaxSpawnDist - distAdjust;
            distAdjust += 10.0f;
            if (this->GetSpawnPositionAheadOfTarget(ipursuit, UMath::Vector4To3(spawnPosition[cindex]), UMath::Vector4To3(spawnInitialVec[cindex]),
                                              d2Try)) {
                GotSpawnPositions = true;
                cindex++;
                break;
            }
        }
        if (!GotSpawnPositions) {
            break;
        }
    }
    if (GotSpawnPositions) {
        int index = 0;
        for (IVehicle::List::iterator iter = gsr->mIVehicleList.begin(); iter != gsr->mIVehicleList.end(); ++iter) {
            IVehicle *availableCopCar = *iter;
            availableCopCar->Activate();
            IVehicleAI *ivehicleAI = availableCopCar->GetAIVehiclePtr();

            UMath::Scale(spawnInitialVec[index], -1.0f);
            ivehicleAI->ResetVehicleToRoadPos(UMath::Vector4To3(spawnPosition[index]), UMath::Vector4To3(spawnInitialVec[index]));
            ivehicleAI->SetSpawned();
            ipursuit->AddVehicle(availableCopCar);
            this->mNumActiveCopCars++;

            index++;
        }
        gsr->mSupportRequestStatus = GroundSupportRequest::ACTIVE;
        gsr->mSupportTimer = gsr->mHeavySupport->Duration;
        return true;
    }

    return false;
}

const char *crossfollowgoals[1] = {"AIGoalPursuit"};
const char *crossbrakegoals[1] = {"AIGoalPursuit"};
const char *crossvblockgoals[3] = {"AIGoalPursuit", "AIGoalPursuit", "AIGoalPursuit"};

bool AICopManager::GetLeaderSupportVehicles(GroundSupportRequest *gsr) {
    const char *vname[3] = {"copcross", "copsporthench", "copsporthench"};
    const UMath::Vector4 CrossPursuitOffsets[3] = {
        {0.0f, 0.0f, -20.0f, 0.0f},
        {-4.0f, 0.0f, -24.0f, 0.0f},
        {4.0f, 0.0f, -24.0f, 0.0f},
    };
    const char **vgoals;
    int numVehicles = 0;
    switch (gsr->mLeaderSupport->LeaderStrategy) {
        case CROSS_FOLLOW:
            numVehicles = 1;
            vgoals = crossfollowgoals;
            break;
        case CROSS_BRAKE:
            numVehicles = 1;
            vgoals = crossbrakegoals;
            break;
        case CROSS_PLUS_V_BLOCK:
            numVehicles = 3;
            vgoals = crossvblockgoals;
            break;
    }
    int index = gsr->mIVehicleList.size();

    for (; index < numVehicles; index++) {
        IVehicle *ivehicle = this->GetAvailableCopVehicleByName(vname[index]);
        if (ivehicle) {
            gsr->mIVehicleList.push_back(ivehicle);
            IPursuitAI *ipv;
            if (ivehicle->QueryInterface(&ipv)) {
                ipv->SetSupportGoal(vgoals[index]);
                ipv->SetPursuitOffset(UMath::Vector4To3(CrossPursuitOffsets[index]));
                ipv->SetInPositionGoal((const char *)nullptr);
            }
        } else {
            break;
        }
    }

    return index == numVehicles;
}

bool AICopManager::StartLeaderSupport(IPursuit *ipursuit, GroundSupportRequest *gsr) {
    UMath::Vector4 spawnPosition[3];
    UMath::Vector4 spawnInitialVec[3];
    bool GotSpawnPositions = false;
    int cindex = 0;
    float distAdjust = 0.0f;

    for (IVehicle::List::iterator iter = gsr->mIVehicleList.begin(); iter != gsr->mIVehicleList.end(); ++iter) {
        GotSpawnPositions = false;
        IVehicle *availableCopCar = *iter;
        // TODO magic
        for (int j = 0; j < 8; j++) {
            float d2Try = -(mCopMinSpawnDist + distAdjust);
            distAdjust += 10.0f;
            if (this->GetSpawnPositionAheadOfTarget(ipursuit, UMath::Vector4To3(spawnPosition[cindex]), UMath::Vector4To3(spawnInitialVec[cindex]),
                                              d2Try)) {
                GotSpawnPositions = true;
                cindex++;
                break;
            }
        }
        if (!GotSpawnPositions) {
            break;
        }
    }
    if (GotSpawnPositions) {
        int index = 0;
        for (IVehicle::List::iterator iter = gsr->mIVehicleList.begin(); iter != gsr->mIVehicleList.end(); ++iter) {
            IVehicle *availableCopCar = *iter;
            availableCopCar->Activate();
            IVehicleAI *ivehicleAI = availableCopCar->GetAIVehiclePtr();

            UMath::Scale(spawnInitialVec[index], -1.0f);
            ivehicleAI->ResetVehicleToRoadPos(UMath::Vector4To3(spawnPosition[index]), UMath::Vector4To3(spawnInitialVec[index]));

            IPursuitAI *ipai;
            // no null check?
            ivehicleAI->QueryInterface(&ipai);
            UMath::Vector3 tmp3 = ipai->GetPursuitOffset();

            ivehicleAI->SetSpawned();
            ipai->SetPursuitOffset(tmp3);
            ipai->SetInFormation(true);
            ipursuit->AddVehicle(availableCopCar);
            this->mNumActiveCopCars++;

            index++;
        }
        gsr->mSupportRequestStatus = GroundSupportRequest::ACTIVE;
        gsr->mSupportTimer = gsr->mLeaderSupport->Duration;
        return true;
    }

    return false;
}

void AICopManager::UpdateSupportCops(IPursuit *ipursuit) {
    GroundSupportRequest *gsr = ipursuit->RequestGroundSupport();
    if (gsr && gsr->mSupportRequestStatus == GroundSupportRequest::PENDING) {
        if (gsr->mHeavySupport) {
            if (this->GetHeavySupportVehicles(gsr) == true) {
                if (gsr->mHeavySupport->HeavyStrategy == HEAVY_ROADBLOCK) {
                    if (!ipursuit->GetRoadBlock()) {
                        if (this->CreateRoadBlock(ipursuit, 4, nullptr, &gsr->mIVehicleList)) {
                            gsr->mSupportRequestStatus = GroundSupportRequest::ACTIVE;
                            gsr->mSupportTimer = gsr->mHeavySupport->Duration;
                            MReqRoadBlock(1).Send("Created");
                        } else {
                            MReqRoadBlock(0).Send("Created");
                        }
                    }
                } else if (this->StartHeavySupport(ipursuit, gsr) != true) {
                    MReqBackup(16).Send("ReqDenied");
                }
            }
        } else if (this->GetLeaderSupportVehicles(gsr) == true) {
            if (this->StartLeaderSupport(ipursuit, gsr) != true) {
                MReqBackup(64).Send("ReqDenied");
            } else {
                MReqBackup(64).Send("Request");
            }
        }
    }
}

void AICopManager::PursuitIsEvaded(IPursuit *ipursuit) {
    if (ipursuit->IsPlayerPursuit()) {
        this->mPursuitsInARow++;
        GManager::Get().TrackValue("pursuits_in_a_row", this->mPursuitsInARow);
        GManager::Get().TrackValue("pursuit_evasion_time", ipursuit->GetPursuitDuration());
        GManager::Get().NotifyPursuitEnded(true);
        AdjustStableImpound_EvadePursuit(0);
        if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
            if (SoundAI::Get() && SoundAI::Get()->IsMusicActive()) {
                MControlPathfinder(false, 14, 0, 0).Send("Event");
            }
            new EShowMilestones(ipursuit->GetEnterSafehouseOnDone());
        }
    }
    AITarget *target = ipursuit->GetTarget();
    if (target) {
        ISimable *simable = target->GetSimable();
        if (simable) {
            MPerpEscaped(simable->GetOwnerHandle()).Post(0x20D60DBF); // magic
            IPlayer *iplayer = simable->GetPlayer();
            if (iplayer) {
                int plrIndex = iplayer->GetSettingsIndex();
                if (plrIndex < 2U) {
                    IPerpetrator *perp;
                    AdjustStableHeat_EvadePursuit(plrIndex);
                    simable->QueryInterface(&perp);
                }
            }
        }
    }
}

void AICopManager::CommitPursuitDetails(IPursuit *ipursuit) {
    if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceParameters()->GetIsChallengeSeriesRace()) {
        return;
    }
    AITarget *target = ipursuit->GetTarget();
    if (target) {
        ISimable *simable = target->GetSimable();
        if (simable) {
            IPlayer *iplayer = simable->GetPlayer();
            if (iplayer) {
                int plrIndex = iplayer->GetSettingsIndex();
                if (plrIndex < 2U) {
                    UserProfile *prof = FEDatabase->CurrentUserProfiles[plrIndex];
                    if ((uintptr_t)prof == 0xffffff1c) {
                        return;
                    }
                    CareerSettings *career = prof->GetCareer();
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(plrIndex);
                    if (!stable) {
                        return;
                    }
                    FECarRecord *fe_car = stable->GetCarByIndex(career->GetCurrentCar());
                    if (!fe_car) {
                        return;
                    }
                    FECareerRecord *record = stable->GetCareerRecordByHandle(fe_car->CareerHandle);
                    if (record) {
                        int pursuitBounty;
                        IPerpetrator *perp;
                        if (simable->QueryInterface(&perp)) {
                            pursuitBounty = perp->GetPendingRepPointsNormal();
                            pursuitBounty += perp->GetPendingRepPointsFromCopDestruction();
                            perp->ClearPendingRepPoints();
                            prof->CommitPursuitInfo(ipursuit, fe_car->FEKey, pursuitBounty, GInfractionManager::Get().GetNumInfractions());
                            record->CommitPursuitCarData(GInfractionManager::Get().GetInfractions(), pursuitBounty, !ipursuit->IsPerpBusted());

                            IReputation *irep;
                            if (iplayer->GetHud()->QueryInterface(&irep)) {
                                irep->SetReputationCareer(stable->GetTotalBounty());
                            }

                            if (!ipursuit->IsPerpBusted()) {
                                MNotifyPlayerRep(simable->GetInstanceHandle(), record->GetBounty()).Post(0x20d60dbf);
                            }
                        }
                    }
                }
            }
        }
    }
}

bool AICopManager::IsCopRequestPending() {
    if (!this->VehicleSpawningEnabled(false)) {
        return false;
    }
    if (!this->mSpawnRequests.empty() || this->mIPursuitWithLatchedRoadblockReq) {
        return true;
    }

    for (Pursuits::const_iterator iter = this->mIPursuitList.begin(); iter != this->mIPursuitList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        if (!ipursuit->ShouldEnd()) {
            if (this->mNumActiveCopCars < this->mMaxActiveCopCars && ipursuit->CopRequest()) {
                return true;
            }
            if (ipursuit->PendingRoadBlockRequest()) {
                return true;
            }
        }
    }

    return false;
}

bool AICopManager::CanPursueRacers() {
    if (GRaceStatus::Exists()) {
        GRaceParameters *raceParms = GRaceStatus::Get().GetRaceParameters();
        if (raceParms && raceParms->GetNumOpponents() >= 1) {
            for (Pursuits::const_iterator iter = this->mIPursuitList.begin(); iter != this->mIPursuitList.end(); ++iter) {
                IPursuit *ipursuit = *iter;
                IVehicle *v;
                if (ipursuit->GetTarget()->QueryInterface(&v)) {
                    if (v->GetDriverClass() == DRIVER_RACER) {
                        return false;
                    }
                }
            }
            return true;
        }
    }
    return false;
}

bool AICopManager::IsPlayerPursuitActive() {
    for (Pursuits::const_iterator iter = this->mIPursuitList.begin(); iter != this->mIPursuitList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        if (ipursuit->IsPlayerPursuit()) {
            return true;
        }
    }
    return false;
}

bool AICopManager::PlayerPursuitHasCop() const {
    for (Pursuits::const_iterator iter = this->mIPursuitList.begin(); iter != this->mIPursuitList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        if (ipursuit->IsPlayerPursuit() && ipursuit->ContingentHasActiveCops()) {
            return true;
        }
    }
    return false;
}

// stack issue because of stl vector constructor
void AICopManager::UpdatePursuits() {
    IVehicle *ivehicle_chopper = nullptr;

    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        bool IsDestroyed = ivehicle->IsDestroyed();
        bool IsActive = ivehicle->IsActive();
        if (!IsActive || IsDestroyed) {
            bool vehicleRemoved = false;
            IPursuit *pursuit = ivehicle->GetAIVehiclePtr()->GetPursuit();
            if (pursuit) {
                vehicleRemoved = true;
                pursuit->RemoveVehicle(ivehicle);
            } else {
                if (ivehicle->GetAIVehiclePtr()->GetRoadBlock()) {
                    if (IsDestroyed) {
                        IPursuit *ip = ivehicle->GetAIVehiclePtr()->GetRoadBlock()->GetPursuit();
                        if (ip) {
                            ip->IncNumCopsDestroyed(ivehicle);
                        }
                    }
                    vehicleRemoved = true;
                    ivehicle->GetAIVehiclePtr()->GetRoadBlock()->RemoveVehicle(ivehicle);
                }
            }

            bool bAllowStatsToAccumulate = false;
            if (!GRaceStatus::Exists() || GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Racing ||
                (GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace())) {
                bAllowStatsToAccumulate = true;
            }

            if (IsDestroyed && vehicleRemoved && bAllowStatsToAccumulate) {
                this->mTotalCopsDestroyed++;
                GManager::Get().TrackValue("cops_destroyed", this->mTotalCopsDestroyed);
            }
        } else {
            if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
                ivehicle_chopper = ivehicle;
            }
        }
    }
    Pursuits detachList;
    IPursuitAI *ipursuitVehicle;
    AITarget *pursuitRequestTarget = nullptr;

    if (this->mPursuitRequestVehicle && this->mPursuitRequestVehicle->QueryInterface(&ipursuitVehicle)) {
        pursuitRequestTarget = ipursuitVehicle->PursuitRequest();
    }

    bool bStillHaveLatchedRBPursuit = false;
    for (Pursuits::const_iterator iter = this->mIPursuitList.begin(); iter != this->mIPursuitList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        if (this->mPursuitRequestVehicle && pursuitRequestTarget && ipursuit->IsTarget(pursuitRequestTarget)) {
            ipursuit->AddVehicle(this->mPursuitRequestVehicle);
            this->mPursuitRequestVehicle = nullptr;
        }

        if (ipursuit->ShouldEnd()) {
            detachList.push_back(ipursuit);
            if (ipursuit->IsPerpBusted()) {
                this->mPursuitsInARow = 0;
                GManager::Get().NotifyPursuitEnded(false);
            }
            if (!ipursuit->IsPerpBusted()) {
                this->PursuitIsEvaded(ipursuit);
            }
            this->CommitPursuitDetails(ipursuit);
        } else {
            this->UpdateSupportCops(ipursuit);
            if (this->mNumActiveCopCars < this->mMaxActiveCopCars) {
                if (!this->mIPursuitWithLatchedRoadblockReq) {
                    const char *want_a_cop = ipursuit->CopRequest();
                    if (want_a_cop) {
                        this->SpawnPursuitCarByName(ipursuit, want_a_cop);
                    }
                }
            }
            if (!this->mIPursuitWithLatchedRoadblockReq) {
                int road_block_cops = ipursuit->RequestRoadBlock();
                if (road_block_cops > 0) {
                    this->mNumCopsForLatchedRoadblockReq = road_block_cops;
                    this->mIPursuitWithLatchedRoadblockReq = ipursuit;
                }
            }

            if (this->mIPursuitWithLatchedRoadblockReq == ipursuit && !ipursuit->GetRoadBlock()) {
                bStillHaveLatchedRBPursuit = true;
                if (this->CreateRoadBlock(ipursuit, this->mNumCopsForLatchedRoadblockReq, ivehicle_chopper, nullptr) == true) {
                    MReqRoadBlock(1).Send("Created");
                    this->mIPursuitWithLatchedRoadblockReq = 0;
                    this->mNumCopsForLatchedRoadblockReq = 0;
                } else {
                    MReqRoadBlock(0).Send("Created");
                }
            }
        }
    }

    if (!bStillHaveLatchedRBPursuit) {
        this->mIPursuitWithLatchedRoadblockReq = 0;
        this->mNumCopsForLatchedRoadblockReq = 0;
    }

    for (Pursuits::const_iterator iter = detachList.begin(); iter != detachList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        this->Detach(ipursuit);
        IAttachable *iattachable;
        if (ipursuit->GetTarget()->IsValid() && ipursuit->QueryInterface(&iattachable)) {
            iattachable->Detach(ipursuit->GetTarget()->GetSimable());
        }

        for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
            IVehicle *ivehicle = *iter;
            IPursuit *carspursuit = ivehicle->GetAIVehiclePtr()->GetPursuit();
            if (ComparePtr(carspursuit, ipursuit)) {
                ipursuit->RemoveVehicle(ivehicle);
                IPursuitAI *ipursuitai;
                if (ivehicle->QueryInterface(&ipursuitai)) {
                    ipursuitai->StartFlee();
                }
            }
        }

        Sim::IActivity *iactivity;
        if (ipursuit->QueryInterface(&iactivity)) {
            iactivity->Release();
        }
    }

    if (this->mPursuitRequestVehicle) {
        Sim::IActivity *ipursuitActivity = Sim::IActivity::CreateInstance("AIPursuit", Sim::Param());
        IPursuit *ipursuit;
        ipursuitActivity->QueryInterface(&ipursuit);
        this->Attach(ipursuit);
        IAttachable *targetattachable;
        if (pursuitRequestTarget->QueryInterface(&targetattachable)) {
            targetattachable->Attach(ipursuit);
        }
        IAttachable *pursuitattachable;
        if (ipursuit->QueryInterface(&pursuitattachable)) {
            pursuitattachable->Attach(pursuitRequestTarget->GetSimable());
        }
        ipursuit->AddVehicle(this->mPursuitRequestVehicle);
    }
}

// stack issue because of stl vector constructor
void AICopManager::UpdateRoadBlocks() {
    RoadBlocks detachList;

    for (RoadBlocks::const_iterator iter = this->mRoadBlockList.begin(); iter != this->mRoadBlockList.end(); ++iter) {
        IRoadBlock *iroadblock = *iter;
        if (iroadblock->GetNumCops() == 0) {
            detachList.push_back(iroadblock);
        }
        if (!iroadblock->GetDodged()) {
            IPursuit *ipursuit = iroadblock->GetPursuit();
            if (ipursuit) {
                AITarget *target = ipursuit->GetTarget();
                if (target) {
                    UMath::Vector3 targetPos = target->GetPosition();
                    UMath::Vector3 roadblockPos = iroadblock->GetRoadBlockCentre();
                    UMath::Vector3 roadblockDir = iroadblock->GetRoadBlockDir();
                    UMath::Vector3 targetToRoadblock;
                    UMath::Sub(roadblockPos, targetPos, targetToRoadblock);
                    UMath::Unit(targetToRoadblock, targetToRoadblock);
                    float heading = Dot(targetToRoadblock, roadblockDir);
                    if (heading < 0.0f) {
                        iroadblock->SetDodged(true);
                        ipursuit->NotifyRoadblockDodged();
                        GManager::Get().IncValue("roadblocks_dodged");
                        MReqRoadBlock(1).Send("Dodged");
                        if (iroadblock->GetNumSpikeStrips() > 0) {
                            float spikeStripsDodged = UMath::Max(0.0f, GManager::Get().GetValue("tire_spikes_dodged"));
                            bool bAllowStatsToAccumulate = false;
                            if (!GRaceStatus::Exists() || GRaceStatus::Get().GetPlayMode() != GRaceStatus::kPlayMode_Racing ||
                                (GRaceStatus::Get().GetRaceParameters() && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace())) {
                                bAllowStatsToAccumulate = true;
                            }
                            if (bAllowStatsToAccumulate) {
                                GManager::Get().TrackValue("tire_spikes_dodged", spikeStripsDodged + iroadblock->GetNumSpikeStrips());
                            }
                            ipursuit->NotifySpikeStripsDodged(iroadblock->GetNumSpikeStrips());
                        }
                    }
                }
            }
        }
    }

    for (RoadBlocks::iterator iter = detachList.begin(); iter != detachList.end(); ++iter) {
        Sim::IActivity *iactivity;
        IRoadBlock *iroadblock = *iter;
        iroadblock->ReleaseAllSmackables();
        if (iroadblock->QueryInterface(&iactivity)) {
            iactivity->Release();
        }
        this->Detach(iroadblock);
    }
}

void AICopManager::UpdateDebug() {
    while (!this->mActionQ->IsEmpty()) {
        ActionRef aRef = this->mActionQ->GetAction();
        if (aRef.ID()) {
            // empty?
        }
        this->mActionQ->PopAction();
    }
}

void AICopManager::UpdateCopPriorities(int numActiveCopCars) {
    if (numActiveCopCars <= this->mPlatformBudgetCopCars) {
        return;
    }
    UMath::Vector3 playerPosition;
    UMath::Vector3 playerForwardVec;
    playerPosition = IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetPosition();
    IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody()->GetForwardVector(playerForwardVec);

    float lowestPriority = 0.0f;
    bool foundLowest = false;
    int lowestPriorityIndex = 0;
    int currentIndex = 0;

    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->IsActive()) {
            UMath::Vector3 dirToPlayer;
            UMath::Sub(ivehicle->GetPosition(), playerPosition, dirToPlayer);
            float facingPpriority = UMath::Normalize(dirToPlayer);
            float priority = UMath::Dot(dirToPlayer, playerForwardVec);

            IRenderable *ir;
            if (ivehicle->QueryInterface(&ir) && ir->InView()) {
                priority += 1.0f;
            }

            if (!foundLowest || priority < lowestPriority) {
                foundLowest = true;
                lowestPriorityIndex = currentIndex;
                lowestPriority = priority;
            }
        }
        currentIndex++;
    }

    currentIndex = 0;
    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->IsActive() && lowestPriorityIndex == currentIndex) {
            IPursuit *pursuit = ivehicle->GetAIVehiclePtr()->GetPursuit();
            if (pursuit) {
                pursuit->RemoveVehicle(ivehicle);
            } else {
                if (ivehicle->GetAIVehiclePtr()->GetRoadBlock()) {
                    ivehicle->GetAIVehiclePtr()->GetRoadBlock()->RemoveVehicle(ivehicle);
                }
            }

            ivehicle->GetAIVehiclePtr()->UnSpawn();
            MUnspawnCop(ivehicle->GetSimable()->GetOwnerHandle(), 1).Send("SoundAI");

            if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
                this->mNumActiveCopHelicopters--;
            } else {
                this->mNumActiveCopCars--;
            }
            break;
        }
        currentIndex++;
    }
}

void AICopManager::PursueAtHeatLevel(int minHeatLevel) {
    IPlayer *player = const_cast<IPlayer *>(*IPlayer::GetList(PLAYER_LOCAL).begin());
    if (player) {
        bool alreadyPursued = false;
        IPursuit *playerPursuit = nullptr;
        for (Pursuits::const_iterator iter = this->mIPursuitList.begin(); iter != this->mIPursuitList.end(); ++iter) {
            IPursuit *ipursuit = *iter;
            if (ipursuit->GetTarget()->GetSimable() == player->GetSimable()) {
                alreadyPursued = true;
                playerPursuit = ipursuit;
                break;
            }
        }

        IPerpetrator *perp;
        if (player->GetSimable()->QueryInterface(&perp)) {
            if (perp->GetHeat() < static_cast<float>(minHeatLevel)) {
                perp->SetHeat(std::max(perp->GetHeat(), static_cast<float>(minHeatLevel)));
                if (playerPursuit) {
                    playerPursuit->LockInPursuitAttribs();
                }
            }
        }

        if (!alreadyPursued) {
            Sim::IActivity *ipursuitActivity = Sim::IActivity::CreateInstance("AIPursuits", Sim::Param());
            IPursuit *ipursuit;
            ipursuitActivity->QueryInterface(&ipursuit);
            this->Attach(ipursuit);
            ISimable *playerSimable = player->GetSimable();
            IAttachable *targetattachable;
            if (playerSimable->QueryInterface(&targetattachable)) {
                targetattachable->Attach(ipursuit);
            }
            IPursuit *pursuit;
            if (ipursuitActivity->QueryInterface(&pursuit)) {
                pursuit->GetTarget()->Aquire(player->GetSimable());
                this->SpawnPursuitCar(pursuit);
            }
        } else if (!this->PlayerPursuitHasCop()) {
            this->SpawnPursuitCar(playerPursuit);
        }
    }
}

static void KillVehicle(IVehicle *ivehicle) {
    ISimable *simable;
    if (ivehicle->QueryInterface(&simable)) {
        simable->Kill();
    }
}

void AICopManager::ResetCopsForRestart(bool release) {
    this->mLockoutTimer = 60.0f;
    if (GRaceStatus::IsChallengeRace()) {
        this->mLockoutTimer = 0.0f;
    }

    if (this->mSpeech) {
        SoundAI *speech = SoundAI::Get();
        if (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming ||
            GRaceDatabase::Exists() && !GRaceDatabase::Get().GetStartupRace()) {
            Attrib::Gen::pursuitlevels pl(speech->GetPursuitSpecs());
            if (pl.IsValid()) {
                this->mLockoutTimer = pl.TimeInactiveFor911();
            }
        }
        speech->ResetPursuit(true);
        Speech::Manager::ClearPlayback();
        Speech::Module *module = Speech::Manager::GetSpeechModule(1);
        if (module) {
            module->ReleaseResource();
        }
    }

    Pursuits killPursuitList;
    RoadBlocks killRoadBlockList;

    for (RoadBlocks::iterator iter = this->mRoadBlockList.begin(); iter != this->mRoadBlockList.end(); ++iter) {
        IRoadBlock *iroadblock = *iter;
        killRoadBlockList.push_back(iroadblock);
    }

    for (RoadBlocks::iterator iter = killRoadBlockList.begin(); iter != killRoadBlockList.end(); ++iter) {
        IRoadBlock *iroadblock = *iter;
        iroadblock->ReleaseAllSmackables();
        Sim::IActivity *iactivity;
        if (iroadblock->QueryInterface(&iactivity)) {
            iactivity->Release();
        }
        this->Detach(iroadblock);
    }

    for (Pursuits::const_iterator iter = this->mIPursuitList.begin(); iter != this->mIPursuitList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        killPursuitList.push_back(ipursuit);
    }

    for (Pursuits::const_iterator iter = killPursuitList.begin(); iter != killPursuitList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        this->Detach(ipursuit);
        IAttachable *targetattachable;
        if (ipursuit->GetTarget() && ipursuit->GetTarget()->GetSimable()) {
            if (ipursuit->GetTarget()->GetSimable()->QueryInterface(&targetattachable)) {
                IPerpetrator *iperp;
                if (ipursuit->GetTarget()->GetSimable()->QueryInterface(&iperp)) {
                    iperp->ClearPendingRepPoints();
                }
                targetattachable->Detach(ipursuit);
            }
        }

        for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
            IVehicle *ivehicle = *iter;
            IPursuit *carspursuit = ivehicle->GetAIVehiclePtr()->GetPursuit();
            if (ComparePtr(carspursuit, ipursuit)) {
                ipursuit->RemoveVehicle(ivehicle);
            }
        }

        Sim::IActivity *iactivity;
        if (ipursuit->QueryInterface(&iactivity)) {
            iactivity->Release();
        }
    }

    for (IVehicle::List::const_iterator iter = this->mIVehicleList.begin(); iter != this->mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (ivehicle->IsActive()) {
            if (ivehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
                this->mNumActiveCopHelicopters--;
            } else {
                this->mNumActiveCopCars--;
            }
            ivehicle->GetAIVehiclePtr()->UnSpawn();
            MUnspawnCop(ivehicle->GetSimable()->GetOwnerHandle(), 0).Send("SoundAI");
        }
    }

    if (release) {
        std::for_each(this->mIVehicleList.begin(), this->mIVehicleList.end(), &KillVehicle);
        this->mIVehicleList.clear();
    }

    this->mNoNewPursuitsOrCops = false;
}

void AICopManager::LockoutCops(bool lockout) {
    if (lockout == true) {
        this->mLockoutTimer = 60.0f;
        if (this->mSpeech && (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming ||
                        GRaceDatabase::Exists() && !GRaceDatabase::Get().GetStartupRace())) {
            SoundAI *speech = SoundAI::Get();
            Attrib::Gen::pursuitlevels pl(speech->GetPursuitSpecs());
            if (pl.IsValid()) {
                this->mLockoutTimer = pl.TimeInactiveFor911();
            }
        }
    } else {
        this->mNoNewPursuitsOrCops = false;
        this->mLockoutTimer = 0.0f;
    }
}

void AICopManager::NoNewPursuitsOrCops() {
    this->mNoNewPursuitsOrCops = true;
}

bool AICopManager::OnTask(HSIMTASK htask, float dT) {
    ProfileNode profile_node("TODO", 0);
    this->mLockoutTimer -= dT;
    if (htask == this->mSimulateTask) {
        this->UpdateDebug();
        this->mPursuitRequestVehicle = nullptr;

        this->ApplyBreakerZones();
        this->UpdatePatrols();
        this->UpdatePursuits();
        this->UpdateRoadBlocks();
        this->UpdateSpawnRequests();

        this->mHeavySupportDelayTimer -= dT;
    }
    return true;
}

void AICopManager::OnDebugDraw() {}
