#include "GCharacter.h"

#include "GManager.h"
#include "GMarker.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/Generated/Messages/MSetTrafficSpeed.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern int SkipFE;
extern int SkipFEDisableTraffic;
extern int SkipFEDisableCops;

void GCharacter::OnAttached(IAttachable *pOther) {
    IVehicle *vehicle;

    if (pOther->QueryInterface(&vehicle)) {
        mVehicle = vehicle;
    }
}

void GCharacter::OnDetached(IAttachable *pOther) {
    IVehicle *vehicle;

    if (pOther->QueryInterface(&vehicle) && mVehicle == vehicle) {
        mVehicle = nullptr;
    }
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
    return mState - kCharState_Spawning_WaitingForModel < 2;
}

bool GCharacter::AttemptSpawn() {
    if (mState == kCharState_Unspawned) {
        const char *carType = CarType(0);
        const char *carTypeLowMem = CarTypeLowMem(0);
        bool isCop = bStrCmp(carType, "copmidsize") == 0;
        DriverClass driverClass = isCop ? DRIVER_COP : DRIVER_TRAFFIC;
        bool spawn_ok = true;

        if (carTypeLowMem && *carTypeLowMem) {
            carType = carTypeLowMem;
        }

        if (SkipFE) {
            int disable = SkipFEDisableTraffic;

            if (isCop) {
                disable = SkipFEDisableCops;
            }

            spawn_ok = disable == 0;
        }

        if (spawn_ok) {
            ISimable *isimable = GManager::Get().GetStockCar(carType);

            if (!isimable) {
                VehicleParams params(&GManager::Get(), driverClass, Attrib::StringToKey(carType), mSpawnDir, mSpawnPos, 0, nullptr, 0);
                isimable = UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::CreateInstance("PVehicle", params);

                if (!isimable) {
                    isimable = GManager::Get().GetRandomEmergencyStockCar();
                    if (!isimable) {
                        return false;
                    }
                    SetFlag(kCharFlag_UsingStockCar);
                }
            } else {
                SetFlag(kCharFlag_UsingStockCar);
            }

            mAttachments->Attach(isimable);
            mVehicle->SetDriverClass(driverClass);
            mState = kCharState_Spawning_WaitingForModel;
        }
    }

    if (mState == kCharState_Spawning_WaitingForModel) {
        if (!mVehicle->IsLoading() || AllowInvisibleSpawn(0)) {
            mState = kCharState_Spawning_WaitingForTrack;
        }
    }

    if (mState == kCharState_Spawning_WaitingForTrack) {
        float worldHeight = 0.0f;
        WCollisionMgr collisionMgr(0, 3);

        if (collisionMgr.GetWorldHeightAtPointRigorous(mSpawnPos, worldHeight, nullptr)) {
            ISimable *isimable;

            if (mVehicle->QueryInterface(&isimable)) {
                IVehicleAI *vehicleAI;

                if (isimable->QueryInterface(&vehicleAI)) {
                    AITarget *target = vehicleAI->GetTarget();
                    WRoadNav *road_nav = vehicleAI->GetCurrentRoad();

                    if (target) {
                        target->Aquire(mTargetPos, mTargetDir);
                    }

                    if (road_nav) {
                        road_nav->ResetCookieTrail();
                    }

                    vehicleAI->SetSpawned();
                    vehicleAI->ResetVehicleToRoadPos(mSpawnPos, mSpawnDir);

                    if (0.0f < mSpawnSpeed) {
                        ITrafficAI *itv;
                        float speedMph;

                        mVehicle->Activate();
                        speedMph = MPS2MPH(mSpawnSpeed);
                        MSetTrafficSpeed msg(speedMph, speedMph, true);
                        msg.SetID(isimable->GetWorldID());
                        msg.Post("AIAction");

                        if (isimable->QueryInterface(&itv)) {
                            itv->StartDriving(mSpawnSpeed);
                        }
                    }
                }

                mState = kCharState_Spawned;
            }
        }
    }

    return mState == kCharState_Spawned;
}
