#include "CameraMover.hpp"
#include "CameraAI.hpp"

Attrib::Key Attrib::Gen::ecar::ClassKey() {
    return 0xa5b543b7;
}

Attrib::Key Attrib::Gen::camerainfo::ClassKey() {
    return 0x93c171e4;
}
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"

DECLARE_CONTAINER_TYPE(CameraAIAvoidables);

struct Avoidables : public _STL::list<IBody *, UTL::Std::Allocator<IBody *, _type_CameraAIAvoidables> > {
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }
};

Avoidables *TheAvoidables;

extern int WeHaveCheckedIfJR2ServerExists;
extern int DisablePrecullerCounter;
extern int JR2ServerExists;
extern int LastUpdateTimeCaffeine;
extern int LastUpdateTimeJR2;
extern int RealTime;
extern int DisableCommunication;
static volatile const int RemoteCaffeinating = 0;
extern int bStreamingPositionFromICE;
extern Timer RealTimer;
extern bool TrackCopCameraMover_IdleSim;

bool OutsidePovType(int pov_type);
bool RenderCarPovType(int pov_type, bool look_back);
bool RenderCarPovTypeRaw(int pov_type, int look_back) __asm__("RenderCarPovType__Fib");
struct LongVector;
inline void espSetCameraPositionFix(const LongVector *eye, const LongVector *target) {}
inline void espCentrePlaneView(const bVector3 *pos) {}
int bFunkDoesServerExist(const char *server_name);

extern bVector4 CameraNoiseHandheldFrequency;
extern bVector4 CameraNoiseHandheldAmplitude;
extern bVector4 CameraNoiseChopperFrequency;
extern bVector4 CameraNoiseChopperAmplitude;
extern bVector4 CameraNoiseSpeedFrequency;
extern bVector4 CameraNoiseSpeedAmplitude;
extern bVector4 CameraNoiseTerrainFrequency;
extern bVector4 CameraNoiseTerrainAmplitude;
extern bVector4 CameraNoiseSpeedData[5];
extern float CameraAccelerationCurve[5];
extern bVector2 CameraSpeedHugData[5];
extern Graph gDriftSpeed;
extern CarTypeInfo *GetCarTypeInfoFromHash(unsigned int car_type_hash);
extern float RVMnearz;
extern float RVMfarz;
extern bVector3 RVMOffsetInCar;
extern float TrackCarIsoZoomDistance[3];
extern float TrackCarLookOffsetX[3];
extern float TrackCarLookOffsetY[3];
extern float TrackCarLookOffsetZ[3];
void ApplyCameraShake(int nViewID, bMatrix4 *pMatrix);
void HideEverySingleHud();
void SplineSeek(tCubic1D *p, float time, float fDClamp, float fDDClamp) __asm__("SplineSeek__6cPointP8tCubic1Dfff");
namespace CameraAI {
void MaybeKillJumpCam(unsigned int id);
void MaybeKillPursuitCam(unsigned int id);
}

struct LongVector {
    int x;
    int y;
    int z;
};

static inline void MakeLongVector(LongVector *dest, const bVector3 *src, float scale) {
    dest->x = static_cast<int>(src->x * scale);
    dest->y = static_cast<int>(src->y * scale);
    dest->z = static_cast<int>(src->z * scale);
}

static float SampleFloatTable(const float *data, int count, float value, float min_value, float max_value) {
    if (count <= 0) {
        return 0.0f;
    }
    if (value <= min_value) {
        return data[0];
    }
    if (value >= max_value) {
        return data[count - 1];
    }

    {
        float scaled = (value - min_value) * (static_cast<float>(count - 1) / (max_value - min_value));
        int index = static_cast<int>(bFloor(scaled));
        float blend = scaled - static_cast<float>(index);

        if (index < 0) {
            index = 0;
        }
        if (index >= count - 1) {
            index = count - 2;
        }

        return data[index] + (data[index + 1] - data[index]) * blend;
    }
}

static bVector2 SampleVector2Table(const bVector2 *data, int count, float value, float min_value, float max_value) {
    if (count <= 0) {
        return bVector2(0.0f, 0.0f);
    }
    if (value <= min_value) {
        return data[0];
    }
    if (value >= max_value) {
        return data[count - 1];
    }

    {
        float scaled = (value - min_value) * (static_cast<float>(count - 1) / (max_value - min_value));
        int index = static_cast<int>(bFloor(scaled));
        float blend = scaled - static_cast<float>(index);
        bVector2 sample;

        if (index < 0) {
            index = 0;
        }
        if (index >= count - 1) {
            index = count - 2;
        }

        sample.x = data[index].x + (data[index + 1].x - data[index].x) * blend;
        sample.y = data[index].y + (data[index + 1].y - data[index].y) * blend;
        return sample;
    }
}

bool OutsidePovType(int pov_type) {
    return (static_cast<unsigned int>(pov_type - 2) < 3) || pov_type == 6 || pov_type == 5;
}

bool RenderCarPovType(int pov_type, bool look_back) {
    return (static_cast<unsigned int>(pov_type - 2) < 3) || (pov_type == 1 && !look_back) || pov_type == 6 || pov_type == 5;
}

static inline void ResetCubic1DState(tCubic1D *cubic) {
    cubic->Val = cubic->ValDesired;
    cubic->dVal = cubic->dValDesired;
    cubic->state = 0;
}

static inline void ResetCubic3DState(tCubic3D *cubic) {
    ResetCubic1DState(&cubic->x);
    ResetCubic1DState(&cubic->y);
    ResetCubic1DState(&cubic->z);
}

template <> void tTable<float>::Blend(float *dest, float *a, float *b, float blend_a) {
    *dest = *a * blend_a + *b * (1.0f - blend_a);
}

template <> void tTable<bVector2>::Blend(bVector2 *dest, bVector2 *a, bVector2 *b, float blend_a) {
    float blend_b = 1.0f - blend_a;
    dest->x = a->x * blend_a + b->x * blend_b;
    dest->y = a->y * blend_a + b->y * blend_b;
}

CameraAnchor::CameraAnchor(int model)
    : mVelMag(0.0f), //
      mTopSpeed(0.0f), //
      mModel(0), //
      mWorldID(0), //
      mSurface(), //
      mCollisionDamping(0.0f), //
      mDrift(0.0f), //
      mGroundCollision(0.0f), //
      mObjectCollision(0.0f), //
      mIsNosEngaged(false), //
      mIsBrakeEngaged(false), //
      mIsDragRace(false), //
      mIsOverRev(false), //
      mIsTouchingGround(false), //
      mIsVehicleDestroyed(false), //
      mIsGearChanging(false), //
      mIsCloseToRoadBlock(false), //
      mZoom(1.0f), //
      mModelAttributes(Attrib::FindCollection(Attrib::Gen::ecar::ClassKey(), 0xeec2271a), 0, nullptr), //
      mCameraInfoAttributes(Attrib::FindCollection(Attrib::Gen::camerainfo::ClassKey(), 0xeec2271a), 0, nullptr) {
    mVelocity.x = 0.0f;
    mVelocity.y = 0.0f;
    mVelocity.z = 0.0f;
    mGeomPos.x = 0.0f;
    mGeomPos.y = 0.0f;
    mGeomPos.z = 0.0f;
    mDimension.x = 0.0f;
    mDimension.y = 0.0f;
    mDimension.z = 0.0f;
    mAccel.x = 0.0f;
    mAccel.y = 0.0f;
    mAccel.z = 0.0f;
    mPOV.Type = 3;
    mPOV.Angle = bDegToAng(mCameraInfoAttributes.ANGLE(0));
    mPOV.Lag = mCameraInfoAttributes.LAG(0);
    mPOV.Height = mCameraInfoAttributes.HEIGHT(0);
    mPOV.LatOffset = mCameraInfoAttributes.LATOFFSET(0);
    mPOV.Fov = bDegToAng(mCameraInfoAttributes.FOV(0));
    mPOV.Stiffness = mCameraInfoAttributes.STIFFNESS(0);
    mPOV.AllowTilting = mCameraInfoAttributes.TILTING(0);
    SetModel(model);
    bIdentity(&mGeomRot);
}

CameraAnchor::~CameraAnchor() {}

void CameraAnchor::SetModel(int model) {
    if (mModel != model) {
        CarTypeInfo *info = GetCarTypeInfoFromHash(model);
        const char *name;

        if (info != nullptr) {
            name = info->CarTypeName;
        } else {
            name = "";
        }

        mModel = model;
        mModelAttributes.ChangeWithDefault(Attrib::StringToLowerCaseKey(name));
    }
}

POV *CameraAnchor::GetPov(int pov_type) {
    unsigned int attrib_key;

    mPOV.Type = static_cast<short>(pov_type);

    switch (pov_type) {
        case 0:
            attrib_key = 0x585517f3;
            break;
        case 1:
            attrib_key = 0xd74c1435;
            break;
        case 2:
            attrib_key = 0x0c2da793;
            break;
        case 3:
            attrib_key = 0xccf03cb3;
            break;
        case 4:
            attrib_key = 0x10204a90;
            break;
        case 5:
            attrib_key = 0x4b675dc8;
            break;
        case 6:
            attrib_key = 0xd76a6fad;
            break;
        default:
            mPOV.Type = 3;
            mCameraInfoAttributes.Change(Attrib::FindCollection(Attrib::Gen::camerainfo::ClassKey(), 0xeec2271a));
            goto after_camerainfo;
    }

    {
        const Attrib::RefSpec *refspec = reinterpret_cast<const Attrib::RefSpec *>(mModelAttributes.GetAttributePointer(attrib_key, 0));

        if (refspec == nullptr) {
            refspec = reinterpret_cast<const Attrib::RefSpec *>(Attrib::DefaultDataArea(sizeof(Attrib::RefSpec)));
        }

        mCameraInfoAttributes.ChangeWithDefault(*refspec);
    }

after_camerainfo:
    {
        float zoom;
        if (mZoom > 1.0f) {
            zoom = mZoom;
        } else {
            zoom = 1.0f;
        }
        unsigned int index = eGetCurrentViewMode() == 3;

        mPOV.Angle = bDegToAng(mCameraInfoAttributes.ANGLE(index));
        mPOV.Lag = mCameraInfoAttributes.LAG(index) / zoom;
        mPOV.Height = mCameraInfoAttributes.HEIGHT(index);
        mPOV.LatOffset = mCameraInfoAttributes.LATOFFSET(index);
        mPOV.Fov = bDegToAng(mCameraInfoAttributes.FOV(index) * zoom);
        mPOV.AllowTilting = static_cast<short>(mCameraInfoAttributes.TILTING(index));
        mPOV.Stiffness = mCameraInfoAttributes.STIFFNESS(index);
    }

    return &mPOV;
}

void CameraAnchor::Update(float dT, const bMatrix4 &matrix, const bVector3 &velocity, const bVector3 &forward) {
    float dist = bDistBetween(&mGeomPos, reinterpret_cast<const bVector3 *>(&matrix.v3));

    PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(&matrix), *reinterpret_cast<Mtx44 *>(&mGeomRot));
    float savedVelMag = mVelMag;
    mGeomRot.v3.z = 0.0f;
    mGeomRot.v3.y = 0.0f;
    mGeomRot.v3.x = 0.0f;

    mGeomPos.x = matrix.v3.x;
    mGeomPos.y = matrix.v3.y;
    mGeomPos.z = matrix.v3.z;
    mVelocity = velocity;

    mVelMag = bLength(&velocity);

    if (dT > 0.0f && dist / dT < 200.0f) {
        bVector3 acc((mVelMag - savedVelMag) / dT, 0.0f, 0.0f);
        bMulMatrix(&mAccel, &mGeomRot, &acc);
    } else {
        mAccel.x = 0.0f;
        mAccel.z = 0.0f;
        mAccel.y = 0.0f;
    }
}

void RenderCameraMovers(eView *view) {
    CameraMover *head = view->CameraMoverList.GetHead();
    int no_camera_mover = head == view->CameraMoverList.EndOfList();
    CameraMover *camera_mover = nullptr;

    if (!no_camera_mover) {
        camera_mover = head;
    }
    if (camera_mover != nullptr) {
        camera_mover = nullptr;
        if (!no_camera_mover) {
            camera_mover = head;
        }
        camera_mover->Render(view);
    }
}

void CameraMoverRestartRace() {
    WeHaveCheckedIfJR2ServerExists = 0;
    CameraAI::Reset();

    for (int view_id = 1; view_id < 4; view_id++) {
        eView *view = eGetView(view_id, false);

        if (view == nullptr) {
            continue;
        }

        CameraMover *camera_mover = view->GetCameraMover();
        if (camera_mover) {
            camera_mover->ResetState();
        }
    }
}

Camera *GetCurrentCamera() {
    Camera *camera = 0;

    if (&eViews[1] != 0) {
        camera = eViews[1].pCamera;
    }

    if (camera != 0) {
        return camera;
    }

    return 0;
}

static bool sHavePrevPosition;
static bVector3 sPrevPosition;

void UpdateCameraMovers(float dT) {
    for (int view_id = 0; view_id < 22; view_id++) {
        eView *view = eGetView(view_id, false);
        CameraMover *m = view->GetCameraMover();

        if (m != nullptr) {
            m->Update(dT);
        }
    }

    if (WeHaveCheckedIfJR2ServerExists == 0) {
        JR2ServerExists = bFunkDoesServerExist("JR2Server");
        WeHaveCheckedIfJR2ServerExists = 1;
    }

    {
        eView *view = eGetView(1, false);
        Camera *camera = view->GetCamera();

        if (JR2ServerExists != 0 && camera != nullptr) {
            if (bAbs(RealTime - LastUpdateTimeJR2) > 0x10) {
                LastUpdateTimeJR2 = RealTime;
                camera->CommunicateWithJollyRancher(const_cast<char *>("Player1Camera"));
            }
        }

        if (RemoteCaffeinating != 0 && DisableCommunication == 0 && camera != nullptr) {
            if (bAbs(RealTime - LastUpdateTimeCaffeine) > 0x10) {
                LongVector fix_eye;
                LongVector fix_look;

                bVector3 look = *camera->GetDirection() * 0.01f;
                bVector3 eye = *camera->GetPosition() - look;

                LastUpdateTimeCaffeine = RealTime;

                MakeLongVector(&fix_eye, &eye, 100.0f);
                MakeLongVector(&fix_look, camera->GetPosition(), 100.0f);
                espSetCameraPositionFix(&fix_eye, &fix_look);

                if (!sHavePrevPosition) {
                    sPrevPosition = bVector3(0.0f, 0.0f, 0.0f);
                    sHavePrevPosition = true;
                }

                if (bDistBetween(&sPrevPosition, camera->GetPosition()) > 0.01f) {
                    sPrevPosition = *camera->GetPosition();
                }

                bLength(reinterpret_cast<const bVector2 *>(camera->GetPosition()));
                espCentrePlaneView(camera->GetPosition());
            }
        }
    }

    if ((!GManager::Exists() || !GManager::Get().GetIsWarping()) &&
        (!GRaceStatus::Exists() || !GRaceStatus::Get().GetIsScriptWaitingForLoading())) {
        bool set_any_positions = false;

        for (int view_id = 1; view_id < 3; view_id++) {
            eView *view = eGetView(view_id, false);

            if (view->Active != 0) {
                CameraMover *camera_mover = view->GetCameraMover();

                if (camera_mover != nullptr) {
                    if (!set_any_positions) {
                        TheTrackStreamer.ClearStreamingPositions();
                        set_any_positions = true;
                    }

                    bVector3 pos;
                    bVector3 vel;
                    bVector3 dir;

                    pos = *view->GetCamera()->GetPosition();
                    vel = *view->GetCamera()->GetVelocityPosition();
                    dir = *view->GetCamera()->GetDirection();

                    if (bStreamingPositionFromICE != 0) {
                        INIS *nis = INIS::Get();
                        if (nis != nullptr) {
                            bConvertFromBond(pos, reinterpret_cast<const bVector3 &>(*nis->GetStartLocation()));
                        }

                        dir.z = vel.z;
                        vel.x = 0.0f;
                        vel.y = 0.0f;
                        dir.x = 0.0f;
                        dir.y = 0.0f;
                    }

                    bool following_car = false;
                    CameraMover *mover = view->GetCameraMover();
                    if (mover->GetType() == CM_DRIVE_CUBIC) {
                        following_car = true;
                    }

                    int pos_num = (view_id == 2);
                    TheTrackStreamer.PredictStreamingPosition(
                        pos_num, &pos, &vel, &dir, following_car);
                }
            }
        }
    }
}

bool DoesCameraTypeDisablePreculler(CameraMoverTypes type) {
    if (type == CM_DEBUG_WORLD) {
        return true;
    }
    return type == CM_TRACK_CAR;
}

CameraMover::CameraMover(int view_id, CameraMoverTypes type)
    : mWPos(0.025f) {
    mCollider = WCollider::Create(0, WCollider::kColliderShape_Sphere, 0x1C, 0);
    Type = type;
    Enabled = 0;
    ViewID = view_id;
    fAccumulatedClearance = 0.0f;
    fAccumulatedAdjust = 0.0f;
    fSavedAdjust = 0.0f;
    vSavedForward.y = 0.0f;
    vSavedForward.x = 0.0f;
    vSavedForward.z = 0.0f;
    if (view_id == -1) {
        RenderDash = 0;
        pView = nullptr;
        pCamera = nullptr;
    } else {
        pView = eGetView(view_id, false);
        pCamera = pView->GetCamera();
        pCamera->SetFarZ(12000.0f);
        RenderDash = pCamera->GetRenderDash();
        Enable();
    }

    if (DoesCameraTypeDisablePreculler(Type)) {
        DisablePrecullerCounter++;
    }
}

CameraMover::~CameraMover() {
    WCollider::Destroy(mCollider);
    if (DoesCameraTypeDisablePreculler(Type)) {
        DisablePrecullerCounter--;
    }
    Disable();
}

void CameraMover::Update(float dT) {}

void CameraMover::Render(eView *view) {}

CameraAnchor *CameraMover::GetAnchor() {
    return nullptr;
}

bool CameraMover::OutsidePOV() {
    return true;
}

bool CameraMover::RenderCarPOV() {
    return true;
}

void CameraMover::Enable() {
    if (Enabled == 0) {
        Enabled = 1;
        if (Camera::StopUpdating == 0) {
            pCamera->SetRenderDash(RenderDash);
        }
        pView->AttachCameraMover(this);
        pCamera->SetNearZ(0.5f);
    }
}

void CameraMover::Disable() {
    if (Enabled != 0) {
        Enabled = 0;
        RenderDash = pCamera->GetRenderDash();
        pView->UnattachCameraMover(this);
    }
}

bool CameraMover::OnWCollide(const WCollisionMgr::WorldCollisionInfo &cInfo, const UMath::Vector3 &cPoint, void *userdata) {
    if (userdata) {
        bVector3 vPoint;
        eSwizzleWorldVector(cPoint, vPoint);
    }
    return true;
}

bool CameraMover::IsSomethingInBetween(const UMath::Vector4 &start, const UMath::Vector4 &end) {
    UMath::Vector4 seg[2];
    WCollisionMgr::WorldCollisionInfo cInfo;
    WCollisionMgr collision_mgr(0, 3);

    seg[0] = start;
    seg[1] = end;
    seg[0].y += 0.1f;
    seg[1].y += 0.1f;
    collision_mgr.CheckHitWorld(seg, cInfo, 3);
    return cInfo.HitSomething();
}

bool CameraMover::IsSomethingInBetween(const bVector3 *start, const bVector3 *end) {
    UMath::Vector4 p0;
    UMath::Vector4 p1;
    eUnSwizzleWorldVector(*start, reinterpret_cast<bVector3 &>(p0));
    eUnSwizzleWorldVector(*end, reinterpret_cast<bVector3 &>(p1));
    return IsSomethingInBetween(p0, p1);
}

float CameraMover::MinDistToWall() {
    return 0.7f;
}

bool CameraMover::EnforceMinGapToWalls(WCollider *collider, bVector3 *pCarPos, bVector3 *pCameraPos, bVector4 *pAdjust) {
    bool bViolates = false;
    bVector3 vAdjust(0.0f, 0.0f, 0.0f);

    {
        float fovScale = bClamp(bAngToDeg(pCamera->GetFieldOfView()) * (1.0f / 180.0f) * 2.0f, 0.0f, 1.0f);

        bVector3 camDir = *pCarPos - *pCameraPos;
        float camDist = bLength(&camDir);

        if (camDist >= 0.1f) {
            bVector3 camVec = bNormalize(camDir);
            bVector3 upVec(0.0f, 1.0f, 0.0f);
            bVector3 leftVec;
            bCross(&leftVec, &upVec, &camVec);

            const int kProbeSize = 2;
            bVector3 camVecProbe[2];

            float sideMargin = fovScale * camDist * 0.5f * 0.85f;
            bScaleAdd(&camVecProbe[0], &camDir, &leftVec, sideMargin);
            bScaleAdd(&camVecProbe[1], &camDir, &leftVec, -sideMargin);

            float clearance = 0.0f;
            for (int i = 0; i < kProbeSize; ++i) {
                bVector3 probeVec;
                bNormalize(&probeVec, &camVecProbe[i]);

                bVector3 vCameraPosBackClearance;
                bScaleAdd(&vCameraPosBackClearance, pCameraPos, &probeVec, -0.5f);

                UMath::Vector4 camSeg[2];
                eUnSwizzleWorldVector(*pCarPos, reinterpret_cast<bVector3 &>(camSeg[0]));
                eUnSwizzleWorldVector(vCameraPosBackClearance, reinterpret_cast<bVector3 &>(camSeg[1]));
                camSeg[0].w = 0.0f;
                camSeg[1].w = 0.0f;

                WCollisionMgr::WorldCollisionInfo cInfo;
                WCollisionMgr collision_mgr(0, 3);

                if (collision_mgr.CheckHitWorld(camSeg, cInfo, 2)) {
                    bViolates = true;
                    bVector3 collisionPt;
                    bVector3 collisionNorm;
                    eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(cInfo.fCollidePt), collisionPt);
                    eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(cInfo.fNormal), collisionNorm);

                    float dot = camDir.x * collisionNorm.x + camDir.y * collisionNorm.y + camDir.z * collisionNorm.z;
                    float normalFactor = (1.0f - bAbs(dot)) * 2.0f + 1.0f;
                    float oldDist = bDistBetween(pCarPos, &vCameraPosBackClearance);
                    float newDist = bDistBetween(pCarPos, &collisionPt);
                    float newClearance = (oldDist - newDist) * normalFactor;
                    if (clearance < newClearance) {
                        clearance = newClearance;
                    }
                }
            }

            float fSmoothed = (fAccumulatedClearance + clearance) * 0.5f;
            fAccumulatedClearance = fAccumulatedClearance + (clearance - fSmoothed);
            if (0.0f < fSmoothed) {
                vAdjust.x = camVec.x * fSmoothed;
                vAdjust.y = camVec.y * fSmoothed;
                vAdjust.z = camVec.z * fSmoothed;
            }

            pAdjust->w = 0.0f;
            pAdjust->x = vAdjust.x;
            pAdjust->y = vAdjust.y;
            pAdjust->z = vAdjust.z;
        }
    }

    return bViolates;
}

unsigned short CameraMover::GetLookbackAngle() {
    return 0;
}

void CameraMover::ResetState() {}

unsigned int CameraMover::GetAnchorID() {
    CameraAnchor *anchor = GetAnchor();
    if (anchor) {
        return anchor->GetWorldID();
    }
    return 0;
}

bVector3 *CameraMover::GetTarget() {
    return pCamera->GetTarget();
}

void CameraMover::HandheldNoise(bMatrix4 *world_to_camera, float f_scale, bool useWorldTimer) {
    if (f_scale <= 0.0f) {
        return;
    }

    bVector4 v_frequency;
    bVector4 v_magnitude;
    bScale(&v_frequency, &CameraNoiseHandheldFrequency, 1.0f);
    bScale(&v_magnitude, &CameraNoiseHandheldAmplitude, f_scale);

    pCamera->SetNoiseFrequency1(&v_frequency);
    pCamera->SetNoiseAmplitude1(&v_magnitude);
    float time = useWorldTimer ? WorldTimer.GetSeconds() : RealTimer.GetSeconds();
    pCamera->ApplyNoise(world_to_camera, time, 1.0f);
}

void CameraMover::ChopperNoise(bMatrix4 *world_to_camera, float f_scale, bool useWorldTimer) {
    if (f_scale <= 0.0f) {
        return;
    }

    const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_ALL);

    for (IVehicle::List::const_iterator iter = vehicles.begin(); iter != vehicles.end(); ++iter) {
        IVehicle *vehicle = *iter;

        if (vehicle != nullptr && vehicle->GetVehicleClass() == VehicleClass::CHOPPER) {
            bVector3 bpos;
            bVector3 dir;
            float distance;

            eSwizzleWorldVector(vehicle->GetPosition(), bpos);
            bSub(&dir, &bpos, pCamera->GetPosition());
            dir.z = 0.0f;
            distance = bLength(&dir);

            if (distance < 100.0f) {
                float intensity = f_scale * (1.0f - distance * 0.01f);
                bVector4 v_frequency;
                bVector4 v_magnitude;
                float time;

                bScale(&v_frequency, &CameraNoiseChopperFrequency, intensity);
                bScale(&v_magnitude, &CameraNoiseChopperAmplitude, intensity);
                pCamera->SetNoiseFrequency1(&v_frequency);
                pCamera->SetNoiseAmplitude1(&v_magnitude);
                time = useWorldTimer ? WorldTimer.GetSeconds() : RealTimer.GetSeconds();
                pCamera->ApplyNoise(world_to_camera, time, 1.0f);
            }
        }
    }
}

void CameraMover::TerrainVelocityNoise(bMatrix4 *world_to_camera, CameraAnchor *car, float speed_scale, float terrain_scale) {
    if (car == nullptr) {
        return;
    }

    float speed_amount = speed_scale * car->GetVelMag();
    float terrain_amount = terrain_scale * car->GetSurface().CAMERA_NOISE(0);
    float terrain_mix = car->GetSurface().CAMERA_NOISE(1);
    bVector3 accel = *car->GetAccel();
    const bMatrix4 *matrix = car->GetMatrix();
    float accel_forward = accel.x * matrix->v0.x + accel.y * matrix->v1.x + accel.z * matrix->v2.x;
    bVector4 speed_frequency = CameraNoiseSpeedFrequency;
    bVector4 speed_amplitude = CameraNoiseSpeedAmplitude;
    bVector4 terrain_frequency = CameraNoiseTerrainFrequency;
    bVector4 terrain_amplitude = CameraNoiseTerrainAmplitude;

    if (speed_amount > 0.0f) {
        int speed_index = bClamp(static_cast<int>(speed_amount * 0.05f), 0, 4);
        bVector4 speed_sample = CameraNoiseSpeedData[speed_index];

        terrain_amount += bAbs(accel_forward) * 0.25f;
        speed_amount = speed_sample.x * speed_scale + bAbs(accel_forward) * 0.5f;
    }

    if (car->IsDragRace()) {
        speed_amount *= 0.75f;
    }

    if (car->IsOverRev()) {
        speed_amount += 1.0f;
        terrain_amount *= 0.5f;
    }

    if (car->IsNosEngaged() && car->GetVelMag() > 1.0f) {
        speed_amount = 1.5f;
        terrain_amount = 1.0f;
    }

    if (car->IsBrakeEngaged() && car->GetVelMag() > 1.0f) {
        speed_amount = 1.5f;
        terrain_amount = 0.0f;
    }

    if (!RenderCarPOV()) {
        speed_amount *= 0.5f;
        terrain_amount *= 0.5f;
    }

    terrain_amount *= terrain_mix;
    bScale(&speed_frequency, &speed_frequency, terrain_amount);
    bScale(&speed_amplitude, &speed_amplitude, speed_amount);
    bScale(&terrain_frequency, &terrain_frequency, terrain_mix);
    bScale(&terrain_amplitude, &terrain_amplitude, terrain_scale * terrain_amount);

    pCamera->SetNoiseFrequency1(&speed_frequency);
    pCamera->SetNoiseAmplitude1(&speed_amplitude);
    pCamera->SetNoiseFrequency2(&terrain_frequency);
    pCamera->SetNoiseAmplitude2(&terrain_amplitude);
    pCamera->ApplyNoise(world_to_camera, WorldTimer.GetSeconds(), 1.0f);
}

void CameraMover::ComputeBankedUpVector(bVector3 *up, bVector3 *eye, bVector3 *look, unsigned short bank) {
    bMatrix4 axis_rotation;
    bVector3 axis;
    bVector3 new_up(0.0f, 0.0f, 1.0f);

    bSub(&axis, look, eye);
    bNormalize(&axis, &axis);
    eCreateAxisRotationMatrix(&axis_rotation, axis, bank);
    eMulVector(up, &axis_rotation, &new_up);
}

void CameraMover::IsoProjectionMatrix(bMatrix4 *pMatrix, bVector3 *pEye, bVector3 *pLook, bVector2 *pProjection) {
    bVector3 vUp(-pMatrix->v0.y, -pMatrix->v1.y, -pMatrix->v2.y);
    bMatrix4 mWorldToCamera;
    bMatrix4 mCameraToWorld;
    bVector3 vNewLookCameraSpace;
    bVector3 vNewLookWorldSpace;

    eCreateLookAtMatrix(&mWorldToCamera, *pEye, *pLook, vUp);
    eInvertTransformationMatrix(&mCameraToWorld, &mWorldToCamera);
    eMulVector(&vNewLookCameraSpace, &mWorldToCamera, pLook);
    vNewLookCameraSpace.x -= vNewLookCameraSpace.z * pProjection->x;
    vNewLookCameraSpace.y -= vNewLookCameraSpace.z * pProjection->y;
    eMulVector(&vNewLookWorldSpace, &mCameraToWorld, &vNewLookCameraSpace);
    eCreateLookAtMatrix(pMatrix, *pEye, vNewLookWorldSpace, vUp);
}

float CameraMover::AdjustHeightAroundCar(const bVector3 *position, bVector3 *pCarPos, bVector3 *pForward) {
    _STL::list<IBody *, UTL::Std::Allocator<IBody *, _type_CameraAIAvoidables> >::const_iterator iter;

    for (iter = TheAvoidables->begin(); iter != TheAvoidables->end(); ++iter) {
        IBody *car = *iter;
        UMath::Matrix4 umatrix;
        bMatrix4 matrix;

        car->GetTransform(umatrix);
        eSwizzleWorldMatrix(reinterpret_cast<const bMatrix4 &>(umatrix), matrix);

        const bVector3 *car_position = reinterpret_cast<const bVector3 *>(&matrix.v3);
        UMath::Vector3 dim;
        bVector3 box;
        bVector2 eye_to_car = *reinterpret_cast<bVector2 *>(const_cast<bVector3 *>(car_position)) - *reinterpret_cast<const bVector2 *>(position);
        float gap_squared = bDot(&eye_to_car, &eye_to_car);
        float gap_height = bAbs(car_position->z - position->z);

        car->GetDimension(dim);
        bFill(&box, dim.z + 0.85f, dim.x + 0.85f, dim.y + 0.85f);

        float min_gap = bLength(reinterpret_cast<const bVector2 *>(&box));
        float min_gap_squared = min_gap * min_gap;

        if (gap_squared < min_gap_squared && gap_height < box.z + box.z) {
            bMatrix4 mWorldToCar;
            bVector3 vCameraCarSpace;

            eInvertTransformationMatrix(&mWorldToCar, &matrix);
            eMulVector(&vCameraCarSpace, &mWorldToCar, position);

            float cam_x4 = vCameraCarSpace.x * vCameraCarSpace.x;
            float cam_y4 = vCameraCarSpace.y * vCameraCarSpace.y;
            float box_x4 = box.x * box.x;
            float box_y4 = box.y * box.y;
            cam_x4 *= cam_x4;
            cam_y4 *= cam_y4;
            box_x4 *= box_x4;
            box_y4 *= box_y4;
            float m = cam_x4 / box_x4 + cam_y4 / box_y4;

            if (m < 1.0f) {
                float remaining = 1.0f - m;
                float sqrt_remaining = bSqrt(remaining);
                float sqrt_sqrt = bSqrt(sqrt_remaining);
                float new_z = sqrt_sqrt * box.z;

                if (new_z > vCameraCarSpace.z) {
                    vCameraCarSpace.z = new_z;
                    bVector3 vNewCam;
                    eMulVector(&vNewCam, &matrix, &vCameraCarSpace);
                    float zdiff = vNewCam.z - position->z;
                    if (zdiff > min_gap) {
                        zdiff = 0.0f;
                    }
                    return zdiff;
                }
            }
        }
    }
    return 0.0f;
}

bVector3 *CameraMover::DutchAroundCar(bVector3 *pCarPos, bVector3 *pCarVelocity) {
    static bVector3 ret(0.0f, 0.0f, 0.0f);

    ret.x = 0.0f;
    ret.z = 0.0f;
    ret.y = 0.0f;

    {
        _STL::list<IBody *, UTL::Std::Allocator<IBody *, _type_CameraAIAvoidables> >::const_iterator iter;

        for (iter = TheAvoidables->begin(); iter != TheAvoidables->end(); ++iter) {
            IBody *car = *iter;
            UMath::Matrix4 umatrix;
            bMatrix4 matrix;

            car->GetTransform(umatrix);
            eSwizzleWorldMatrix(reinterpret_cast<const bMatrix4 &>(umatrix), matrix);

            const bVector3 *car_position = reinterpret_cast<const bVector3 *>(&matrix.v3);
            bVector3 eye_to_car;
            bSub(&eye_to_car, car_position, pCarPos);
            float gap_squared = bDot(&eye_to_car, &eye_to_car);

            if (gap_squared > 100.0f && gap_squared < 10000.0f) {
                bVector3 unitEyeVelocity;
                bNormalize(&eye_to_car, &eye_to_car);
                bNormalize(&unitEyeVelocity, pCarVelocity);

                float dot = bDot(&eye_to_car, &unitEyeVelocity);

                if (dot > 0.5f) {
                    float gap_factor = bClamp(1000.0f / gap_squared, 0.0f, 1.0f);

                    UMath::Vector3 uvelocity;
                    car->GetLinearVelocity(uvelocity);
                    bVector3 velocity;
                    eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(uvelocity), velocity);

                    float vel_diff = bDistBetween(pCarVelocity, &velocity) - 5.0f;
                    float vel_factor = bClamp(vel_diff * 0.1f, 0.0f, 1.0f);

                    bVector3 unitVelocity;
                    bNormalize(&unitVelocity, &velocity);
                    bNormalize(&unitEyeVelocity, pCarVelocity);

                    float vel_dot = bDot(&unitEyeVelocity, &unitVelocity);
                    float dot_factor = 0.0f;
                    if (vel_dot < 0.0f) {
                        dot_factor = -vel_dot;
                    }

                    bScaleAdd(&ret, &ret, &eye_to_car, gap_factor * (vel_factor + dot_factor));
                }
            }
        }
    }

    return &ret;
}

int CameraMover::MinGapCars(bMatrix4 *pMatrix, bVector3 *pLook, bVector3 *pForward) {
    bMatrix4 mCameraToWorld;

    eInvertTransformationMatrix(&mCameraToWorld, pMatrix);
    bool ret = false;
    bVector3 *pCameraPos = reinterpret_cast<bVector3 *>(&mCameraToWorld.v3);
    float old_z = pCameraPos->z;
    int i = 0;
    bVector3 vCarCameraSpace;
    double adjust;

    for (;;) {
        adjust = AdjustHeightAroundCar(pCameraPos, pLook, pForward);
        if (i > 15) break;
        if (adjust <= 0.0f) break;
        ret = true;
        i++;
        pCameraPos->z += adjust;
        AdjustHeightAroundCar(pCameraPos, pLook, pForward);
    }

    {
        float fwd_dot = bDot(&vSavedForward, reinterpret_cast<const bVector3 *>(pMatrix));
        bCopy(&vSavedForward, reinterpret_cast<const bVector3 *>(pMatrix));

        float total_adjust = pCameraPos->z - old_z;
        float speed2D = bLength(reinterpret_cast<const bVector2 *>(pForward));

        if (speed2D < 1.0f && fwd_dot > 0.9f && total_adjust < fSavedAdjust) {
            total_adjust = fSavedAdjust;
        }

        fSavedAdjust = total_adjust;
        float avg = (fAccumulatedAdjust + total_adjust) * 0.5f;
        fAccumulatedAdjust += total_adjust - avg;
        pCameraPos->z = old_z + avg;
    }

    eMulVector(&vCarCameraSpace, pMatrix, pLook);

    if (vCarCameraSpace.z > 0.0f) {
        bVector2 vProjection(vCarCameraSpace.x / vCarCameraSpace.z, vCarCameraSpace.y / vCarCameraSpace.z);
        IsoProjectionMatrix(pMatrix, pCameraPos, pLook, &vProjection);
    } else {
        ret = false;
    }

    return ret;
}

int CameraMover::MinGapTopology(bMatrix4 *pMatrix, bVector3 *pLook) {
    bMatrix4 inv_matrix;
    bVector3 eye;
    bVector3 look_camera_space;

    eInvertTransformationMatrix(&inv_matrix, pMatrix);
    eye.x = pMatrix->v3.x;
    eye.y = pMatrix->v3.y;
    eye.z = pMatrix->v3.z;

    if (eye.z < pLook->z + 0.5f) {
        eye.z = pLook->z + 0.5f;
    }

    eMulVector(&look_camera_space, pMatrix, pLook);
    if (look_camera_space.z <= 0.0f) {
        return 0;
    }

    {
        bVector2 projection(look_camera_space.x / look_camera_space.z, look_camera_space.y / look_camera_space.z);
        IsoProjectionMatrix(pMatrix, &eye, pLook, &projection);
    }

    return 1;
}

void CameraMover::FovCubicInit(tCubic1D *cubic) {
    float fov = static_cast<float>(pCamera->GetFov()) + static_cast<float>(pCamera->GetVelocityFov()) * (1.0f / 30.0f);
    float fov_velocity = static_cast<float>(pCamera->GetVelocityFov()) * cubic->duration;

    cubic->SetVal(fov);
    cubic->SetdVal(fov_velocity);
}

void CameraMover::EyeCubicInit(tCubic3D *pEye, bMatrix4 *pMatrix, bVector3 *pVelocity) {
    bVector3 vEye;
    bScaleAdd(&vEye, pCamera->GetPosition(), pCamera->GetVelocityPosition(), 1.0f / 30.0f);

    if (pMatrix != nullptr) {
        bMulMatrix(&vEye, pMatrix, &vEye);
    }

    pEye->SetVal(&vEye);

    bVector3 vEyeRel(*pCamera->GetVelocityPosition());

    if (pVelocity != nullptr) {
        bSub(&vEyeRel, &vEyeRel, pVelocity);
    }

    bVector4 vEyeVel;
    bScale(reinterpret_cast<bVector3 *>(&vEyeVel), &vEyeRel, pEye->x.duration);
    vEyeVel.w = 0.0f;

    if (pMatrix != nullptr) {
        bMulMatrix(&vEyeVel, pMatrix, &vEyeVel);
    }

    pEye->SetdVal(reinterpret_cast<bVector3 *>(&vEyeVel));
}

void CameraMover::LookCubicInit(tCubic3D *pLook, bMatrix4 *pMatrix, bVector3 *pVelocity) {
    bVector3 vLook;
    bScaleAdd(&vLook, pCamera->GetTarget(), pCamera->GetVelocityTarget(), 1.0f / 30.0f);

    if (pMatrix != nullptr) {
        bMulMatrix(&vLook, pMatrix, &vLook);
    }

    pLook->SetVal(&vLook);

    bVector3 vLookRel(*pCamera->GetVelocityTarget());

    if (pVelocity != nullptr) {
        bSub(&vLookRel, &vLookRel, pVelocity);
    }

    bVector4 vLookVel;
    bScale(reinterpret_cast<bVector3 *>(&vLookVel), &vLookRel, pLook->x.duration);
    vLookVel.w = 0.0f;

    if (pMatrix != nullptr) {
        bMulMatrix(&vLookVel, pMatrix, &vLookVel);
    }

    pLook->SetdVal(reinterpret_cast<bVector3 *>(&vLookVel));
}

void CameraMover::SetEyeLook(tCubic3D *eye, tCubic3D *look, tCubic1D *fov, bMatrix4 *matrix, bVector3 *target) {
    FovCubicInit(fov);
    EyeCubicInit(eye, matrix, target);
    LookCubicInit(look, matrix, target);
}

CameraAnchor *CubicCameraMover::GetAnchor() {
    return pCar;
}

void CubicCameraMover::SetLookBack(bool b) {
    bLookBack = b;
}

void CubicCameraMover::SetDisableLag(bool disable) {
    bAccelLag = !disable;
}

bool CubicCameraMover::OutsidePOV() {
    return OutsidePovType(nPovTypeUsed);
}

bool CubicCameraMover::RenderCarPOV() {
    return RenderCarPovTypeRaw(nPovTypeUsed, bLookBack);
}

float CubicCameraMover::MinDistToWall() {
    return 0.7f;
}

bool CubicCameraMover::HighliteMode() {
    return false;
}

void CubicCameraMover::SetSnapNext() {
    if (fIgnoreSetSnapNextTimer > 0.0f) {
        return;
    }
    bSnapNext = 1;
}

unsigned short CubicCameraMover::GetLookbackAngle() {
    if (bLookBack == 0) {
        return 0;
    }
    return 0x8000;
}

bool CubicCameraMover::IsHoodCamera() {
    bool is_hood_camera = false;

    if (nPovTypeUsed == 1) {
        is_hood_camera = !bLookBack;
    }
    return is_hood_camera;
}

void CubicCameraMover::SetForward(POV *pov, bool bSnap) {
    if (pov != nullptr && OutsidePovType(pov->Type)) {
        if (!bSnap && HighliteMode()) {
            return;
        }

        bVector3 v(*pCar->GetVelocity());
        const bVector3 *pFwd = pCar->GetForwardVector();
        float fDot = bDot(&v, pFwd);

        if (fDot < 0.0f) {
            bScaleAdd(&v, &v, pFwd, fDot * -2.0f);
        }

        float fDrift = gDriftSpeed.GetValue(pCar->GetVelocityMagnitude());
        bNormalize(&v, &v);
        bScale(&v, &v, fDrift);
        bScaleAdd(&v, &v, pFwd, 1.0f - fDrift);

        float fSeconds = (WorldTimer - tLastGrounded).GetSeconds();
        float z = bClamp(1.0f - fSeconds * 0.5f, 0.0f, 1.0f);
        v.z *= z;
        v.z *= 1.0f - pCar->GetForwardVector()->z * pCar->GetForwardVector()->z;

        pForward->SetValDesired(&v);

        if (!bSnap) {
            return;
        }
    } else {
        if (pCar == nullptr) {
            return;
        }
        pForward->SetValDesired(pCar->GetForwardVector());
    }

    pForward->Snap();
}

void CubicCameraMover::CameraAccelCurve(bVector3 *pAccel) {
    tTable<float> accel_table(CameraAccelerationCurve, 5, -30.0f, 30.0f);
    accel_table.GetValue(&pAccel->x, pCar->GetAcceleration()->x);
    accel_table.GetValue(&pAccel->y, pCar->GetAcceleration()->y);
    accel_table.GetValue(&pAccel->z, pCar->GetAcceleration()->z);
    bScale(pAccel, pAccel, 30.0f);
}

void CubicCameraMover::CameraSpeedHug(bVector3 *pEyeOffset) {
    if (pCar == nullptr) {
        return;
    }

    float f_top_speed = pCar->GetTopSpeed();
    if (f_top_speed > 0.0f) {
        tTable<bVector2> speed_table(CameraSpeedHugData, 5, 0.0f, f_top_speed);
        bVector2 v_speed_hug;
        speed_table.GetValue(&v_speed_hug, pCar->GetVelocityMagnitude());

        pEyeOffset->x *= v_speed_hug.x;
        pEyeOffset->z *= v_speed_hug.y;
    }
}

void CubicCameraMover::MakeSpace(bMatrix4 *pMatrix) {
    if (OutsidePOV()) {
        bIdentity(pMatrix);

        bVector3 vForward(pForward->x.Val, pForward->y.Val, pForward->z.Val);
        bNormalize(reinterpret_cast<bVector3 *>(&pMatrix->v0), &vForward);
        bCross(reinterpret_cast<bVector3 *>(&pMatrix->v1), reinterpret_cast<const bVector3 *>(&pMatrix->v2), reinterpret_cast<const bVector3 *>(&pMatrix->v0));
        bCross(reinterpret_cast<bVector3 *>(&pMatrix->v2), reinterpret_cast<const bVector3 *>(&pMatrix->v0), reinterpret_cast<const bVector3 *>(&pMatrix->v1));
        bCopy(&pMatrix->v3, pCar->GetGeometryPosition(), 1.0f);
    } else {
        bCopy(pMatrix, pCar->GetGeometryOrientation(), pCar->GetGeometryPosition());

        if (pCar->GetGeometryOrientation()->v2.z < 0.5f) {
            pMatrix->v1.x = -pMatrix->v1.x;
            pMatrix->v1.y = -pMatrix->v1.y;
            pMatrix->v1.z = -pMatrix->v1.z;
            pMatrix->v1.w = -pMatrix->v1.w;
            pMatrix->v2.x = -pMatrix->v2.x;
            pMatrix->v2.y = -pMatrix->v2.y;
            pMatrix->v2.z = -pMatrix->v2.z;
            pMatrix->v2.w = -pMatrix->v2.w;
        }
    }
}

CubicCameraMover::~CubicCameraMover() {
    delete pUp;
    delete pFov;
    delete pEye;
    delete pLook;
    delete pForward;
    delete reinterpret_cast<AverageBase *>(pAvgAccel);
}

void CubicCameraMover::SetPovType(int pov_type) {
    if (pov_type != nPovTypeUsed) {
        bool old_outside = OutsidePovType(nPovTypeUsed);
        bool new_outside = OutsidePovType(pov_type);

        bSnapNext = !!(bSnapNext | (!new_outside || !old_outside));
        nPovType = pov_type;
    }
}

void CubicCameraMover::ResetState() {
    ResetCubic3DState(pUp);
    ResetCubic1DState(pFov);
    ResetCubic3DState(pEye);
    ResetCubic3DState(pLook);
    ResetCubic3DState(pForward);
    GetCamera()->ClearVelocity();
    vCameraImpcat.y = 0.0f;
    vCameraImpcat.x = 0.0f;
    vCameraImpcatTimer.y = 0.0f;
    vCameraImpcatTimer.x = 0.0f;
}

Bezier::Bezier()
    : pControlPoints(nullptr) {
    mBasis.v0.x = 1.0f;
    mBasis.v0.y = -3.0f;
    mBasis.v0.z = 3.0f;
    mBasis.v0.w = -1.0f;
    mBasis.v1.x = 0.0f;
    mBasis.v1.y = 3.0f;
    mBasis.v1.z = -6.0f;
    mBasis.v1.w = 3.0f;
    mBasis.v2.x = 0.0f;
    mBasis.v2.y = 0.0f;
    mBasis.v2.z = 3.0f;
    mBasis.v2.w = -3.0f;
    mBasis.v3.x = 0.0f;
    mBasis.v3.y = 0.0f;
    mBasis.v3.z = 0.0f;
    mBasis.v3.w = 1.0f;
}

void Bezier::GetPoint(bVector3 *pPoint, float parameter) {
    if (pControlPoints != nullptr) {
        bVector4 basis;
        bVector4 v;
        float t = 1.0f - parameter;
        float t2 = t * t;

        basis.x = t * t2;
        basis.y = t2;
        basis.z = t;
        basis.w = 1.0f;

        bMulMatrix(&v, &mBasis, &basis);
        bMulMatrix(reinterpret_cast<bVector4 *>(pPoint), pControlPoints, &v);
    }
}

RearViewMirrorCameraMover::RearViewMirrorCameraMover(int view_id, CameraAnchor *car)
    : CameraMover(view_id, CM_REAR_VIEW_MIRROR),
      pCar(car) {
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetRenderDash(0);
    }
}

RearViewMirrorCameraMover::~RearViewMirrorCameraMover() {}

void RearViewMirrorCameraMover::Update(float dT) {
    if (FEDatabase->GetGameplaySettings()->RearviewOn) {
        bMatrix4 m;
        bMatrix4 tbod;
        bMatrix4 CarRotMat;
        bMatrix4 rvm_matrix;

        eIdentity(&m);
        m.v3.x = -pCar->GetGeometryPosition()->x;
        m.v3.y = -pCar->GetGeometryPosition()->y;
        m.v3.z = -pCar->GetGeometryPosition()->z;
        PSMTX44Copy(*reinterpret_cast<const Mtx44 *>(pCar->GetGeometryOrientation()), *reinterpret_cast<Mtx44 *>(&CarRotMat));
        eTransposeMatrix(&tbod, &CarRotMat);
        eRotateX(&tbod, &tbod, 0x4000);
        eRotateY(&tbod, &tbod, 0x4000);
        eRotateZ(&tbod, &tbod, 0);
        eMulMatrix(&rvm_matrix, &m, &tbod);
        rvm_matrix.v3.x += RVMOffsetInCar.x;
        rvm_matrix.v3.y += RVMOffsetInCar.y;
        rvm_matrix.v3.z += RVMOffsetInCar.z;
        if (Camera::StopUpdating == 0) {
            GetCamera()->SetFieldOfView(20000);
        }
        GetCamera()->SetNearZ(RVMnearz);
        GetCamera()->SetFarZ(RVMfarz);
        ApplyCameraShake(ViewID, &rvm_matrix);
        GetCamera()->SetCameraMatrix(rvm_matrix, dT);
    }
}

TrackCarCameraMover::TrackCarCameraMover(int nView, CameraAnchor *pCar, bool focus_effects)
    : CameraMover(nView, CM_TRACK_CAR),
      CarToFollow(pCar),
      FocalDistCubic(1, 1.0f),
      FocusEffects(focus_effects),
      CameraType(0) {
    Init();
}

void TrackCarCameraMover::Init() {
    bVector3 offset;

    CameraType = 0;
    Eye = *CarToFollow->GetGeomPos();
    Look = *CarToFollow->GetGeomPos();

    Eye.z += TrackCarIsoZoomDistance[CameraType];
    offset.x = TrackCarLookOffsetX[CameraType];
    offset.y = TrackCarLookOffsetY[CameraType];
    offset.z = TrackCarLookOffsetZ[CameraType];
    eMulVector(&offset, CarToFollow->GetMatrix(), &offset);

    Look.x += offset.x;
    Look.y += offset.y;
    Look.z += offset.z;
    FocalDistCubic.Val = 0.0f;
    FocalDistCubic.ValDesired = 0.0f;
    FocalDistCubic.dVal = 0.0f;
    FocalDistCubic.dValDesired = 0.0f;
    GetCamera()->ClearVelocity();

    if (Camera::StopUpdating == 0) {
        GetCamera()->SetTargetDistance(bDistBetween(&Eye, CarToFollow->GetGeomPos()));
    }
}

TrackCarCameraMover::~TrackCarCameraMover() {
    GetCamera()->SetSimTimeMultiplier(1.0f);
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetFocalDistance(0.0f);
    }
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetDepthOfField(0.0f);
    }
    GetCamera()->ClearVelocity();
}

void TrackCarCameraMover::Update(float dT) {
    if (!TheGameFlowManager.IsPaused()) {
        bVector3 world_up(0.0f, 0.0f, 1.0f);
        bVector3 to_car;
        bVector3 offset;
        bMatrix4 camera_matrix;
        float dist;
        float inv_dist;
        float focal_distance;

        GetCamera()->SetSimTimeMultiplier(1.0f);

        to_car.x = Eye.x - CarToFollow->GetGeomPos()->x;
        to_car.y = Eye.y - CarToFollow->GetGeomPos()->y;
        to_car.z = Eye.z - CarToFollow->GetGeomPos()->z;
        dist = bLength(&to_car);

        if (dist > 0.0f) {
            unsigned short fov = bATan(dist, TrackCarIsoZoomDistance[CameraType]);
            unsigned int fov_limit = bMax(800, static_cast<int>((fov & 0x7fff) << 1));

            if (fov_limit > 0x332c) {
                fov_limit = 0x332c;
            }
            if (Camera::StopUpdating == 0 && (fov & 0x7fff) != 0) {
                GetCamera()->SetFieldOfView(static_cast<unsigned short>(fov_limit));
            }
        }

        Look = *CarToFollow->GetGeomPos();
        inv_dist = dist > 1.0f ? 1.0f / dist : 1.0f;
        to_car.x *= inv_dist;
        to_car.y *= inv_dist;
        to_car.z *= inv_dist;
        world_up = bCross(to_car, world_up);

        offset.x = TrackCarLookOffsetX[CameraType];
        offset.y = TrackCarLookOffsetY[CameraType];
        offset.z = TrackCarLookOffsetZ[CameraType];
        eMulVector(&offset, CarToFollow->GetMatrix(), &offset);

        Look.x += offset.x;
        Look.y += offset.y;
        Look.z += offset.z;

        eCreateLookAtMatrix(&camera_matrix, Eye, Look, world_up);
        focal_distance = bDistBetween(CarToFollow->GetGeomPos(), &Eye);
        if (Camera::StopUpdating == 0) {
            GetCamera()->SetTargetDistance(focal_distance);
        }

        FocalDistCubic.dVal = FocalDistCubic.Val * 0.1f;
        SplineSeek(&FocalDistCubic, dT, 0.0f, 0.0f);
        focal_distance = bMax(1.0f, focal_distance + FocalDistCubic.Val);

        if (FocusEffects) {
            if (Camera::StopUpdating == 0) {
                GetCamera()->SetFocalDistance(focal_distance + FocalDistCubic.Val);
            }
            if (Camera::StopUpdating == 0) {
                GetCamera()->SetDepthOfField(2.0f);
            }
        }

        GetCamera()->SetCameraMatrix(camera_matrix, dT);
        if (IsSomethingInBetween(GetCamera()->GetPosition(), CarToFollow->GetGeomPos())) {
            CameraAI::MaybeKillJumpCam(CarToFollow->GetWorldID());
        }
    }
}

TrackCopCameraMover::TrackCopCameraMover(int nView, CameraAnchor *pCar, bool focus_effects)
    : CameraMover(nView, CM_TRACK_COP),
      CarToFollow(pCar),
      FocalDistCubic(1, 1.0f),
      FocusEffects(focus_effects),
      bRenderCarPOV(true) {
    ZoomSplineParam = 0.0f;
    EyeSplineParam = 0.0f;
    LookSplineParam = 0.0f;
    Init();
}

TrackCopCameraMover::~TrackCopCameraMover() {
    TrackCopCameraMover_IdleSim = false;
    GetCamera()->SetSimTimeMultiplier(1.0f);
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetFocalDistance(0.0f);
    }
    if (Camera::StopUpdating == 0) {
        GetCamera()->SetDepthOfField(0.0f);
    }
    GetCamera()->ClearVelocity();
}

bool TrackCopCameraMover::FindPursuitVehiclePosition(bVector3 *pPosition) {
    bool found = false;
    float best_distance = 10000.0f;
    const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_ALL);

    for (IVehicle::List::const_iterator iter = vehicles.begin(); iter != vehicles.end(); ++iter) {
        IVehicle *vehicle = *iter;

        if (vehicle != nullptr && vehicle->GetVehicleClass() == VehicleClass::CAR) {
            bVector3 render_position;
            float distance;

            eSwizzleWorldVector(vehicle->GetPosition(), render_position);
            if (IsSomethingInBetween(GetCamera()->GetPosition(), &render_position)) {
                continue;
            }

            distance = bDistBetween(CarToFollow->GetGeomPos(), &render_position);
            if (distance < best_distance) {
                *pPosition = render_position;
                best_distance = distance;
                found = true;
            }
        }
    }

    return found;
}

void TrackCopCameraMover::Init() {
    bVector3 pursuit_position;
    bVector3 eye = *GetCamera()->GetPosition();
    bVector3 look = *GetCamera()->GetTarget();
    float fov = static_cast<float>(GetCamera()->GetFieldOfView());

    TrackCopCameraMover_IdleSim = true;
    HideEverySingleHud();
    bRenderCarPOV = true;

    ZoomVerts.v0.x = fov;
    ZoomVerts.v0.y = 0.0f;
    ZoomVerts.v0.z = 0.0f;
    ZoomVerts.v0.w = 1.0f;
    ZoomVerts.v1 = ZoomVerts.v0;
    ZoomVerts.v2 = ZoomVerts.v0;
    ZoomVerts.v3 = ZoomVerts.v0;

    EyeVerts.v0.x = eye.x;
    EyeVerts.v0.y = eye.y;
    EyeVerts.v0.z = eye.z;
    EyeVerts.v0.w = 1.0f;
    EyeVerts.v1 = EyeVerts.v0;
    EyeVerts.v2 = EyeVerts.v0;
    EyeVerts.v3 = EyeVerts.v0;

    LookVerts.v0.x = look.x;
    LookVerts.v0.y = look.y;
    LookVerts.v0.z = look.z;
    LookVerts.v0.w = 1.0f;
    LookVerts.v1 = LookVerts.v0;
    LookVerts.v2 = LookVerts.v0;
    LookVerts.v3 = LookVerts.v0;

    if (FindPursuitVehiclePosition(&pursuit_position)) {
        bVector3 chase_eye = pursuit_position;
        bVector3 chase_look = *CarToFollow->GetGeomPos();

        chase_eye.z += 2.0f;
        EyeVerts.v1.x = chase_eye.x;
        EyeVerts.v1.y = chase_eye.y;
        EyeVerts.v1.z = chase_eye.z;
        EyeVerts.v2 = EyeVerts.v1;
        EyeVerts.v3 = EyeVerts.v1;

        LookVerts.v1.x = chase_look.x;
        LookVerts.v1.y = chase_look.y;
        LookVerts.v1.z = chase_look.z;
        LookVerts.v2 = LookVerts.v1;
        LookVerts.v3 = LookVerts.v1;

        ZoomVerts.v1.x = fov;
        ZoomVerts.v2.x = fov;
        ZoomVerts.v3.x = fov;
        bRenderCarPOV = false;
    } else {
        CameraAI::MaybeKillPursuitCam(CarToFollow->GetWorldID());
    }

    EyeSpline.SetControlPoints(&EyeVerts);
    LookSpline.SetControlPoints(&LookVerts);
    ZoomSpline.SetControlPoints(&ZoomVerts);
    EyeSplineParam = 0.0f;
    LookSplineParam = 0.0f;
    ZoomSplineParam = 0.0f;
    FocalDistCubic.Val = 0.0f;
    FocalDistCubic.ValDesired = 0.0f;
    FocalDistCubic.dVal = 0.0f;
    FocalDistCubic.dValDesired = 0.0f;

    if (FocusEffects == 0 && Camera::StopUpdating == 0) {
        GetCamera()->SetFocalDistance(0.0f);
        GetCamera()->SetDepthOfField(0.0f);
    }
}

void TrackCopCameraMover::Update(float dT) {
    if (FEManager::Get() == nullptr || !FEManager::ShouldPauseSimulation(true)) {
        bVector3 world_up(0.0f, 0.0f, 1.0f);
        bVector3 look;
        bVector3 eye;
        bVector3 offset;
        bMatrix4 camera_matrix;
        float focal_distance;

        EyeSplineParam += dT * 0.25f;
        if (EyeSplineParam > 1.0f) {
            EyeSplineParam = 1.0f;
        }
        EyeSpline.GetPoint(&eye, EyeSplineParam);

        LookSplineParam += dT * 0.25f;
        if (LookSplineParam > 1.0f) {
            LookSplineParam = 1.0f;
        }
        LookSpline.GetPoint(&look, LookSplineParam);

        ZoomSplineParam += dT * 0.25f;
        if (ZoomSplineParam > 1.0f) {
            ZoomSplineParam = 1.0f;
        }
        ZoomSpline.GetPoint(reinterpret_cast<bVector3 *>(&offset), ZoomSplineParam);

        if (Camera::StopUpdating == 0) {
            GetCamera()->SetFieldOfView(static_cast<unsigned short>(bDegToAng(offset.x)));
        }

        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = 0.0f;
        eMulVector(&offset, CarToFollow->GetMatrix(), &offset);
        look.x += offset.x;
        look.y += offset.y;
        look.z += offset.z;

        eCreateLookAtMatrix(&camera_matrix, eye, look, world_up);
        focal_distance = bDistBetween(CarToFollow->GetGeomPos(), &eye);
        if (Camera::StopUpdating == 0) {
            GetCamera()->SetTargetDistance(focal_distance);
        }

        FocalDistCubic.dVal = FocalDistCubic.Val * 0.1f;
        SplineSeek(&FocalDistCubic, dT, 0.0f, 0.0f);
        focal_distance = bMax(1.0f, focal_distance + FocalDistCubic.Val);

        if (FocusEffects) {
            if (Camera::StopUpdating == 0) {
                GetCamera()->SetFocalDistance(focal_distance + FocalDistCubic.Val);
            }
            if (Camera::StopUpdating == 0) {
                GetCamera()->SetDepthOfField(2.0f);
            }
        }

        GetCamera()->SetCameraMatrix(camera_matrix, dT);
    }
}

CameraAnchor *RearViewMirrorCameraMover::GetAnchor() {
    return pCar;
}

CameraAnchor *TrackCarCameraMover::GetAnchor() {
    return CarToFollow;
}

bVector3 *TrackCarCameraMover::GetTarget() {
    CameraAnchor *car = CarToFollow;
    if (car != nullptr) {
        return car->GetGeomPos();
    }
    return GetCamera()->GetTarget();
}

CameraAnchor *TrackCopCameraMover::GetAnchor() {
    return CarToFollow;
}

bool TrackCopCameraMover::RenderCarPOV() {
    return bRenderCarPOV;
}

bVector3 *TrackCopCameraMover::GetTarget() {
    CameraAnchor *car = CarToFollow;
    if (car != nullptr) {
        return car->GetGeomPos();
    }
    return GetCamera()->GetTarget();
}
