#include "Speed/Indep/Src/Camera/Movers/TrackCop.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/Point.hpp"
#include "Speed/Indep/Src/AI/AITarget.h"

// Inlined tCubic1D::tCubic1D(short type, float dur) ctor (Spline.hpp is shared; replicated here).
static inline void TrackCopCubic1DInit(tCubic1D *cubic, short type, float dur) {
    cubic->duration = dur;
    cubic->state = 0;
    cubic->flags = type;
    cubic->Val = 0.0f;
    cubic->dVal = 0.0f;
    cubic->ValDesired = 0.0f;
    cubic->dValDesired = 0.0f;
    cubic->time = 0.0f;
    cubic->Coeff[0] = 0.0f;
    cubic->Coeff[1] = 0.0f;
    cubic->Coeff[2] = 0.0f;
    cubic->Coeff[3] = 0.0f;
}

// Original bMath.hpp has bAngToDeg(unsigned short) = angle * (360/65536); the shared header's
// signature differs, so the conversion is replicated here.
static inline float TrackCopAngToDeg(unsigned short angle) {
    return ((float)angle) * (360.0f / 65536.0f);
}

int TrackCopCameraMover_IdleSim = 0; // .data: 0x80416FF0

Bezier::Bezier() {
    pControlPoints = NULL;

    bFill(&mBasis.v0, 1.0f, -3.0f, 3.0f, -1.0f);
    bFill(&mBasis.v1, 0.0f, 3.0f, -6.0f, 3.0f);
    bFill(&mBasis.v2, 0.0f, 0.0f, 3.0f, -3.0f);
    bFill(&mBasis.v3, 0.0f, 0.0f, 0.0f, 1.0f);
}

void Bezier::GetPoint(bVector3 *pOut, float t) {
    if (pControlPoints != NULL) {
        float s = 1.0f - t;

        bVector4 u;
        u.x = s * (s * s);
        u.y = s * s;
        u.z = s;
        u.w = 1.0f;
        bVector4 p;

        bMulMatrix(&p, &mBasis, &u);
        bMulMatrix((bVector4 *)pOut, pControlPoints, &p);
    }
}

static float CrossXY(const bVector3 *v1, const bVector3 *v2) {
    return v1->x * v2->y - v1->y * v2->x;
}

TrackCopCameraMover::TrackCopCameraMover(int nView, CameraAnchor *pCar, bool focus_effects)
    : CameraMover(nView, CM_TRACK_COP),
      CarToFollow(pCar),
      ZoomSplineParam(0.0f),
      EyeSplineParam(0.0f),
      LookSplineParam(0.0f) {
    TrackCopCubic1DInit(&FocalDistCubic, 1, 1.0f);

    FocusEffects = focus_effects;

    bRenderCarPOV = 1;

    Init();
}

TrackCopCameraMover::~TrackCopCameraMover() {
    TrackCopCameraMover_IdleSim = 0;
    pCamera->SetSimTimeMultiplier(1.0f);
    pCamera->SetFocalDistance(0.0f);
    pCamera->SetDepthOfField(0.0f);
    pCamera->ClearVelocity();
}

bool TrackCopCameraMover::FindPursuitVehiclePosition(bVector3 *copPos) {
    const float kMaxDist = 300.0f;

    float minDist = kMaxDist;

    for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_AICOPS).begin(); iter != IVehicle::GetList(VEHICLE_AICOPS).end();
         iter++) {
        IVehicle *p_car = *iter;
        if (p_car == NULL) {
            continue;
        }
        if (!p_car->IsActive()) {
            continue;
        }
        if (p_car->GetVehicleClass() == VehicleClass::CAR) {
            IVehicleAI *p_vehicleai;
            if (p_car->QueryInterface(&p_vehicleai)) {
            AITarget *p_target = p_vehicleai->GetTarget();
            if (p_target == NULL) {
                continue;
            }
            if (!p_target->IsValid()) {
                continue;
            }

            ISimable *p_targetsimable = p_target->GetSimable();
            if (p_targetsimable == NULL) {
                continue;
            }
            if (p_targetsimable->GetWorldID() != CarToFollow->GetWorldID()) {
                continue;
            }

            IPursuitAI *p_pursuitai;
            if (p_car->QueryInterface(&p_pursuitai) && p_pursuitai != NULL) {
                if (p_pursuitai->GetInPursuit()) {
                    if (p_pursuitai->GetTimeSinceTargetSeen() > 0.0f) {
                        continue;
                    }

                    UMath::Vector3 upos = p_car->GetPosition();

                    bVector3 bpos;

                    eSwizzleWorldVector(upos, bpos);

                    if (IsSomethingInBetween(pCamera->GetPosition(), &bpos)) {
                        continue;
                    }

                    float dist = bDistBetween(CarToFollow->GetGeometryPosition(), &bpos);
                    if (dist < minDist) {
                        minDist = dist;
                        *copPos = bpos;
                    }
                }
            }
            }
        }
    }

    return minDist < kMaxDist;
}

void TrackCopCameraMover::Init() {
    TrackCopCameraMover_IdleSim = 1;
    HideEverySingleHud();

    bCopy(&EyeVerts.v0, pCamera->GetPosition(), 0.0f);
    bCopy(&EyeVerts.v1, pCamera->GetPosition(), 0.0f);
    bCopy(&EyeVerts.v2, pCamera->GetPosition(), 0.0f);
    bCopy(&EyeVerts.v3, pCamera->GetPosition(), 0.0f);

    bCopy(&LookVerts.v0, pCamera->GetTarget(), 0.0f);
    bCopy(&LookVerts.v1, pCamera->GetTarget(), 0.0f);
    bCopy(&LookVerts.v2, pCamera->GetTarget(), 0.0f);
    bCopy(&LookVerts.v3, pCamera->GetTarget(), 0.0f);

    bVector3 copPos;
    bool copFound = FindPursuitVehiclePosition(&copPos);
    if (copFound) {
        bVector3 eyeDisplacement(*pCamera->GetPosition() - *pCamera->GetTarget());
        float eyeDistance = bLength(&eyeDisplacement);

        float copDistance = 5.0f;

        bVector3 copDisplacement(*pCamera->GetTarget() - copPos);
        float savedZ = copDisplacement.z;
        copDisplacement.z = 0.0f;

        float distance = bLength(&copDisplacement);
        if (distance < 0.1f) {
            distance = 0.1f;
        }

        copDisplacement /= distance;
        copDisplacement *= copDistance;
        copDisplacement.z = bClamp(savedZ, eyeDisplacement.z, 2.25f) + 0.25f;

        copDistance = bLength(&copDisplacement);

        bVector3 copEyePos(*pCamera->GetTarget() + copDisplacement);

        float dot = bDot(&eyeDisplacement, &copDisplacement) / (eyeDistance * copDistance);
        if (dot < 0.0f) {
            float s = (1.0f - dot) * 0.5f;
            if (CrossXY(&eyeDisplacement, &copDisplacement) < 0.0f) {
                s = -s;
            }

            bVector3 perpEye(-eyeDisplacement.y * s, eyeDisplacement.x * s, 0.0f);
            bVector3 perpCop(copDisplacement.y * s, -copDisplacement.x * s, 0.0f);

            bVector3 eyePos(*pCamera->GetPosition() + perpEye);
            bVector3 copPos2(copEyePos + perpCop);

            if (!IsSomethingInBetween(&eyePos, pCamera->GetTarget()) &&
                !IsSomethingInBetween(&copPos2, pCamera->GetTarget())) {
                bCopy(&EyeVerts.v1, &eyePos, 0.0f);
                bCopy(&EyeVerts.v2, &copPos2, 0.0f);
                bCopy(&EyeVerts.v3, &copEyePos, 0.0f);
            }
        }

        bCopy(&LookVerts.v3, &copPos, 0.0f);
    }

    EyeSpline.SetControlPoints(&EyeVerts);
    LookSpline.SetControlPoints(&LookVerts);

    bVector3 displacement(*(bVector3 *)&EyeVerts.v3 - *(bVector3 *)&LookVerts.v3);
    float distance = bLength(&displacement);
    if (distance < 1.0f) {
        distance = 1.0f;
    }

    unsigned short fov = 2 * bATan(distance, 3.0f);
    if (fov != 0) {
        fov = bClamp(fov, 1000, 13100);

        float startDeg = TrackCopAngToDeg(pCamera->GetFieldOfView());

        ZoomVerts.v0.x = ZoomVerts.v0.y = ZoomVerts.v0.z = ZoomVerts.v1.x = ZoomVerts.v1.y = ZoomVerts.v1.z = ZoomVerts.v2.x = ZoomVerts.v2.y =
            ZoomVerts.v2.z = startDeg;

        float endDeg = TrackCopAngToDeg(fov);

        ZoomVerts.v3.x = ZoomVerts.v3.y = ZoomVerts.v3.z = endDeg;

        ZoomVerts.v0.w = ZoomVerts.v1.w = ZoomVerts.v2.w = ZoomVerts.v3.w = 0.0f;
    } else {
        float startDeg = TrackCopAngToDeg(pCamera->GetFieldOfView());

        ZoomVerts.v0.x = ZoomVerts.v0.y = ZoomVerts.v0.z = ZoomVerts.v1.x = ZoomVerts.v1.y = ZoomVerts.v1.z = ZoomVerts.v2.x = ZoomVerts.v2.y =
            ZoomVerts.v2.z = ZoomVerts.v3.x = ZoomVerts.v3.y = ZoomVerts.v3.z = startDeg;

        ZoomVerts.v0.w = ZoomVerts.v1.w = ZoomVerts.v2.w = ZoomVerts.v3.w = 0.0f;
    }

    ZoomSpline.SetControlPoints(&ZoomVerts);

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

    if (!copFound) {
        CameraAI::MaybeKillPursuitCam(CarToFollow->GetWorldID());
    }
}

bVector3 *TrackCopCameraMover::GetTarget() {
    if (CarToFollow != NULL) {
        return CarToFollow->GetGeometryPosition();
    }
    return pCamera->GetTarget();
}

void TrackCopCameraMover::Update(float dT) {
    FEManager *fe = FEManager::Get();
    if (fe != NULL && FEManager::ShouldPauseSimulation(true)) {
        return;
    }

    bVector3 up(0.0f, 0.0f, 1.0f);

    bVector3 Eye;
    bVector3 Look;
    bVector3 Zoom;

    EyeSplineParam += dT * 1.275f;
    if (EyeSplineParam > 1.0f) {
        EyeSplineParam = 1.0f;
    }
    EyeSpline.GetPoint(&Eye, EyeSplineParam);

    LookSplineParam += dT * 0.765f;
    if (LookSplineParam > 1.0f) {
        LookSplineParam = 1.0f;
    }
    LookSpline.GetPoint(&Look, LookSplineParam);

    ZoomSplineParam += dT * 0.765f;
    if (ZoomSplineParam > 1.0f) {
        ZoomSplineParam = 1.0f;
    }
    ZoomSpline.GetPoint(&Zoom, ZoomSplineParam);

    unsigned short fov = bDegToAng(Zoom.z);
    pCamera->SetFieldOfView(fov);

    bVector3 displacement(Eye - Look);
    float distance = bLength(&displacement);
    if (distance < 1.0f) {
        distance = 1.0f;
    }

    displacement /= distance;

    float vert_comp = 0.0f;
    float horiz_comp = 0.0f;

    bVector3 hcomp;
    bCross(&hcomp, &displacement, &up);
    bScale(&hcomp, &hcomp, vert_comp);

    bVector3 look_offset;
    look_offset.x = 0.0f;
    look_offset.y = 0.0f;
    look_offset.z = 0.0f;
    eMulVector(&look_offset, CarToFollow->GetGeometryOrientation(), &look_offset);
    Look += look_offset;

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
}
