#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

extern float TrackCarEyeOffsetZ[];
extern bool gCamCloseToRoadBlock;
static bVector3 PreviousEye;
static const float Tweak_JumpCamPositionSpeedMult[4] = {0.5f, 1.3f, 1.0f, 1.5f};

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
    const IPlayer::List &playerList = IPlayer::GetList(PLAYER_ALL);

    IPlayer *const *iter = playerList.begin();

    while (iter != playerList.end()) {
        IPlayer *ip = *iter;
        if (ip->GetRenderPort() == nView) {
            ISimable *simable = ip->GetSimable();
            if (simable != nullptr) {
                simable->QueryInterface(&iperp);
                if (iperp != nullptr) {
                    if (iperp->IsBeingPursued()) {
                        return true;
                    }
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
    float ground_elevation;

    test = *point;
    ground_elevation = 0.0f;
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

void TrackCarCameraMover::Init() {
    WRoadNav nav;
    nav.SetCookieTrail(true);
    nav.SetNavType(WRoadNav::kTypeDirection);

    if (gCamCloseToRoadBlock) {
        gCamCloseToRoadBlock = false;
        CameraType = 2;
    } else {
        if (IsBeingPursued(ViewID)) {
            if (IsAnyCopNear(CarToFollow)) {
                CameraType = 1;
            }
        }
    }

    float positionSpeedMultiplier[2];
    const float kMaxPositionDistance = 150.0f;
    positionSpeedMultiplier[0] = Tweak_JumpCamPositionSpeedMult[CameraType];
    positionSpeedMultiplier[1] = Tweak_JumpCamPositionSpeedMult[3];
    float vel = CarToFollow->GetVelocityMagnitude();
    if (vel * positionSpeedMultiplier[0] > kMaxPositionDistance) {
        positionSpeedMultiplier[0] = kMaxPositionDistance / vel;
    }
    if (vel * positionSpeedMultiplier[1] > kMaxPositionDistance) {
        positionSpeedMultiplier[1] = kMaxPositionDistance / vel;
    }

    bVector3 vCarPosQuarterTime;
    bScaleAdd(&vCarPosQuarterTime, CarToFollow->GetGeometryPosition(), CarToFollow->GetVelocity(), 0.1f);

    UMath::Vector4 carPosQuarterTime;
    eUnSwizzleWorldVector(vCarPosQuarterTime, reinterpret_cast<bVector3 &>(carPosQuarterTime));

    bVector3 vFuture;
    bScaleAdd(&vFuture, CarToFollow->GetGeometryPosition(), CarToFollow->GetVelocity(), positionSpeedMultiplier[0]);

    bVector3 vLeft(vFuture);
    bVector3 vRight(vFuture);

    bVector3 vFutureHigh;
    bScaleAdd(&vFutureHigh, CarToFollow->GetGeometryPosition(), CarToFollow->GetVelocity(), positionSpeedMultiplier[1]);

    bVector3 vLeftHigh(vFutureHigh);
    bVector3 vRightHigh(vFutureHigh);

    UMath::Vector4 carPos;
    eUnSwizzleWorldVector(*CarToFollow->GetGeometryPosition(), reinterpret_cast<bVector3 &>(carPos));

    UMath::Vector4 carDir;
    eUnSwizzleWorldVector(*CarToFollow->GetForwardVector(), reinterpret_cast<bVector3 &>(carDir));

    float rightDistHigh = 0.0f;
    float leftDistHigh = rightDistHigh;
    float rightDist = rightDistHigh;
    float leftDist = rightDistHigh;

    nav.InitAtPoint(Vector4To3(carPos), Vector4To3(carDir), true, 1.0f);

    if (nav.IsValid()) {
        const float kMaxNavDist = 10.0f;

        nav.IncNavPosition(positionSpeedMultiplier[0] * CarToFollow->GetVelocityMagnitude(), UMath::Vector3::kZero, rightDistHigh);

        float navDist = UMath::Distancexz(nav.GetLeftPosition(), nav.GetPosition());
        float sideLerp = 0.9f;
        if (navDist > kMaxNavDist) {
            sideLerp = (kMaxNavDist / navDist) * 0.9f;
        }

        UMath::Vector3 leftPos;
        int focal_error = 0;
        UMath::Lerp(nav.GetPosition(), nav.GetLeftPosition(), sideLerp, leftPos);
        FixWorldHeight(&leftPos, CameraType);

        UMath::Vector4 leftPosVec4 = UMath::Vector4Make(leftPos, 1.0f);
        UMath::Vector4 leftPosVec4_2 = leftPosVec4;

        if (IsSomethingInBetween(leftPosVec4_2, carPos)) {
            UMath::Vector4 leftPosVec4_3 = UMath::Vector4Make(leftPos, 1.0f);
            leftPosVec4 = leftPosVec4_3;
            if (!IsSomethingInBetween(leftPosVec4, carPosQuarterTime) && CameraType != 2) {
                focal_error = 1;
            }
        } else {
            focal_error = 1;
        }

        if (focal_error) {
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(leftPos), vLeft);
            if (bDistBetween(&PreviousEye, &vLeft) > 3.0f) {
                leftDist = bDistBetween(&vFuture, &vLeft);
            }
        }

        navDist = UMath::Distancexz(nav.GetRightPosition(), nav.GetPosition());
        sideLerp = 0.9f;
        if (navDist > kMaxNavDist) {
            sideLerp = (kMaxNavDist / navDist) * 0.9f;
        }

        UMath::Vector3 rightPos;
        focal_error = 0;
        UMath::Lerp(nav.GetPosition(), nav.GetRightPosition(), sideLerp, rightPos);
        FixWorldHeight(&rightPos, CameraType);

        UMath::Vector4 rightPosVec4 = UMath::Vector4Make(rightPos, 1.0f);
        UMath::Vector4 rightPosVec4_2 = rightPosVec4;

        if (IsSomethingInBetween(rightPosVec4_2, carPos)) {
            UMath::Vector4 rightPosVec4_3 = UMath::Vector4Make(rightPos, 1.0f);
            rightPosVec4 = rightPosVec4_3;
            if (!IsSomethingInBetween(rightPosVec4, carPosQuarterTime) && CameraType != 2) {
                focal_error = 1;
            }
        } else {
            focal_error = 1;
        }

        if (focal_error) {
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(rightPos), vRight);
            if (bDistBetween(&PreviousEye, &vRight) > 3.0f) {
                rightDist = bDistBetween(&vFuture, &vRight);
            }
        }
    }

    {
        UMath::Vector3 pos;
        focal_error = 0;
        bScaleAdd(&vLeftHigh, &vFutureHigh, CarToFollow->GetLeftVector(), 4.0f);
        eUnSwizzleWorldVector(vLeftHigh, reinterpret_cast<bVector3 &>(pos));
        FixWorldHeight(&pos, 3);

        UMath::Vector4 posVec4 = UMath::Vector4Make(pos, 1.0f);
        pos.y = Vector4To3(posVec4).y;
        UMath::Vector4 posVec4_2 = posVec4;

        if (IsSomethingInBetween(posVec4_2, carPos)) {
            UMath::Vector4 posVec4_3 = UMath::Vector4Make(pos, 1.0f);
            posVec4 = posVec4_3;
            if (!IsSomethingInBetween(posVec4, carPosQuarterTime) && CameraType != 2) {
                focal_error = 1;
            }
        } else {
            focal_error = 1;
        }

        if (focal_error) {
            if (bDistBetween(&PreviousEye, &vLeftHigh) > 3.0f) {
                leftDistHigh = bDistBetween(&vFutureHigh, &vLeftHigh) * 0.1f;
            }
        }
    }

    {
        UMath::Vector3 pos;
        focal_error = 0;
        bScaleAdd(&vRightHigh, &vFutureHigh, CarToFollow->GetLeftVector(), -4.0f);
        eUnSwizzleWorldVector(vRightHigh, reinterpret_cast<bVector3 &>(pos));
        FixWorldHeight(&pos, 3);

        UMath::Vector4 posVec4 = UMath::Vector4Make(pos, 1.0f);
        pos.y = Vector4To3(posVec4).y;
        UMath::Vector4 posVec4_2 = posVec4;

        if (IsSomethingInBetween(posVec4_2, carPos)) {
            UMath::Vector4 posVec4_3 = UMath::Vector4Make(pos, 1.0f);
            posVec4 = posVec4_3;
            if (!IsSomethingInBetween(posVec4, carPosQuarterTime) && CameraType != 2) {
                focal_error = 1;
            }
        } else {
            focal_error = 1;
        }

        if (focal_error) {
            if (bDistBetween(&PreviousEye, &vRightHigh) > 3.0f) {
                rightDistHigh = bDistBetween(&vFutureHigh, &vRightHigh) * 0.1f;
            }
        }
    }

    if (leftDist > 0.0f || rightDist > 0.0f || leftDistHigh > 0.0f || rightDistHigh > 0.0f) {
        if (leftDist + leftDistHigh > rightDist + rightDistHigh) {
            if (leftDist > leftDistHigh) {
                Eye = vLeft;
            } else {
                Eye = vLeftHigh;
            }
        } else {
            if (rightDist > rightDistHigh) {
                Eye = vRight;
            } else {
                Eye = vRightHigh;
            }
        }
    } else {
        float heightOffset = 2.0f;
        Eye = *pCamera->GetPosition();
        Eye.z += heightOffset;
        if (IsSomethingInBetween(&Eye, CarToFollow->GetGeometryPosition())) {
            Eye.z -= heightOffset;
        }
    }

    Eye.z += 0.5f;
    PreviousEye = Eye;

    FocalDistCubic.SetDuration(0.42f);
    FocalDistCubic.SetFlags(1);

    int focal_error = bRandom(0x13) + 0x1A;
    if (bRandom(2) != 0) {
        focal_error = -focal_error;
    }
    FocalDistCubic.SetVal(static_cast<float>(focal_error));
    FocalDistCubic.SetValDesired(0.0f);

    if (FocusEffects == 0) {
        if (!Camera::StopUpdating) {
            pCamera->SetFocalDistance(0.0f);
        }
        if (!Camera::StopUpdating) {
            pCamera->SetDepthOfField(0.0f);
        }
    }
}