#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"

extern bool gCamCloseToRoadBlock;

static const float Tweak_JumpCamPositionSpeedMult[4] = {0.5f, 1.3f, 1.0f, 1.5f};
static bVector3 PreviousEye;

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

void TrackCarCameraMover::Init() {
    const float kMaxPositionDistance = 150.0f;
    WRoadNav nav;
    nav.SetCookieTrail(true);
    nav.SetNavType(WRoadNav::kTypeDirection);

    if (gCamCloseToRoadBlock) {
        gCamCloseToRoadBlock = false;
        CameraType = 2;
    } else {
        if (IsBeingPursued(ViewID) && IsAnyCopNear(CarToFollow)) {
            CameraType = 1;
        }
    }

    float positionSpeedMultiplier[2];
    positionSpeedMultiplier[0] = Tweak_JumpCamPositionSpeedMult[CameraType];
    positionSpeedMultiplier[1] = Tweak_JumpCamPositionSpeedMult[3];

    if (CarToFollow->GetVelocityMagnitude() * positionSpeedMultiplier[0] > kMaxPositionDistance) {
        positionSpeedMultiplier[0] = kMaxPositionDistance / CarToFollow->GetVelocityMagnitude();
    }
    if (CarToFollow->GetVelocityMagnitude() * positionSpeedMultiplier[1] > kMaxPositionDistance) {
        positionSpeedMultiplier[1] = kMaxPositionDistance / CarToFollow->GetVelocityMagnitude();
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

    nav.InitAtPoint(reinterpret_cast<const UMath::Vector3 &>(carPos),
                    reinterpret_cast<const UMath::Vector3 &>(carDir), true, 1.0f);

    float leftDist = 0.0f;
    float rightDist = 0.0f;
    float leftDistHigh = 0.0f;
    float rightDistHigh = 0.0f;

    if (nav.IsValid()) {
        nav.IncNavPosition(positionSpeedMultiplier[0] * CarToFollow->GetVelocityMagnitude(),
                           UMath::Vector3::kZero, 0.0f);

        float navDist = UMath::Distancexz(nav.GetPosition(), nav.GetLeftPosition());

        float sideLerp = 0.9f;
        if (navDist > 10.0f) {
            sideLerp = (10.0f / navDist) * 0.9f;
        }

        UMath::Vector3 leftPos;
        UMath::Vector3 rightPos;

        int focal_error = 0;

        UMath::Lerp(nav.GetLeftPosition(), nav.GetPosition(), sideLerp, leftPos);
        FixWorldHeight(&leftPos, CameraType);

        if (!IsSomethingInBetween(UMath::Vector4Make(leftPos, 1.0f),
                                  UMath::Vector4Make(reinterpret_cast<UMath::Vector3 &>(carPos), 1.0f))) {
            if (IsSomethingInBetween(UMath::Vector4Make(leftPos, 1.0f), carPosQuarterTime) && CameraType == 2) {
            } else {
                focal_error = 1;
            }
        }

        if (focal_error) {
            eSwizzleWorldVector(reinterpret_cast<bVector3 &>(leftPos), vLeft);
            if (bDistBetween(&PreviousEye, &vLeft) > 3.0f) {
                leftDist = bDistBetween(&vFuture, &vLeft);
            }
        }

        navDist = UMath::Distancexz(nav.GetPosition(), nav.GetRightPosition());

        sideLerp = 0.9f;
        if (navDist > 10.0f) {
            sideLerp = (10.0f / navDist) * 0.9f;
        }

        focal_error = 0;

        UMath::Lerp(nav.GetRightPosition(), nav.GetPosition(), sideLerp, rightPos);
        FixWorldHeight(&rightPos, CameraType);

        if (!IsSomethingInBetween(UMath::Vector4Make(rightPos, 1.0f),
                                  UMath::Vector4Make(reinterpret_cast<UMath::Vector3 &>(carPos), 1.0f))) {
            if (IsSomethingInBetween(UMath::Vector4Make(rightPos, 1.0f), carPosQuarterTime) && CameraType == 2) {
            } else {
                focal_error = 1;
            }
        }

        if (focal_error) {
            eSwizzleWorldVector(reinterpret_cast<bVector3 &>(rightPos), vRight);
            if (bDistBetween(&PreviousEye, &vRight) > 3.0f) {
                rightDist = bDistBetween(&vFuture, &vRight);
            }
        }
    }

    {
        UMath::Vector3 pos;
        bool visible = false;

        bScaleAdd(&vLeftHigh, &vFutureHigh, CarToFollow->GetLeftVector(), 4.0f);
        eUnSwizzleWorldVector(vLeftHigh, reinterpret_cast<bVector3 &>(pos));
        FixWorldHeight(&pos, 3);
        vLeftHigh.z = pos.y;

        if (!IsSomethingInBetween(UMath::Vector4Make(pos, 1.0f),
                                  UMath::Vector4Make(reinterpret_cast<UMath::Vector3 &>(carPos), 1.0f))) {
            if (IsSomethingInBetween(UMath::Vector4Make(pos, 1.0f), carPosQuarterTime) && CameraType == 2) {
            } else {
                visible = true;
            }
        }

        if (visible && bDistBetween(&PreviousEye, &vLeftHigh) > 3.0f) {
            leftDistHigh = bDistBetween(&vFutureHigh, &vLeftHigh) * 0.1f;
        }

        visible = false;

        bScaleAdd(&vRightHigh, &vFutureHigh, CarToFollow->GetLeftVector(), -4.0f);
        eUnSwizzleWorldVector(vRightHigh, reinterpret_cast<bVector3 &>(pos));
        FixWorldHeight(&pos, 3);
        vRightHigh.z = pos.y;

        if (!IsSomethingInBetween(UMath::Vector4Make(pos, 1.0f),
                                  UMath::Vector4Make(reinterpret_cast<UMath::Vector3 &>(carPos), 1.0f))) {
            if (IsSomethingInBetween(UMath::Vector4Make(pos, 1.0f), carPosQuarterTime) && CameraType == 2) {
            } else {
                visible = true;
            }
        }

        if (visible && bDistBetween(&PreviousEye, &vRightHigh) > 3.0f) {
            rightDistHigh = bDistBetween(&vFutureHigh, &vRightHigh) * 0.1f;
        }
    }

    if (leftDist > 0.0f || rightDist > 0.0f || leftDistHigh > 0.0f || rightDistHigh > 0.0f) {
        if (leftDist + leftDistHigh <= rightDist + rightDistHigh) {
            if (rightDist > rightDistHigh) {
                Eye = vRight;
            } else {
                Eye = vRightHigh;
            }
        } else {
            if (leftDist > leftDistHigh) {
                Eye = vLeft;
            } else {
                Eye = vLeftHigh;
            }
        }
    } else {
        Eye = *GetCamera()->GetPosition();
        Eye.z += 2.0f;
        if (IsSomethingInBetween(GetCamera()->GetPosition(), CarToFollow->GetGeometryPosition())) {
            Eye.z -= 2.0f;
        }
    }

    Eye.z += 0.5f;
    PreviousEye = Eye;

    FocalDistCubic.SetDuration(0.42f);
    FocalDistCubic.SetFlags(1);

    int dutch_raw = bRandom(0x13) + 0x1a;
    if (bRandom(2) != 0) {
        dutch_raw = -dutch_raw;
    }

    float dutch = static_cast<float>(dutch_raw);
    FocalDistCubic.SetVal(dutch);
    FocalDistCubic.SetValDesired(0.0f);

    if (FocusEffects == 0) {
        if (!Camera::StopUpdating) {
            GetCamera()->SetFocalDistance(0.0f);
        }
        if (!Camera::StopUpdating) {
            GetCamera()->SetDepthOfField(0.0f);
        }
    }
}