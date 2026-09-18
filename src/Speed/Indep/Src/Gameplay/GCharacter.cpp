#include "GCharacter.h"

#include "GManager.h"
#include "GMarker.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

// Los diez stores del objetivo salen en ORDEN DE DECLARACION y TODOS con la
// misma linea de fuente (GCharacter.cpp:50, la del cuerpo): eso es la lista de
// inicializacion, no el cuerpo. Ademas asi CSE se queda las tres palabras de
// UMath::Vector3::kZero en registro para las cuatro copias, en vez de
// recargarlas (nueve `lwz` de mas).
GCharacter::GCharacter(const Attrib::Key &triggerKey)
    : GRuntimeInstance(triggerKey, kGameplayObjType_Character), UTL::COM::Object(1), IAttachable(this), //
      mSpawnPos(UMath::Vector3::kZero),                                                                 //
      mState(kCharState_Unspawned),                                                                     //
      mFlags(0),                                                                                        //
      mCreateAttemptsMade(0),                                                                           //
      mSpawnDir(UMath::Vector3::kZero),                                                                 //
      mSpawnSpeed(0.0f),                                                                                //
      mTargetPos(UMath::Vector3::kZero),                                                                //
      mVehicle(nullptr),                                                                                //
      mTargetDir(UMath::Vector3::kZero),                                                                //
      mAttachments(new Sim::Attachments(this)) {
}

GCharacter::~GCharacter() {
    if (mAttachments != nullptr) {
        delete mAttachments;
    }
}

void GCharacter::OnAttached(IAttachable *pOther) {
    IVehicle *vehicle = nullptr;
    if (pOther->QueryInterface(&vehicle)) {
        mVehicle = vehicle;
    }
}

void GCharacter::OnDetached(IAttachable *pOther) {
    IVehicle *vehicle = nullptr;
    if (!pOther->QueryInterface(&vehicle)) {
        return;
    }

    ISimable *simable = nullptr;
    mVehicle->QueryInterface(&simable);

    IVehicleAI *vehicleAI = nullptr;
    if (simable->QueryInterface(&vehicleAI)) {
        WRoadNav *roadNav = vehicleAI->GetDriveToNav();
        if (roadNav != nullptr) {
            roadNav->CancelPathFinding();
        }
    }

    if (IsFlagSet(kCharFlag_UsingStockCar)) {
        mVehicle->Deactivate();
        GManager::Get().ReleaseStockCar(simable);
        ClearFlag(kCharFlag_UsingStockCar);
    }

    mVehicle = nullptr;
    Unspawn();
}

void GCharacter::Spawn(const UMath::Vector3 &pos, const UMath::Vector3 &dir, GMarker *targetPoint, float initialSpeed) {
    Unspawn();

    mSpawnPos = pos;
    mSpawnDir = dir;
    mSpawnSpeed = MPH2MPS(initialSpeed);
    mTargetPos = targetPoint->GetPosition();
    mTargetDir = targetPoint->GetDirection();
    mCreateAttemptsMade = 0;
    AttemptSpawn();
    GManager::Get().AttachCharacter(this);
    SetFlag(kCharFlag_AttachedToManager);
}

bool GCharacter::SpawnPending() const {
    return mState == kCharState_Spawning_WaitingForModel || mState == kCharState_Spawning_WaitingForTrack;
}

bool GCharacter::IsSpawned() const {
    return mState == kCharState_Spawned || mState == kCharState_Unspawning_WaitingUntilOffscreen;
}

void GCharacter::ReleaseVehicle() {
    if (mVehicle == nullptr) {
        return;
    }

    ISimable *simable = nullptr;
    if (IsFlagClear(kCharFlag_UsingStockCar)) {
        mVehicle->QueryInterface(&simable);
    }

    Detach(mVehicle);

    if (simable != nullptr) {
        simable->Kill();
    }
}

void GCharacter::Unspawn() {
    if (IsFlagSet(kCharFlag_AttachedToManager)) {
        GManager::Get().DetachCharacter(this);
        ClearFlag(kCharFlag_AttachedToManager);
    }
    if (mVehicle != nullptr) {
        ReleaseVehicle();
    }
    mState = kCharState_Unspawned;
}

void GCharacter::UnspawnWhenOffscreen() {
    switch (mState) {
    case kCharState_Invalid:
    case kCharState_Unspawned:
        break;
    case kCharState_Spawning_WaitingForModel:
    case kCharState_Spawning_WaitingForTrack:
        Unspawn();
        break;
    case kCharState_Spawned:
    case kCharState_Unspawning_WaitingUntilOffscreen:
        if (mVehicle->GetOffscreenTime() > 0.0f) {
            Unspawn();
        }
        mState = kCharState_Unspawning_WaitingUntilOffscreen;
        break;
    }
}

bool GCharacter::IsNoLongerUseful() const {
    const float kUselessOffScreenTime = 0.5f;
    const float kUselessDistance = 100.0f;

    if (!IsSpawned()) {
        return false;
    }
    if (mVehicle->GetOffscreenTime() < kUselessOffScreenTime) {
        return false;
    }

    IVehicle *racerVehicle = IVehicle::First(VEHICLE_RACERS);
    for (int racerIdx = 0; racerIdx < IVehicle::Count(VEHICLE_RACERS); racerVehicle = racerVehicle->Next(VEHICLE_RACERS), racerIdx++) {
        ISimable *racerSimable = racerVehicle->GetSimable();
        if (racerVehicle->GetDriverClass() == DRIVER_HUMAN && racerSimable != nullptr) {
            IRigidBody *rigidBody = racerSimable->GetRigidBody();
            if (rigidBody != nullptr) {
                UMath::Vector3 charPos = mVehicle->GetPosition();
                UMath::Vector3 dirToChar;
                UMath::Vector3 humanPos = racerVehicle->GetPosition();
                UMath::Vector3 humanForward;
                float distToChar;

                UMath::Sub(charPos, humanPos, dirToChar);
                distToChar = UMath::Normalize(dirToChar);
                if (distToChar < kUselessDistance) {
                    return false;
                }
                rigidBody->GetForwardVector(humanForward);
                if (!(UMath::Dot(dirToChar, humanForward) < 0.0f)) {
                    return false;
                }
            }
        }
    }

    return true;
}

bool GCharacter::AttemptSpawn() {
    if (mState == kCharState_Unspawned) {
        const char *carName = CarType();
        const char *lowMemName = CarTypeLowMem();
        if (lowMemName != nullptr && lowMemName[0] != 0) {
            carName = lowMemName;
        }

        bool isCop = bStrCmp(carName, "copmidsize") == 0;
        DriverClass driverClass = DRIVER_TRAFFIC;
        if (isCop) {
            driverClass = DRIVER_NONE;
        }

        bool spawn_ok = true;
        if (SkipFE) {
            spawn_ok = isCop ? (SkipFEDisableCops == 0) : (SkipFEDisableTraffic == 0);
        }
        if (spawn_ok) {

        ISimable *simable = GManager::Get().GetStockCar(carName);
        if (simable == nullptr) {
            IVehicleCache *cache = &GManager::Get();
            VehicleParams params(cache, driverClass, Attrib::StringToKey(carName), mSpawnDir, mSpawnPos, 0, nullptr, nullptr);
            simable = ISimable::CreateInstance("PVehicle", params);
            if (simable == nullptr) {
                simable = GManager::Get().GetRandomEmergencyStockCar();
                if (simable == nullptr) {
                    goto next_state;
                }
                SetFlag(kCharFlag_UsingStockCar);
            }
        } else {
            SetFlag(kCharFlag_UsingStockCar);
        }

        Attach(simable);

        mVehicle->SetDriverClass(driverClass);

        mState = kCharState_Spawning_WaitingForModel;
        }
    }

next_state:
    if (mState == kCharState_Spawning_WaitingForModel) {
        if (!mVehicle->IsLoading() || AllowInvisibleSpawn()) {
            mState = kCharState_Spawning_WaitingForTrack;
        }
    }

    if (mState == kCharState_Spawning_WaitingForTrack) {
        float height = 0.0f;
        if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(mSpawnPos, height, nullptr) == 0) {
            goto done;
        }

        ISimable *simable = nullptr;
        if (!mVehicle->QueryInterface(&simable)) {
            goto done;
        }

        IVehicleAI *vehicleAI = nullptr;
        if (simable->QueryInterface(&vehicleAI)) {
            AITarget *target = vehicleAI->GetTarget();
            target->Aquire(mTargetPos, mTargetDir);
            WRoadNav *roadNav = vehicleAI->GetDriveToNav();
            roadNav->DetermineVehicleHalfWidth();
            roadNav->ResetCookieTrail();
        }

        mVehicle->Activate();
        mVehicle->SetVehicleOnGround(mSpawnPos, mSpawnDir);

        if (mSpawnSpeed > 0.0f) {
            mVehicle->SetSpeed(mSpawnSpeed);
            MSetTrafficSpeed message(mSpawnSpeed * 2.23699f, mSpawnSpeed * 2.23699f, 1);
            message.SetID(simable->GetWorldID());
            message.Post(UCrc32("AIAction"));
        }

        ITrafficAI *trafficAI = nullptr;
        if (simable->QueryInterface(&trafficAI)) {
            trafficAI->StartDriving(mSpawnSpeed);
        }

        mState = kCharState_Spawned;
    }

done:
    return mState == kCharState_Spawned;
}

IVehicle *GCharacter::GetSpawnedVehicle() const {
    return mVehicle;
}

unsigned int GCharacter::GetName() const {
    const char *name = RacerName();
    if (name == nullptr || bStrLen(name) == 0) {
        return 0;
    }

    return bStringHash(name);
}
