#include "AICopManager.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
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
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// TODO
extern int SkipFE;
extern int SkipFEMaxCops;
extern int SkipFEDisableCops;
extern int SkipFEHelicopter;

AICopManager *TheOneCopManager;

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
    MakeDebugable(DBG_AI);
    mSpeech = UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(UCrc32("SoundAI"), Sim::Param());
    // TODO is the if check in Attach?
    if (mSpeech) {
        Attach(mSpeech);
    }
    mMessSetAutoSpawn = Hermes::Handler::Create<MSetCopAutoSpawnMode, AICopManager, AICopManager>(this, &AICopManager::MessageSetAutoSpawnMode,
                                                                                                  UCrc32("AICopManager"), 0);
    mMessSetCopsEnabled =
        Hermes::Handler::Create<MSetCopsEnabled, AICopManager, AICopManager>(this, &AICopManager::MessageSetCopsEnabled, UCrc32("AICopManager"), 0);
    mMessBreakerStopCops =
        Hermes::Handler::Create<MBreakerStopCops, AICopManager, AICopManager>(this, &AICopManager::MessageBreakerStopCops, UCrc32("AICopManager"), 0);
    mMessForcePursuitStart = Hermes::Handler::Create<MForcePursuitStart, AICopManager, AICopManager>(this, &AICopManager::MessageForcePursuitStart,
                                                                                                     UCrc32("AICopManager"), 0);

    mIVehicleList.clear();
    mIPursuitList.clear();
    mIVehicleList.reserve(10);

    mSimulateTask = AddTask(UCrc32("AICopManager"), 0.5f, 0.0f, Sim::TASK_FRAME_FIXED);
    Sim::ProfileTask(mSimulateTask, "AICopManager");

    mMaxCopCars = UMath::Min(mMaxCopCars, mPlatformBudgetCopCars);

    if (SkipFEMaxCops > 0 || SkipFEHelicopter > 0) {
        mMaxActiveCopCars = UMath::Min(mMaxCopCars, SkipFEMaxCops);
        mMaxActiveCopHelicopters = UMath::Min(mMaxCopHelicopters, SkipFEHelicopter);
    }

    ICopMgr::mDisableCops = false;
    if (SkipFE && SkipFEDisableCops) {
        ICopMgr::mDisableCops = true;
    }

    mActionQ = new ActionQueue(0, 0x98c7a2f5, "AICopManager", false); // TODO magic
    mAttributes = nullptr;
    TheOneCopManager = this;
}

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

// Functionally matching, reserve shouldn't get inlined for some reason though
void AICopManager::OnAttached(IAttachable *pOther) {
    IRoadBlock *iroadblock;
    IPursuit *ipursuit;
    IVehicle *ivehicle;

    if (pOther->QueryInterface(&ivehicle)) {
        mIVehicleList.push_back(ivehicle);
        if (mSpeech) {
            mSpeech->Attach(ivehicle);
        }
    } else {
        if (pOther->QueryInterface(&ipursuit)) {
            mIPursuitList.push_back(ipursuit);
        } else {
            if (pOther->QueryInterface(&iroadblock)) {
                mRoadBlockList.push_back(iroadblock);
            } else {
                return;
            }
        }
    }
    Activity::OnAttached(pOther);
}

void AICopManager::OnDetached(IAttachable *pOther) {
    IRoadBlock *iroadblock;
    IPursuit *ipursuit;
    IVehicle *ivehicle;

    if (pOther->QueryInterface(&ivehicle)) {
        IVehicle::List::iterator iter = std::find(mIVehicleList.begin(), mIVehicleList.end(), ivehicle);
        if (iter != mIVehicleList.end()) {
            mIVehicleList.erase(iter);
        }
        if (mSpeech) {
            mSpeech->Detach(ivehicle);
        }
    } else {
        if (pOther->QueryInterface(&ipursuit)) {
            mIPursuitList.erase(std::find(mIPursuitList.begin(), mIPursuitList.end(), ipursuit));
        } else {
            if (pOther->QueryInterface(&iroadblock)) {
                mRoadBlockList.erase(std::find(mRoadBlockList.begin(), mRoadBlockList.end(), iroadblock));
            }
        }
    }
    if (UTL::COM::ComparePtr(pOther, mSpeech)) {
        mSpeech = nullptr;
    }
    Activity::OnDetached(pOther);
}

bool AICopManager::VehicleSpawningEnabled(bool isdespawn) {
    if (mNoNewPursuitsOrCops && !isdespawn) {
        return false;
    }
    if (mLockoutTimer > 0.0f || mDisableCops) {
        return false;
    }
    if (INIS::Exists() && (!isdespawn || !INIS::Get()->IsWorldMomement())) {
        return false;
    }
    return true;
}

IVehicle *AICopManager::GetAvailableCopVehicleByClass(UCrc32 vehicleClass, bool bValidOnesOnly) {
    if (!VehicleSpawningEnabled(false)) {
        return nullptr;
    }
    IPerpetrator *perp = nullptr;
    ISimable *simable = IPlayer::First(PLAYER_LOCAL)->GetSimable();
    if (simable) {
        simable->QueryInterface(&perp);
    }
    const char *name = nullptr;
    for (IVehicle::List::const_iterator iter = mIVehicleList.begin(); iter != mIVehicleList.end(); ++iter) {
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
    return GetAvailableCopVehicleByName(name);
}

IVehicle *AICopManager::GetAvailableCopVehicleByName(const char *name) {
    if (!VehicleSpawningEnabled(false)) {
        return nullptr;
    }
    if (!name) {
        return nullptr;
    }
    UCrc32 nameHash = UCrc32(name);
    for (IVehicle::List::const_iterator iter = mIVehicleList.begin(); iter != mIVehicleList.end(); ++iter) {
        IVehicle *ivehicle = *iter;
        if (!ivehicle->IsActive() && !IsPendingSupportVehicle(ivehicle)) {
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
    ISimable *isimable = UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::CreateInstance(
        UCrc32("PVehicle"), VehicleParams(this, DRIVER_COP, Attrib::StringToKey(name), initialVec, initialPos, 0, nullptr, 0));
    if (isimable) {
        Attach(isimable);
        IVehicle *ivehicle;
        if (isimable->QueryInterface(&ivehicle)) {
            ivehicle->GetAIVehiclePtr()->UnSpawn();
            MUnspawnCop(isimable->GetOwnerHandle(), 4).Send(UCrc32("SoundAI"));
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
    if (!VehicleSpawningEnabled(false)) {
        return nullptr;
    }
    IVehicle *bestChoice = nullptr;
    float bestScore = 0.0f;
    for (IVehicle::List::const_iterator iter = mIVehicleList.begin(); iter != mIVehicleList.end(); ++iter) {
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
        RemoveActiveCopVehicle(bestChoice);
        MUnspawnCop(bestChoice->GetSimable()->GetOwnerHandle(), 5).Send(UCrc32("SoundAI"));
    }
    return bestChoice;
}

IPursuit *AICopManager::GetPursuitActivity(ISimable *itargetSimable) {
    for (Pursuits::const_iterator iter = mIPursuitList.begin(); iter != mIPursuitList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        AITarget *pursuitTarget = ipursuit->GetTarget();
        if (pursuitTarget && UTL::COM::ComparePtr(pursuitTarget->GetSimable(), itargetSimable)) {
            return ipursuit;
        }
    }

    IPursuit *ipursuitActivity;
    IActivity *newinstance = UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance(UCrc32("AIPursuit"), Sim::Param());
    newinstance->QueryInterface(&ipursuitActivity);
    Attach(ipursuitActivity);
    IAttachable *targetattachable;
    if (itargetSimable->QueryInterface(&targetattachable)) {
        targetattachable->Attach(newinstance);
    }

    return ipursuitActivity;
}

void AICopManager::SpawnCop(UMath::Vector3 &InitialPos, UMath::Vector3 &InitialVec, const char *VehicleName, bool InPursuit, bool RoadBlock) {
    mSpawnRequests.push_back(SpawnCopRequest(InitialPos, InitialVec, VehicleName, InPursuit, RoadBlock));
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
    UpdateCopPriorities(mNumActiveCopCars + 1);

    IVehicle *availableCopCar = GetAvailableCopVehicleByName(vehicleName);
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
        IPursuit *ipursuitActivity = GetPursuitActivity(targetSimable);

        ipv->StartPursuit(nullptr, targetSimable);
        ipursuitActivity->AddVehicle(availableCopCar);
    } else if (request.InRoadBlock) {
        ipv->StartRoadBlock();
    } else {
        ipv->StartPatrol();
    }
    mNumActiveCopCars++;
    return true;
}

bool AICopManager::IsCopSpawnPending() const {
    return mSpawnRequests.size() != 0;
}

void AICopManager::UpdateSpawnRequests() {
    for (SpawnList::iterator i = mSpawnRequests.begin(); i != mSpawnRequests.end();) {
        if (TrySpawnCop(*i)) {
            SpawnList::iterator j = i++;
            mSpawnRequests.erase(j);
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
    PursueAtHeatLevel(message.GetMinHeatLevel());
}

void AICopManager::SetAllBustedTimersToZero() {
    for (Pursuits::const_iterator iter = mIPursuitList.begin(); iter != mIPursuitList.end(); ++iter) {
        IPursuit *ipursuit = *iter;
        ipursuit->SetBustedTimerToZero();
    }
}

void AICopManager::MessageBreakerStopCops(const MBreakerStopCops &message) {
    mBreakerZones.push_back(BreakerZone(message.GetInitialPos(), message.GetRadius(), Sim::GetTime() + message.GetDuration()));
}

void AICopManager::ApplyBreakerZones() {
    if (mBreakerZones.empty()) {
        return;
    }
    for (IVehicle::List::const_iterator iter = mIVehicleList.begin(); iter != mIVehicleList.end(); ++iter) {
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
        for (BreakerList::const_iterator b = mBreakerZones.begin(); b != mBreakerZones.end(); ++b) {
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
    for (BreakerList::iterator b = mBreakerZones.begin(); b != mBreakerZones.end();) {
        BreakerList::iterator t = b++;
        if (t->endtime < Sim::GetTime()) {
            mBreakerZones.erase(t);
        }
    }
}

bool AICopManager::SpawnPatrolCar() {
    IVehicle *availableCopCar = GetAvailableCopVehicleByClass(VehicleClass::CAR, true);
    if (!availableCopCar) {
        return false;
    }
    short segInd = 0;
    char laneInd = 0;
    float timeStep = 0.0f;
    if (GetSpawnLocation(segInd, laneInd, timeStep)) {
        IVehicleAI *ivehicleAI = availableCopCar->GetAIVehiclePtr();
        IPursuitAI *ipv;
        if (ivehicleAI->QueryInterface(&ipv)) {
            availableCopCar->Activate();
            ivehicleAI->ResetVehicleToRoadNav(segInd, laneInd, timeStep);
            ivehicleAI->SetSpawned();
            ipv->StartPatrol();
            mNumActiveCopCars++;
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
            if (CheckSpawnPosition(pos, false, 0, 0, checkDist)) {
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
    IVehicle *availableCopCar = GetAvailableCopVehicleByClass(VehicleClass::CAR, true);
    const char *name;
    float distance;
    if (!GManager::Get().GetIsWarping() && !GManager::Get().GetStartFreeRoamPursuit()) {
        distance = mCopMaxSpawnDist - 60.0f;
    } else {
        distance = 80.0f;
    }
    if (!availableCopCar) {
        availableCopCar = GetActiveCopVehicleFromOutOfView(VehicleClass::CAR);
        if (!availableCopCar) {
            return false;
        }
    }
    UMath::Vector3 spawnPosition;
    UMath::Vector3 spawnInitialVec;
    if (GetSpawnPositionAheadOfTarget(ipursuit, spawnPosition, spawnInitialVec, distance)) {
        availableCopCar->Activate();
        IVehicleAI *ivehicleAI = availableCopCar->GetAIVehiclePtr();
        ivehicleAI->ResetVehicleToRoadPos(spawnPosition, spawnInitialVec);
        ivehicleAI->SetSpawned();

        IPursuitAI *ipai;
        if (ivehicleAI->QueryInterface(&ipai)) {
            ipai->ZeroTimeSinceTargetSeen();
        }
        ipursuit->AddVehicle(availableCopCar);
        mNumActiveCopCars++;
        return true;
    }
    AITarget *pursuitTarget = ipursuit->GetTarget();
    if (pursuitTarget) {
        SpawnCopCarNow(ipursuit);
    }

    return false;
}
