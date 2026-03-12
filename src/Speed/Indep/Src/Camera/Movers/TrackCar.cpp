#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

static float TrackCarEyeOffsetZ[4] = {0.1f, 3.0f, 3.5f, 2.0f};

static bool IsAnyCopNear(CameraAnchor *pCar) {
    IVehicle *const *iter = IVehicle::GetList(VEHICLE_AICOPS).begin();

    while (iter != IVehicle::GetList(VEHICLE_AICOPS).end()) {
        IVehicle *p_car = *iter;
        if (p_car != nullptr && p_car->IsActive()) {
            UVector3 ucoppos(p_car->GetPosition());
            bVector3 coppos;
            bVector3 copdir;
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(ucoppos), coppos);
            bSub(&copdir, &coppos, pCar->GetGeometryPosition());
            float copdist = bLength(&copdir);
            if (copdist < 30.0f) {
                return true;
            }
        }
        ++iter;
    }
    return false;
}

static bool IsBeingPursued(int nView) {
    IPerpetrator *iperp;
    const IPlayer::List &playerList = IPlayer::GetList(PLAYER_LOCAL);

    IPlayer *const *iter = playerList.begin();

    while (iter != playerList.end()) {
        IPlayer *ip = *iter;
        if (ip->GetControllerPort() == nView) {
            ISimable *simable = ip->GetSimable();
            if (simable != nullptr) {
                simable->QueryInterface(&iperp);
                if (iperp != nullptr) {
                    if (!iperp->IsBeingPursued()) {
                        return false;
                    }
                    return true;
                }
            }
            return false;
        }
        ++iter;
    }
    return false;
}

static void FixWorldHeight(UMath::Vector3 *point, int type) {
    if (!IsGameFlowInGame()) {
        return;
    }

    UMath::Vector3 test;
    float ground_elevation = 0.0f;

    test = *point;
    test.y += 4.0f;

    WCollisionMgr collision_mgr(0, 3);

    if (collision_mgr.GetWorldHeightAtPointRigorous(test, ground_elevation, nullptr)) {
        if (type == 3) {
            point->y = ground_elevation;
        }
    }

    if (ground_elevation > point->y) {
        point->y = ground_elevation;
    }

    point->y += TrackCarEyeOffsetZ[type];
}

// TEMP: force emission for testing
volatile void *_force_active_TrackCar[] = {
    (void *)IsAnyCopNear,
    (void *)IsBeingPursued,
    (void *)FixWorldHeight,
};