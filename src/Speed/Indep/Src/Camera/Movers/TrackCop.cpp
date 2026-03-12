#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"

extern bool TrackCopCameraMover_IdleSim;
void HideEverySingleHud();

static float CrossXY(const bVector3 *v1, const bVector3 *v2) {
    return v1->x * v2->y - v1->y * v2->x;
}

void TrackCopCameraMover::Init() {
    bVector3 copPos;
    int focal_error;

    TrackCopCameraMover_IdleSim = 1;
    HideEverySingleHud();

    unsigned short fov;

    bCopy(&EyeVerts.v0, GetCamera()->GetPosition(), 0.0f);
    bCopy(&EyeVerts.v1, GetCamera()->GetPosition(), 0.0f);
    bCopy(&EyeVerts.v2, GetCamera()->GetPosition(), 0.0f);
    bCopy(&EyeVerts.v3, GetCamera()->GetPosition(), 0.0f);

    bCopy(&LookVerts.v0, GetCamera()->GetTarget(), 0.0f);
    bCopy(&LookVerts.v1, GetCamera()->GetTarget(), 0.0f);
    bCopy(&LookVerts.v2, GetCamera()->GetTarget(), 0.0f);
    bCopy(&LookVerts.v3, GetCamera()->GetTarget(), 0.0f);

    bool copFound = FindPursuitVehiclePosition(&copPos);

    if (copFound) {
        bVector3 eyeDisplacement = *GetCamera()->GetPosition() - *GetCamera()->GetTarget();
        float eyeDistance = bLength(&eyeDisplacement);

        bVector3 copDisplacement = *GetCamera()->GetTarget() - copPos;
        float savedZ = copDisplacement.z;
        copDisplacement.z = 0.0f;
        float copDistance = bLength(&copDisplacement);

        if (copDistance < 0.1f) {
            copDistance = 0.1f;
        }

        copDisplacement /= copDistance;
        copDisplacement *= 5.0f;

        copDisplacement.z = bClamp(savedZ, eyeDisplacement.z, 2.25f) + 0.25f;

        copDistance = bLength(&copDisplacement);

        bVector3 copEyePos = *GetCamera()->GetTarget() + copDisplacement;

        float dot = bDot(&eyeDisplacement, &copDisplacement) / (eyeDistance * copDistance);

        if (dot < 0.0f) {
            float scale = (1.0f - dot) * 0.5f;
            float cross = CrossXY(&eyeDisplacement, &copDisplacement);
            if (cross < 0.0f) {
                scale = -scale;
            }

            bVector3 eyeNormalDisplacement(-eyeDisplacement.y * scale, eyeDisplacement.x * scale, 0.0f);
            bVector3 copNormalDisplacement(copDisplacement.y * scale, -copDisplacement.x * scale, 0.0f);

            bVector3 ctrl1 = *GetCamera()->GetPosition() + eyeNormalDisplacement;
            bVector3 ctrl2 = copEyePos + copNormalDisplacement;

            if (!IsSomethingInBetween(&ctrl1, GetCamera()->GetTarget()) &&
                !IsSomethingInBetween(&ctrl2, GetCamera()->GetTarget())) {
                bCopy(&EyeVerts.v1, &ctrl1, 0.0f);
                bCopy(&EyeVerts.v2, &ctrl2, 0.0f);
                bCopy(&EyeVerts.v3, &copEyePos, 0.0f);
            }
        }

        bCopy(&LookVerts.v3, &copPos, 0.0f);
    }

    EyeSpline.SetControlPoints(&EyeVerts);
    LookSpline.SetControlPoints(&LookVerts);

    bVector3 displacement = *reinterpret_cast<bVector3 *>(&EyeVerts.v3) - *reinterpret_cast<bVector3 *>(&LookVerts.v3);
    float distance = bLength(&displacement);
    if (distance < 1.0f) {
        distance = 1.0f;
    }

    fov = static_cast<unsigned short>((bATan(distance, 3.0f) & 0x7FFF) << 1);

    if (fov != 0) {
        fov = static_cast<unsigned short>(bClamp(static_cast<int>(fov), 1000, 13100));
        float fovDegDefault = bAngToDeg(GetCamera()->GetFov());
        float fovDeg = bAngToDeg(fov);

        ZoomVerts.v0.x = fovDegDefault;
        ZoomVerts.v0.y = fovDegDefault;
        ZoomVerts.v0.z = fovDegDefault;
        ZoomVerts.v0.w = 0.0f;
        ZoomVerts.v1.x = fovDegDefault;
        ZoomVerts.v1.y = fovDegDefault;
        ZoomVerts.v1.z = fovDegDefault;
        ZoomVerts.v1.w = 0.0f;
        ZoomVerts.v2.x = fovDegDefault;
        ZoomVerts.v2.y = fovDegDefault;
        ZoomVerts.v2.z = fovDegDefault;
        ZoomVerts.v2.w = 0.0f;
        ZoomVerts.v3.x = fovDeg;
        ZoomVerts.v3.y = fovDeg;
        ZoomVerts.v3.z = fovDeg;
        ZoomVerts.v3.w = 0.0f;
    } else {
        float fovDeg = bAngToDeg(GetCamera()->GetFov());

        ZoomVerts.v0.x = fovDeg;
        ZoomVerts.v0.y = fovDeg;
        ZoomVerts.v0.z = fovDeg;
        ZoomVerts.v0.w = 0.0f;
        ZoomVerts.v1.x = fovDeg;
        ZoomVerts.v1.y = fovDeg;
        ZoomVerts.v1.z = fovDeg;
        ZoomVerts.v1.w = 0.0f;
        ZoomVerts.v2.x = fovDeg;
        ZoomVerts.v2.y = fovDeg;
        ZoomVerts.v2.z = fovDeg;
        ZoomVerts.v2.w = 0.0f;
        ZoomVerts.v3.x = fovDeg;
        ZoomVerts.v3.y = fovDeg;
        ZoomVerts.v3.z = fovDeg;
        ZoomVerts.v3.w = 0.0f;
    }

    ZoomSpline.SetControlPoints(&ZoomVerts);

    FocalDistCubic.SetDuration(0.42f);
    FocalDistCubic.SetFlags(1);

    focal_error = bRandom(19) + 26;
    if (bRandom(2) != 0) {
        focal_error = -focal_error;
    }

    FocalDistCubic.SetVal(static_cast<float>(focal_error));
    FocalDistCubic.SetValDesired(0.0f);

    if (FocusEffects == 0) {
        if (Camera::StopUpdating == 0) {
            GetCamera()->SetFocalDistance(0.0f);
        }
        if (Camera::StopUpdating == 0) {
            GetCamera()->SetDepthOfField(0.0f);
        }
    }

    if (!copFound) {
        CameraAI::MaybeKillPursuitCam(CarToFollow->GetWorldID());
    }
}
