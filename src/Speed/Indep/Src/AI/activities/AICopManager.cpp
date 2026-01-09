#include "AICopManager.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/AI/AIPursuit.h"
#include "Speed/Indep/Src/AI/AIRoadBlock.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicleHelicopter.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pursuitsupport.h"
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
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Sim/UTil.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

#include <cfloat>

// TODO
extern int SkipFE;
extern int SkipFEMaxCops;
extern int SkipFEDisableCops;
extern int SkipFEHelicopter;

int ICopMgr::mDisableCops = false;

float AICopManager::mCopMinSpawnDist = 150.0f;
float AICopManager::mCopMaxSpawnDist = 400.0f;

AICopManager *TheOneCopManager = nullptr;

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
    mSpeech = UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance("SoundAI", Sim::Param());
    // TODO is the if check in Attach?
    if (mSpeech) {
        Attach(mSpeech);
    }
    mMessSetAutoSpawn =
        Hermes::Handler::Create<MSetCopAutoSpawnMode, AICopManager, AICopManager>(this, &AICopManager::MessageSetAutoSpawnMode, "AICopManager", 0);
    mMessSetCopsEnabled =
        Hermes::Handler::Create<MSetCopsEnabled, AICopManager, AICopManager>(this, &AICopManager::MessageSetCopsEnabled, "AICopManager", 0);
    mMessBreakerStopCops =
        Hermes::Handler::Create<MBreakerStopCops, AICopManager, AICopManager>(this, &AICopManager::MessageBreakerStopCops, "AICopManager", 0);
    mMessForcePursuitStart =
        Hermes::Handler::Create<MForcePursuitStart, AICopManager, AICopManager>(this, &AICopManager::MessageForcePursuitStart, "AICopManager", 0);

    mIVehicleList.clear();
    mIPursuitList.clear();
    mIVehicleList.reserve(10);

    mSimulateTask = AddTask("AICopManager", 0.5f, 0.0f, Sim::TASK_FRAME_FIXED);
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
        "PVehicle", VehicleParams(this, DRIVER_COP, Attrib::StringToKey(name), initialVec, initialPos, 0, nullptr, 0));
    if (isimable) {
        Attach(isimable);
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
        MUnspawnCop(bestChoice->GetSimable()->GetOwnerHandle(), 5).Send("SoundAI");
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
    IActivity *newinstance = UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance("AIPursuit", Sim::Param());
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

// UNSOLVED the multiplication by M_TWOPI is at the wrong place
UMath::Vector3 rand_point_in_circle() {
    float angle = Sim::GetRandom()._SimRandom_FloatRange(M_TWOPI);
    float radius = UMath::Sqrt(Sim::GetRandom()._SimRandom_FloatRange(1.0f));
    angle *= (float)M_TWOPI;

    UMath::Vector3 r;
    r.x = UMath::Sinr(angle) * radius;
    r.y = 0.0f;
    r.z = UMath::Cosr(angle) * radius;

    return r;
}

bool AICopManager::SpawnPursuitCarByName(IPursuit *ipursuit, const char *name) {
    UCrc32 nameHash(name);
    if (nameHash == "copheli") {
        return SpawnPursuitHelicopter(ipursuit);
    }
    IVehicle *availableCopCar = GetAvailableCopVehicleByName(name);
    bool rv = false;
    if (availableCopCar) {
        rv = SpawnPursuitIVehicle(ipursuit, availableCopCar);
    }
    return rv;
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
    mNumActiveCopCars++;
}

bool AICopManager::SpawnCopCarNow(IPursuit *ipursuit) {
    IVehicle *availableCopCar = GetAvailableCopVehicleByClass(VehicleClass::CAR, false);
    if (availableCopCar) {
        SpawnVehicleBehindTarget(ipursuit, availableCopCar);
        return true;
    }
    return false;
}

bool AICopManager::SpawnPursuitHelicopter(IPursuit *ipursuit) {
    if (HeliVehicleActive()) {
        return false;
    }
    IVehicle *availableCopHelicopter = GetAvailableCopVehicleByClass(VehicleClass::CHOPPER, false);
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
        mNumActiveCopHelicopters++;
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
        mNumActiveCopHelicopters--;
    } else {
        mNumActiveCopCars--;
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
        IVehicle *ivehicle = GetAvailableCopVehicleByName(vname);
        if (ivehicle) {
            gsr->mIVehicleList.push_back(ivehicle);
            IPursuitAI *ipv;
            if (ivehicle->QueryInterface(&ipv)) {
                ipv->SetSupportGoal(vgoals[index]);
            }
            if (gsr->mHeavySupport->HeavyStrategy == RAM) {
                mHeavySupportDelayTimer = 3.0f;
                if (index == numVehicles - 1) {
                    MReqBackup(16).Send("Request"); // TODO 16? what does that mean?
                }
            }
        } else {
            break;
        }
    }
    return mHeavySupportDelayTimer < 0.0f && index == numVehicles;
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
            if (GetSpawnPositionAheadOfTarget(ipursuit, UMath::Vector4To3(spawnPosition[cindex]), UMath::Vector4To3(spawnInitialVec[cindex]),
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
            mNumActiveCopCars++;

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
        IVehicle *ivehicle = GetAvailableCopVehicleByName(vname[index]);
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
            if (GetSpawnPositionAheadOfTarget(ipursuit, UMath::Vector4To3(spawnPosition[cindex]), UMath::Vector4To3(spawnInitialVec[cindex]),
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
            mNumActiveCopCars++;

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
    if (gsr == nullptr || gsr->mSupportRequestStatus != GroundSupportRequest::PENDING) {
        return;
    }
    if (gsr->mHeavySupport) {
        if (GetHeavySupportVehicles(gsr)) {
            return;
        }
        if (gsr->mHeavySupport->HeavyStrategy == HEAVY_ROADBLOCK) {
            if (!ipursuit->GetRoadBlock()) {
                if (CreateRoadBlock(ipursuit, 4, nullptr, &gsr->mIVehicleList)) {
                    gsr->mSupportRequestStatus = GroundSupportRequest::ACTIVE;
                    gsr->mSupportTimer = gsr->mHeavySupport->Duration;
                    MReqRoadBlock(1).Send("Created");
                } else {
                    MReqRoadBlock(0).Send("Created");
                }
            }
        } else {
            if (!StartHeavySupport(ipursuit, gsr)) {
                MReqBackup(16).Send("ReqDenied");
            }
        }
    } else if (GetLeaderSupportVehicles(gsr)) {
        if (StartLeaderSupport(ipursuit, gsr)) {
            MReqBackup(64).Send("Request");
        } else {
            MReqBackup(64).Send("ReqDenied");
        }
    }
}
