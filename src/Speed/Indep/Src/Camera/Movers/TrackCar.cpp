#include "Speed/Indep/Src/Camera/Movers/TrackCar.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/Point.hpp"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

static bVector3 PreviousEye(0.0f, 0.0f, 0.0f); // .bss: 0x8045ADF4

static const float Tweak_JumpCamPositionSpeedMult[4] = {0.5f, 1.2999999523f, 1.0f, 1.5f};       // 0x803D3AFC
static const float TrackCarIsoZoomDistance[3] = {3.2999999523f, 3.2999999523f, 3.2999999523f}; // 0x803D3B0C
static const float TrackCarLookOffsetX[3] = {1.2000000476f, -4.0f, 1.2000000476f};              // 0x803D3B18
static const float TrackCarLookOffsetY[3] = {0.0f, 0.0f, 0.0f};                                 // 0x803D3B24
static const float TrackCarLookOffsetZ[3] = {-0.2000000029f, -0.2000000029f, 1.0f};             // 0x803D3B30
static const float TrackCarEyeOffsetZ[4] = {0.1000000014f, 3.0f, 3.5f, 2.0f};                   // 0x803D3B3C

TrackCarCameraMover::TrackCarCameraMover(int nView, CameraAnchor *pCar, bool focus_effects)
    : CameraMover(nView, CM_TRACK_CAR), CarToFollow(pCar), FocalDistCubic(1, 1.0f) {
    FocusEffects = focus_effects;
    CameraType = 0;

    Init();
}

TrackCarCameraMover::~TrackCarCameraMover() {
    pCamera->SetSimTimeMultiplier(1.0f);
    pCamera->SetFocalDistance(0.0f);
    pCamera->SetDepthOfField(0.0f);
    pCamera->ClearVelocity();
}

static bool IsAnyCopNear(CameraAnchor *pCar) {
    for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_AICOPS).begin(); iter != IVehicle::GetList(VEHICLE_AICOPS).end();
         iter++) {
        IVehicle *p_car = *iter;
        if (p_car == NULL || !p_car->IsActive()) {
            continue;
        }

        UVector3 pos = p_car->GetPosition();

        bVector3 coppos;

        eSwizzleWorldVector(pos, coppos);

        bVector3 copdir;

        bSub(&copdir, &coppos, pCar->GetGeometryPosition());

        float copdist = bLength(&copdir);

        if (copdist < 30.0f) {
            return true;
        }
    }
    return false;
}

static bool IsBeingPursued(int nView) {
    const UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, 3>::List &playerList =
        UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, 3>::GetList(PLAYER_LOCAL);
    for (IPlayer *const *iter = playerList.begin(); iter != playerList.end(); iter++) {
        IPlayer *ip = *iter;
        if (ip->GetRenderPort() == nView) {
            ISimable *simable = ip->GetSimable();
            if (simable == NULL) {
                return false;
            }
            IPerpetrator *iperp;
            simable->QueryInterface(&iperp);
            if (iperp == NULL) {
                return false;
            }
            if (!iperp->IsBeingPursued()) {
                break;
            }

            return true;
        }
    }
    return false;
}

static void FixWorldHeight(UMath::Vector3 *point, int type) {
    if (IsGameFlowInGame()) {
        UMath::Vector3 test(*point);
        test.y += 4.0f;

        float ground_elevation = 0.0f;
        if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(test, ground_elevation, NULL)) {
            if (type == 3) {
                point->y = ground_elevation;
            }
        }
        if (ground_elevation > point->y) {
            point->y = ground_elevation;
        }

        point->y += TrackCarEyeOffsetZ[type];
    }
}

void TrackCarCameraMover::Init() {
    float rightDist;
    WRoadNav nav;

    nav.SetCookieTrail(true);
    nav.SetNavType(WRoadNav::kTypeDirection);

    if (gCamCloseToRoadBlock != 0) {
        gCamCloseToRoadBlock = 0;
        CameraType = 2;
    } else if (IsBeingPursued(ViewID)) {
        if (IsAnyCopNear(CarToFollow)) {
            CameraType = 1;
        }
    }

    float positionSpeedMultiplier[2];

    const float kMaxPositionDistance = 150.0f;

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
    eUnSwizzleWorldVector(vCarPosQuarterTime, (bVector3 &)carPosQuarterTime);

    bVector3 vFuture;
    bScaleAdd(&vFuture, CarToFollow->GetGeometryPosition(), CarToFollow->GetVelocity(), positionSpeedMultiplier[0]);
    bVector3 vLeft(vFuture);
    bVector3 vRight(vFuture);

    bVector3 vFutureHigh;
    bScaleAdd(&vFutureHigh, CarToFollow->GetGeometryPosition(), CarToFollow->GetVelocity(),
              positionSpeedMultiplier[1]);
    bVector3 vLeftHigh(vFutureHigh);
    bVector3 vRightHigh(vFutureHigh);

    float rightDistHigh = 0.0f;
    float leftDistHigh = 0.0f;
    rightDist = 0.0f;

    float leftDist = 0.0f;

    UMath::Vector4 carPos;
    UMath::Vector4 carDir;
    eUnSwizzleWorldVector(*CarToFollow->GetGeometryPosition(), (bVector3 &)carPos);
    eUnSwizzleWorldVector(*CarToFollow->GetForwardVector(), (bVector3 &)carDir);

    nav.InitAtPoint(Vector4To3(carPos), Vector4To3(carDir), true, 1.0f);

    if (nav.IsValid()) {
        nav.IncNavPosition(positionSpeedMultiplier[0] * CarToFollow->GetVelocityMagnitude(),
                           UMath::Vector3::kZero, 0.0f);

        const float kMaxNavDist = 10.0f;
        float navDist;
        float sideLerp;

        navDist = Distancexz(nav.GetPosition(), nav.GetLeftPosition());
        sideLerp = 0.9f;
        if (navDist > kMaxNavDist) {
            sideLerp = 0.9f * (kMaxNavDist / navDist);
        }

        UMath::Vector3 leftPos;

        Lerp(nav.GetPosition(), nav.GetLeftPosition(), sideLerp, leftPos);
        FixWorldHeight(&leftPos, CameraType);

        if (!IsSomethingInBetween(Vector4Make(leftPos, 1.0f), carPos) ||
            (!IsSomethingInBetween(Vector4Make(leftPos, 1.0f), carPosQuarterTime) &&
             CameraType != 2)) {
            eSwizzleWorldVector(leftPos, vLeft);
            if (bDistBetween(&PreviousEye, &vLeft) > 3.0f) {
                leftDist = bDistBetween(&vFuture, &vLeft);
            }
        }

        navDist = Distancexz(nav.GetPosition(), nav.GetRightPosition());
        sideLerp = 0.9f;
        if (navDist > kMaxNavDist) {
            sideLerp = 0.9f * (kMaxNavDist / navDist);
        }

        UMath::Vector3 rightPos;

        Lerp(nav.GetPosition(), nav.GetRightPosition(), sideLerp, rightPos);
        FixWorldHeight(&rightPos, CameraType);

        if (!IsSomethingInBetween(Vector4Make(rightPos, 1.0f), carPos) ||
            (!IsSomethingInBetween(Vector4Make(rightPos, 1.0f), carPosQuarterTime) &&
             CameraType != 2)) {
            eSwizzleWorldVector(rightPos, vRight);
            if (bDistBetween(&PreviousEye, &vRight) > 3.0f) {
                rightDist = bDistBetween(&vFuture, &vRight);
            }
        }

    }

    {
        UMath::Vector3 pos;

        bScaleAdd(&vLeftHigh, &vFutureHigh, CarToFollow->GetLeftVector(), 4.0f);
        eUnSwizzleWorldVector(vLeftHigh, (bVector3 &)pos);
        FixWorldHeight(&pos, 3);
        vLeftHigh.z = pos.y;

        if (!IsSomethingInBetween(Vector4Make(pos, 1.0f), carPos) ||
            (!IsSomethingInBetween(Vector4Make(pos, 1.0f), carPosQuarterTime) &&
             CameraType != 2)) {
            if (bDistBetween(&PreviousEye, &vLeftHigh) > 3.0f) {
                leftDistHigh = 0.1f * bDistBetween(&vFutureHigh, &vLeftHigh);
            }
        }

        bScaleAdd(&vRightHigh, &vFutureHigh, CarToFollow->GetLeftVector(), -4.0f);
        eUnSwizzleWorldVector(vRightHigh, (bVector3 &)pos);
        FixWorldHeight(&pos, 3);
        vRightHigh.z = pos.y;

        if (!IsSomethingInBetween(Vector4Make(pos, 1.0f), carPos) ||
            (!IsSomethingInBetween(Vector4Make(pos, 1.0f), carPosQuarterTime) &&
             CameraType != 2)) {
            if (bDistBetween(&PreviousEye, &vRightHigh) > 3.0f) {
                rightDistHigh = 0.1f * bDistBetween(&vFutureHigh, &vRightHigh);
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
        Eye = *pCamera->GetPosition();
        Eye.z += 2.0f;
        if (IsSomethingInBetween(&Eye, CarToFollow->GetGeometryPosition())) {
            Eye.z -= 2.0f;
        }
    }

    Eye.z += 0.5f;
    PreviousEye = Eye;

    FocalDistCubic.duration = 0.42f;
    FocalDistCubic.flags = 1;

    int focal_error = bRandom(19) + 26;
    if (bRandom(2) != 0) {
        focal_error = -focal_error;
    }

    FocalDistCubic.SetVal((float)focal_error);
    FocalDistCubic.SetValDesired(0.0f);

    if (FocusEffects == 0) {
        pCamera->SetFocalDistance(0.0f);
        pCamera->SetDepthOfField(0.0f);
    }
}

bVector3 *TrackCarCameraMover::GetTarget() {
    if (CarToFollow != NULL) {
        return CarToFollow->GetGeometryPosition();
    }
    return pCamera->GetTarget();
}

void TrackCarCameraMover::Update(float dT) {
    if (IsGameFlowPaused()) {
        return;
    }

    pCamera->SetSimTimeMultiplier(0.2f);

    bVector3 up(0.0f, 0.0f, 1.0f);
    unsigned short fov;

    bVector3 displacement(Eye - *CarToFollow->GetGeometryPosition());

    float distance = bLength(&displacement);
    if (distance < 1.0f) {
        distance = 1.0f;
    }

    fov = 2 * bATan(distance, TrackCarIsoZoomDistance[CameraType]);
    if (fov != 0) {
        pCamera->SetFieldOfView(bClamp(fov, 800, 13100));
    }

    Look = *CarToFollow->GetGeometryPosition();

    displacement /= distance;

    float vert_comp = 0.0f;
    float horiz_comp = 0.0f;

    bVector3 hcomp;
    bCross(&hcomp, &displacement, &up);
    bScale(&hcomp, &hcomp, vert_comp);

    bVector3 look_offset;
    look_offset.x = TrackCarLookOffsetX[CameraType];
    look_offset.y = TrackCarLookOffsetY[CameraType];
    look_offset.z = TrackCarLookOffsetZ[CameraType];
    eMulVector(&look_offset, CarToFollow->GetGeometryOrientation(), &look_offset);

    Look += look_offset;

    bVector3 lookdir(Look - Eye);
    bNormalize(&lookdir, &lookdir);

    bMatrix4 m;
    eCreateLookAtMatrix(&m, Eye, Look, up);

    float focal_dist = bDistBetween(CarToFollow->GetGeometryPosition(), &Eye);
    pCamera->SetTargetDistance(focal_dist);

    FocalDistCubic.SetdValDesired(FocalDistCubic.Val * 6.2699999809f);
    cPoint::SplineSeek(&FocalDistCubic, dT, 0.0f, 0.0f);

    focal_dist += FocalDistCubic.Val;
    if (focal_dist < 2.0f) {
        focal_dist = 2.0f;
    }

    if (FocusEffects != 0) {
        pCamera->SetFocalDistance(focal_dist + FocalDistCubic.Val);
        pCamera->SetDepthOfField(7.5f);
    }

    pCamera->SetCameraMatrix(m, dT);

    if (IsSomethingInBetween(pCamera->GetPosition(), CarToFollow->GetGeometryPosition())) {
        CameraAI::MaybeKillJumpCam(CarToFollow->GetWorldID());
    }
}
