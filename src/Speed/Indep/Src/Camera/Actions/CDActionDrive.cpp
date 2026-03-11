#include "Speed/Indep/Src/Camera/Actions/CDActionDrive.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/Messages/MJumpCut.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

extern bool gCinematicMomementCamera;
extern bool gGameBreakerCamera;
extern bool gCamCloseToRoadBlock;

static float kCinematicMomementSeconds;

class IVisualTreatment {
public:
    static IVisualTreatment *Get();
    void TriggerPulse(float length);
};

int GetPOVTypeFromPlayerCamera(ePlayerSettingsCameras cam);
void MaybeCameraShake(int nPlayer, bVector3 *pAccel);

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionDrive("DRIVE", CDActionDrive::Construct);

const Attrib::StringKey &CDActionDrive::GetName() const {
    static Attrib::StringKey name("DRIVE");
    return name;
}

Attrib::StringKey CDActionDrive::GetNext() const {
    return Attrib::StringKey();
}

void CDActionDrive::SetSpecial(float val) {
    if (val > 0.0f) {
        kCinematicMomementSeconds = val;
        mCinematicMomementTimerInc = true;
    }
}

bool CDActionDrive::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionDrive::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionDrive::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionDrive::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionDrive::Construct(CameraAI::Director *director) {
    IPlayer *player = nullptr;
    int player_idx;
    IPlayer *ip;
    for (IPlayer *const *iter = IPlayer::GetList(PLAYER_LOCAL).begin(); iter != IPlayer::GetList(PLAYER_LOCAL).end(); ++iter) {
        ip = *iter;
        if (ip->GetControllerPort() == static_cast<int>(director->GetViewID())) {
            player = ip;
            break;
        }
    }

    if (player == nullptr) {
        goto null_return;
    }

    if (player->GetSettingsIndex() == 0) {
        goto null_return;
    }

    {
        ISimable *isimable = player->GetSimable();
        if (isimable == nullptr) {
            goto null_return;
        }

        unsigned int world_id = isimable->GetWorldID();
        CameraAI::Action *action = nullptr;
        if (world_id != 0) {
            action = new (static_cast<const char *>(0)) CDActionDrive(director, player);
        }
        return action;
    }

null_return:
    return nullptr;
}

CDActionDrive::CDActionDrive(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this), //
      mTarget(0) {
    mPlayer = player;
    mVehicle = nullptr;
    mGameBreakerScale = 0.0f;
    mCinematicMomementTimer = 0.0f;
    mViewID = director->GetViewID();
    mDampCollisionTime = 0.0f;
    mGroundCollisionTime = 0.0f;
    mObjectCollisionTime = 0.0f;
    mMaxCollisionTime = 0.5f;
    mPulseTimer = 0.0f;
    mCinematicMomementTimerInc = false;
    mGear = 0;

    gCinematicMomementCamera = false;
    gGameBreakerCamera = false;
    if (director->IsCinematicMomement()) {
        gCinematicMomementCamera = true;
        mCinematicMomementTimer = 1.0f;
        kCinematicMomementSeconds = mMaxCollisionTime;
        mPulseTimer = 0.5f;
    }

    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));

    mMsgJumpCut = Hermes::Handler::Create<MJumpCut, CDActionDrive, CDActionDrive>(
        this, &CDActionDrive::MessageJumpCut, "Camera", 0);

    mAttachments->Attach(mPlayer);

    bool smooth = false;
    CameraMover *m = director->GetMover();
    if (m != nullptr && m->GetType() == CM_ICE) {
        smooth = TheICEManager.IsSmoothExit();
    }

    mAnchor = new (static_cast<const char *>(0), 0) CameraAnchor(0);

    if (0.0f < mCinematicMomementTimer) {
        mAnchor->SetZoom(1.0f - mCinematicMomementTimer * 0.1f);
    }

    AquireCar();

    if (mTarget.IsValid()) {
        bMatrix4 mat(*mTarget.GetMatrix());

        ICollisionBody *irbc = nullptr;
        if (mVehicle != nullptr && mVehicle->QueryInterface(&irbc)) {
            IRigidBody *irb = mVehicle->GetSimable()->GetRigidBody();
            UVector3 cg(irbc->GetCenterOfGravity());
            irb->ConvertLocalToWorld(cg, false);
            cg += irb->GetPosition();
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(cg), reinterpret_cast<bVector3 &>(mat.v3));
        }

        mAnchor->Update(0.0f, mat, *mTarget.GetVelocity(), *mTarget.GetAcceleration());
    }

    mMover = new (static_cast<const char *>(0), 0) CubicCameraMover(static_cast<int>(director->GetViewID()), mAnchor, player->GetSettings()->CurCam, smooth, false, false, true);
    mRearViewMirrorMover = new (static_cast<const char *>(0), 0) RearViewMirrorCameraMover(3, mAnchor);
}

CDActionDrive::~CDActionDrive() {
    if (mMsgJumpCut) {
        Hermes::Handler::Destroy(mMsgJumpCut);
    }
    if (mPlayer) {
        mAttachments->Detach(mPlayer);
    }
    if (mVehicle) {
        mAttachments->Detach(mVehicle);
    }
    CameraMover *rvm = mRearViewMirrorMover;
    delete rvm;
    CameraMover *m = mMover;
    delete m;
    delete mAnchor;
    delete mAttachments;
    Sim::Collision::RemoveListener(static_cast<Sim::Collision::IListener *>(this));
}

void CDActionDrive::Reset() {
    mGameBreakerScale = 0.0f;
    mDampCollisionTime = 0.0f;
    mGroundCollisionTime = 0.0f;
    mObjectCollisionTime = 0.0f;
    mPulseTimer = 0.0f;
    mCinematicMomementTimerInc = false;
    mCinematicMomementTimer = 0.0f;
    mGear = 0;
}

void CDActionDrive::OnDetached(IAttachable *pOther) {
    if (ComparePtr(pOther, mPlayer)) {
        mPlayer = nullptr;
    }
    if (ComparePtr(pOther, mVehicle)) {
        OnCarDetached();
    }
}

void CDActionDrive::OnCarDetached() {
    Sim::Collision::RemoveListener(static_cast<Sim::Collision::IListener *>(this), mVehicle);
    if (mTarget.IsValid()) {
        mTarget.Set(0);
    }
    if (mAnchor != nullptr) {
        mAnchor->SetWorldID(0);
    }
    mVehicle = nullptr;
}

void CDActionDrive::OnCollision(const Sim::Collision::Info &cinfo) {
    float speed = UMath::Length(cinfo.closingVel);
    switch (cinfo.type) {
    case Sim::Collision::Info::OBJECT: {
        if (speed > 20.0f) {
            float objecttime = UMath::Min((speed - 20.0f) * 0.01f, 1.0f);
            objecttime = UMath::Max(objecttime * mMaxCollisionTime, mObjectCollisionTime);
            mObjectCollisionTime = objecttime;
        }
        break;
    }
    case Sim::Collision::Info::WORLD: {
        if (speed > 5.0f) {
            float damptime = UMath::Min((speed - 5.0f) * 0.025f, 1.0f);
            damptime = UMath::Max(damptime * mMaxCollisionTime, mDampCollisionTime);
            mDampCollisionTime = damptime;
        }
        break;
    }
    case Sim::Collision::Info::GROUND: {
        if (speed > 3.0f) {
            float groundtime = UMath::Min((speed - 3.0f) * 0.05f, 1.0f);
            groundtime = UMath::Max(groundtime * mMaxCollisionTime, mGroundCollisionTime);
            mGroundCollisionTime = groundtime;
        }
        break;
    }
    }
}

void CDActionDrive::AquireCar() {
    ISimable *isimable;

    if (mPlayer == nullptr) {
        return;
    }

    if (!ComparePtr(mPlayer->GetSimable(), mVehicle)) {
        if (mVehicle != nullptr) {
            Detach(mVehicle);
            mVehicle = nullptr;
        }
    }
    if (mVehicle != nullptr) {
        return;
    }
    isimable = mPlayer->GetSimable();
    if (isimable != nullptr) {
        mTarget.Set(isimable->GetWorldID());
        if (mTarget.IsValid()) {
            if (isimable->QueryInterface(&mVehicle)) {
                Attach(mVehicle);
                Sim::Collision::AddListener(static_cast<Sim::Collision::IListener *>(this), mVehicle, "Camera");
                CameraAnchor *anchor = mAnchor;
                const char *model_str = mVehicle->GetVehicleAttributes().MODEL().GetString();
                if (model_str == nullptr) {
                    model_str = "";
                }
                anchor->SetModel(bStringHash(model_str));
                mAnchor->SetWorldID(mTarget.GetWorldID());
                IRigidBody *body = isimable->GetRigidBody();
                UVector3 dimension(body->GetDimension());
                mAnchor->SetDimension(dimension);
                ITransmission *itrans;
                if (mVehicle->QueryInterface(&itrans)) {
                    mAnchor->SetTopSpeed(itrans->GetMaxSpeedometer());
                    mGear = itrans->GetGear();
                }
            }
        }
    }
}

void CDActionDrive::Update(float dT) {
    gCinematicMomementCamera = false;
    gGameBreakerCamera = false;

    mDampCollisionTime -= dT;
    if (mDampCollisionTime < 0.0f) {
        mDampCollisionTime = 0.0f;
    }
    mGroundCollisionTime -= dT;
    if (mGroundCollisionTime < 0.0f) {
        mGroundCollisionTime = 0.0f;
    }
    mObjectCollisionTime -= dT;
    if (mObjectCollisionTime < 0.0f) {
        mObjectCollisionTime = 0.0f;
    }

    if (mPlayer == nullptr) {
        if (mVehicle != nullptr) {
            Detach(mVehicle);
            mVehicle = nullptr;
        }
        return;
    }

    AquireCar();
    mAnchor->SetVehicleDestroyed(false);
    if (mVehicle != nullptr && mVehicle->IsDestroyed()) {
        mAnchor->SetVehicleDestroyed(true);
    }

    if (mMover->OutsidePOV() && GRaceStatus::Exists()) {
        if (false) { // TODO: false
            if (GRaceStatus::Get().GetRaceTimeRemaining() <= 0.0f) {
                ISimable *playerSim = mPlayer->GetSimable();
                GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(playerSim);
                if (racerInfo != nullptr && !false) { // TODO: false
                    return;
                }
            }
        }

        if (GRaceStatus::Exists()) {
            ISimable *playerSim = mPlayer->GetSimable();
            GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(playerSim);
            if (racerInfo != nullptr && false) { // TODO: false
                return;
            }
        }
    }

    bool isBeingPursued = false;
    mAnchor->SetCloseToRoadBlock(isBeingPursued);
    IVehicleAI *ivehicleai = mVehicle->GetAIVehiclePtr();
    if (ivehicleai != nullptr) {
        IPursuit *ipursuit = ivehicleai->GetPursuit();
        if (ipursuit != nullptr) {
            float distance = 0.0f;
            IVehicle *cop = ipursuit->GetNearestCopInRoadblock(&distance);
            if (cop != nullptr && 0.0f < distance && distance < mAnchor->GetVelocityMagnitude() * 3.0f) {
                for (IVehicle *const *iter = IVehicle::GetList(VEHICLE_AICOPS).begin(); iter != IVehicle::GetList(VEHICLE_AICOPS).end(); ++iter) {
                    IVehicle *p_car = *iter;
                    if (p_car != nullptr && p_car->IsActive() && p_car->GetVehicleClass() == VehicleClass::CAR) {
                        UVector3 ucoppos(p_car->GetPosition());
                        bVector3 coppos;
                        eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(ucoppos), coppos);
                        bVector3 copdir;
                        bSub(&copdir, &coppos, mAnchor->GetGeometryPosition());
                        float copdist = bLength(&copdir);

                        if (0.0f < copdist && copdist < mAnchor->GetVelocityMagnitude() * 3.0f) {
                            bVector3 unitcopdir;
                            bNormalize(&unitcopdir, &copdir);
                            float dot = bDot(&unitcopdir, mAnchor->GetForwardVector());
                            if (-0.5f < dot) {
                                float s = bClamp(dot, 0.0f, 1.0f);
                                s = 1.0f - s * s;
                                float targetsize = bSqrt(s);
                                bVector2 target(copdir.z * targetsize, copdir.x * targetsize);
                                if (bLength(&target) < 10.0f) {
                                    mAnchor->SetCloseToRoadBlock(true);
                                    gCamCloseToRoadBlock = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    IInput *iinput;
    if (mVehicle != nullptr && mVehicle->QueryInterface(&iinput)) {
        mMover->SetLookBack(false);
        if (iinput->IsLookBackButtonPressed() && !mVehicle->IsStaging() && !isBeingPursued) {
            mMover->SetLookBack(true);
        }
        mAnchor->SetBrakeEngaged(false);
        if (0.0f < iinput->GetControls().fHandBrake || 0.0f < iinput->GetControls().fBrake) {
            mAnchor->SetBrakeEngaged(true);
        }
    }

    if (0.0f < mPulseTimer) {
        float pulseTime = mPulseTimer - dT;
        mPulseTimer = pulseTime;
        if (0.0f < pulseTime && pulseTime < 0.25f) {
            mPulseTimer = 0.0f;
            IVisualTreatment *ivt = IVisualTreatment::Get();
            if (ivt != nullptr) {
                ivt->TriggerPulse(0.5f);
            }
        }
    }

    float timer;
    if (mCinematicMomementTimerInc) {
        timer = 1.0f;
        if (mCinematicMomementTimer < 1.0f) {
            timer = mCinematicMomementTimer + dT * 2.0f / kCinematicMomementSeconds;
        }
    } else {
        timer = 0.0f;
        if (0.0f < mCinematicMomementTimer) {
            timer = mCinematicMomementTimer - dT / kCinematicMomementSeconds;
        }
    }
    mCinematicMomementTimer = timer;

    if (0.75f < mCinematicMomementTimer) {
        gCinematicMomementCamera = true;
    }

    if (gCinematicMomementCamera) {
        Camera *camera = mMover->GetCamera();
        if (camera != nullptr) {
            float timeScale = (1.0f - mCinematicMomementTimer) * 2.5f + 3.0f;
            if (0.9f < timeScale) {
                timeScale = 1.0f;
            }
            camera->SetSimTimeMultiplier(timeScale);
        }
    }

    float gbScale;
    if (mPlayer->InGameBreaker()) {
        gbScale = dT * 2.0f + mGameBreakerScale;
        mGameBreakerScale = gbScale;
        gbScale = UMath::Min(gbScale, 1.0f);
    } else {
        gbScale = mGameBreakerScale - dT;
        mGameBreakerScale = gbScale;
        gbScale = UMath::Max(gbScale, 0.0f);
    }
    mGameBreakerScale = gbScale;

    if (0.75f < mGameBreakerScale) {
        gGameBreakerCamera = true;
    }

    {
        PlayerSettings *settings = mPlayer->GetSettings();
        if (settings != nullptr) {
            int pov_type = GetPOVTypeFromPlayerCamera(settings->CurCam);

            if (mVehicle == nullptr || !mVehicle->QueryInterface(&iinput)) {
                mMover->SetPovType(pov_type);
            } else {
                static int old_pov = -1;
                if (iinput->IsPullBackButtonPressed()) {
                    old_pov = pov_type;
                    mMover->SetPovType(PSC_PURSUIT);
                } else {
                    if (iinput->IsLookBackButtonPressed() && isBeingPursued) {
                        old_pov = pov_type;
                        mMover->SetPovType(PSC_PURSUIT);
                    } else if (old_pov > -1) {
                        mMover->SetPovType(old_pov);
                        old_pov = -1;
                    } else {
                        mMover->SetPovType(pov_type);
                    }
                }
            }
        }
    }

    if (mTarget.IsValid()) {
        if (mVehicle != nullptr) {
            mAnchor->SetDragRace(mVehicle->GetDriverStyle() == STYLE_DRAG);
        }

        bMatrix4 mat(*mTarget.GetMatrix());

        ICollisionBody *irbc = nullptr;
        mAnchor->SetSurface(SimSurface::kNull);
        mAnchor->SetTouchingGround(false);

        if (mVehicle != nullptr && mVehicle->QueryInterface(&irbc)) {
            IRigidBody *irb = mVehicle->GetSimable()->GetRigidBody();
            UVector3 cg(irbc->GetCenterOfGravity());
            irb->ConvertLocalToWorld(cg, false);
            cg += irb->GetPosition();
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(cg), reinterpret_cast<bVector3 &>(mat.v3));

            ISuspension *isuspension;
            if (mVehicle->QueryInterface(&isuspension)) {
                if (isuspension->GetNumWheels() == isuspension->GetNumWheelsOnGround()) {
                    ISimable *isimable = mVehicle->GetSimable();
                    WWorldPos &wpos = isimable->GetWPos();
                    SimSurface surf(wpos.GetSurface());
                    mAnchor->SetSurface(surf);
                }
                if (isuspension->GetNumWheelsOnGround() > 0) {
                    mAnchor->SetTouchingGround(true);
                }
            }
        }

        mAnchor->SetNosEngaged(false);
        mAnchor->SetOverRev(false);
        if (mVehicle != nullptr) {
            ISimable *isimable;
            if (mVehicle->QueryInterface(&isimable)) {
                IEngine *iengine;
                if (isimable->QueryInterface(&iengine)) {
                    mAnchor->SetNosEngaged(iengine->IsNOSEngaged());
                    mAnchor->SetOverRev(iengine->GetRPM() > iengine->GetRedline() - 500.0f);
                }
            }
        }

        float drift = 0.0f;
        if (mAnchor->IsTouchingGround() && mVehicle != nullptr) {
            float slipangle = UMath::Abs(mVehicle->GetSlipAngle());
            slipangle = ANGLE2DEG(slipangle);
            drift = UMath::Ramp(slipangle, 5.0f, 45.0f);
            float speedFactor = UMath::Ramp(mVehicle->GetAbsoluteSpeed() - 10.0f, 0.0f, 10.0f);
            drift = drift * speedFactor;
        }
        mAnchor->SetDrift(drift);

        mAnchor->SetGearChanging(false);
        ITransmission *itrans;
        if (mVehicle->QueryInterface(&itrans)) {
            int gear = itrans->GetGear();
            mAnchor->SetGearChanging(gear != mGear);
            mGear = gear;
        }

        mAnchor->SetCollisionDamping(mDampCollisionTime / mMaxCollisionTime);
        mAnchor->SetGroundCollision(mGroundCollisionTime / mMaxCollisionTime);
        mAnchor->SetObjectCollision(mObjectCollisionTime / mMaxCollisionTime);

        float zoom_input = mGameBreakerScale;
        if (mGameBreakerScale - mCinematicMomementTimer < 0.0f) {
            zoom_input = mCinematicMomementTimer;
        }
        mAnchor->SetZoom(1.0f - zoom_input * 0.1f);

        mAnchor->Update(dT, mat, *mTarget.GetVelocity(), *mTarget.GetAcceleration());

        unsigned int viewIndex = mViewID - 1;
        if (viewIndex < 2) {
            MaybeCameraShake(viewIndex, mAnchor->GetAcceleration());

            for (IExplosion *const *iter = IExplosion::GetList().begin(); iter != IExplosion::GetList().end(); ++iter) {
                IExplosion *explosion = *iter;
                const UMath::Vector3 &pos = explosion->GetOrigin();
                bVector3 bpos;
                eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(pos), bpos);
                bVector3 dir;
                bSub(&dir, &bpos, mMover->GetPosition());
                float distance = bLength(&dir);

                float radius = explosion->GetMaximumRadius();
                if (distance < radius + 20.0f && 0.0f < distance) {
                    bVector3 explosion_dir;
                    explosion_dir = bNormalize(dir);
                    float force = -explosion->GetExpansionSpeed() / dT;
                    bVector3 acc;
                    bScale(&acc, &explosion_dir, force);
                    MaybeCameraShake(mViewID - 1, &acc);
                }
            }
        }
    }
}

bool CDActionDrive::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    IBody *ibody;
    if (mVehicle == nullptr) {
        return false;
    }
    if (!mVehicle->QueryInterface(&ibody)) {
        return false;
    }
    ibody->GetTransform(matrix);
    ibody->GetLinearVelocity(velocity);
    return true;
}

void CDActionDrive::MessageJumpCut(const MJumpCut &message) {
    if (mAnchor != nullptr && message.GetAnchorWorldID() == mAnchor->GetWorldID()) {
        static_cast<CubicCameraMover *>(mMover)->SetSnapNext();
    }
}
