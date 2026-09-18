#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Camera/Movers/Rearview.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/Generated/Messages/MJumpCut.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Misc/Rumble.hpp"
#include "Speed/Indep/Src/Physics/PVehicle.h"
#include "Speed/Indep/Src/Sim/Collision.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/World/VisualTreatment.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/mtx.h>
#endif

extern int gCamCloseToRoadBlock;

static float kCinematicMomementSeconds = 2.0f;
int gCinematicMomementCamera = 0;
int gGameBreakerCamera = 0;

int gCamCloseToRoadBlock = 0; // .data: 0x80416568

static int old_pov = -1;

class CDActionDrive :                       public CameraAI::Action,
                      public IAttachable,
                      public Sim::Collision::IListener,
                      public ITrafficCenter {
  public:
    static CameraAI::Action *Construct(CameraAI::Director *director);

    CDActionDrive(CameraAI::Director *director, IPlayer *player);
    ~CDActionDrive() override;

    void AquireCar();
    void OnCarDetached();
    void MessageJumpCut(const MJumpCut &message);

    void Reset() override;
    const Attrib::StringKey &GetName() const override {
        static Attrib::StringKey name("CDActionDrive");
        return name;
    }
    Attrib::StringKey GetNext() const override {
        return Attrib::StringKey();
    }
    CameraMover *GetMover() override {
        return mMover;
    }
    void Update(float dT) override;
    void SetSpecial(float scale) override {
        if (scale <= 0.0f) {
            return;
        }
        kCinematicMomementSeconds = scale;
        mCinematicRising = 1;
    }
    bool Attach(UTL::COM::IUnknown *pOther) override {
        return mAttachments->Attach(pOther);
    }
    bool Detach(UTL::COM::IUnknown *pOther) override {
        return mAttachments->Detach(pOther);
    }
    bool IsAttached(const UTL::COM::IUnknown *pOther) const override {
        return mAttachments->IsAttached(pOther);
    }
    void OnAttached(IAttachable *pOther) override {}
    void OnDetached(IAttachable *pOther) override;
    const IAttachable::List *GetAttachments() const override {
        return (const IAttachable::List *)mAttachments;
    }
    bool GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) override;
    void OnCollision(const Sim::Collision::Info &cinfo) override;

    USE_FASTALLOC(CDActionDrive)

  private:
    CameraMover *mMover;
    CameraMover *mRearView;
    CameraAnchor *mAnchor;
    WorldConn::Reference mTarget;
    IPlayer *mPlayer;
    Sim::Attachments *mAttachments;
    IVehicle *mVehicle;
    float mGameBreakerTimer;
    EVIEW_ID mView;
    float mWorldHitTimer;
    float mGroundHitTimer;
    float mObjectHitTimer;
    float mCollisionScale;
    float mPulseTimer;
    float mCinematicTimer;
    int mCinematicRising;
    int mLastGear;
    Hermes::HHANDLER mJumpCutHandler;
};

UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionDrive("CDActionDrive", CDActionDrive::Construct);

CameraAI::Action *CDActionDrive::Construct(CameraAI::Director *director) {
    IPlayer *found = 0;

    for (IPlayer::List::const_iterator it = IPlayer::GetList(PLAYER_LOCAL).begin(); it != IPlayer::GetList(PLAYER_LOCAL).end(); ++it) {
        IPlayer *player = *it;
        if (player->GetRenderPort() == director->GetView()) {
            found = player;
            break;
        }
    }

    if (!found) {
        return 0;
    }

    if (!found->GetSettings()) {
        return 0;
    }

    ISimable *simable = found->GetSimable();
    if (simable) {
        unsigned int world_id = simable->GetWorldID();
        if (world_id == 0) {
            return 0;
        }

        return new CDActionDrive(director, found);
    }

    return 0;
}

CDActionDrive::CDActionDrive(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(0), IAttachable(this), mTarget(0), mPlayer(player), mVehicle(0), mGameBreakerTimer(0.0f), mView(director->GetView()),
      mWorldHitTimer(0.0f), mGroundHitTimer(0.0f), mObjectHitTimer(0.0f), mCollisionScale(2.0f), mPulseTimer(0.0f), mCinematicTimer(0.0f),
      mCinematicRising(0), mLastGear(0) {
    gCinematicMomementCamera = 0;
    gGameBreakerCamera = 0;
    if (director->IsCinematicMomement()) {
        gCinematicMomementCamera = 1;
        mCinematicTimer = 1.0f;
        kCinematicMomementSeconds = 2.0f;
        mPulseTimer = 0.3f;
    }

    mAttachments = new Sim::Attachments(this);
    mJumpCutHandler =
        Hermes::Handler::Create<MJumpCut, CDActionDrive, CDActionDrive>(this, &CDActionDrive::MessageJumpCut, "CameraMessagePort", 0);
    mAttachments->Attach(mPlayer);

    bool smooth = 0;
    CameraMover *prev = director->GetMover();
    if (prev && prev->GetType() == CM_ICE) {
        smooth = TheICEManager.GetSmoothExit();
    }

    mAnchor = new ("CameraAnchor", 0) CameraAnchor(0);
    if (mCinematicTimer > 0.0f) {
        mAnchor->SetZoom(1.0f - mCinematicTimer * 0.4f);
    }

    AquireCar();
    if (mTarget.IsValid()) {
        bMatrix4 mat;
        bMatrixCopy(mat, *mTarget.GetMatrix());
        ICollisionBody *cbody = 0;
        if (mVehicle && mVehicle->QueryInterface(&cbody)) {
            IRigidBody *body = mVehicle->GetSimable()->GetRigidBody();
            UVector3 cg = cbody->GetCenterOfGravity();
            body->ConvertLocalToWorld(cg, false);
            VU0_v3add(cg, body->GetPosition(), cg);
            bSetPosition(mat.v3, cg);
        }
        mAnchor->Update(0.0f, mat, *mTarget.GetVelocity(), *mTarget.GetAcceleration());
    }

    mMover = new ("CubicCameraMover", 0) CubicCameraMover(director->GetView(), mAnchor, player->GetSettings()->CurCam, smooth, false, false, true);
    mRearView = new ("RearViewMirrorCameraMover", 0) RearViewMirrorCameraMover(EVIEW_PLAYER1_RVM, mAnchor);
}

CDActionDrive::~CDActionDrive() {
    if (mJumpCutHandler) {
        Hermes::Handler::Destroy(mJumpCutHandler);
    }
    if (mPlayer) {
        mAttachments->Detach(mPlayer);
    }
    if (mVehicle) {
        mAttachments->Detach(mVehicle);
    }
    delete mRearView;
    delete mMover;
    delete mAnchor;
    delete mAttachments;
    Sim::Collision::RemoveListener(this);
}

void CDActionDrive::Reset() {
    mGameBreakerTimer = 0.0f;
    mWorldHitTimer = 0.0f;
    mGroundHitTimer = 0.0f;
    mObjectHitTimer = 0.0f;
    mPulseTimer = 0.0f;
    mCinematicRising = 0;
    mLastGear = 0;
    mCinematicTimer = 0.0f;
}

void CDActionDrive::OnDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(pOther, mPlayer)) {
        mPlayer = 0;
    }
    if (UTL::COM::ComparePtr(pOther, mVehicle)) {
        OnCarDetached();
    }
}

void CDActionDrive::OnCarDetached() {
    Sim::Collision::RemoveListener(this, mVehicle);
    if (mTarget.IsValid()) {
        mTarget.Set(0);
    }
    if (mAnchor) {
        mAnchor->SetWorldID(0);
    }
    mVehicle = 0;
}

void CDActionDrive::OnCollision(const Sim::Collision::Info &cinfo) {
    float speed = VU0_sqrt(VU0_v3lengthsquare(cinfo.closingVel));
    switch (cinfo.Type()) {
    case Sim::Collision::Info::OBJECT: {
        if (speed <= 20.0f) {
            break;
        }
        float objecttime = (speed - 20.0f) * 0.01f;
        objecttime = UMath::Min(objecttime, 1.0f);
        objecttime *= mCollisionScale;
        mObjectHitTimer = UMath::Max(objecttime, mObjectHitTimer);
        break;
    }
    case Sim::Collision::Info::WORLD: {
        if (speed <= 5.0f) {
            break;
        }
        float damptime = (speed - 5.0f) * 0.025f;
        damptime = UMath::Min(damptime, 1.0f);
        damptime *= mCollisionScale;
        mWorldHitTimer = UMath::Max(damptime, mWorldHitTimer);
        break;
    }
    case Sim::Collision::Info::GROUND: {
        if (speed <= 3.0f) {
            break;
        }
        float groundtime = (speed - 3.0f) * 0.05f;
        groundtime = UMath::Min(groundtime, 1.0f);
        groundtime *= mCollisionScale;
        mGroundHitTimer = UMath::Max(groundtime, mGroundHitTimer);
        break;
    }
    default:
        break;
    }
}

void CDActionDrive::AquireCar() {
    if (!mPlayer) {
        return;
    }

    if (!UTL::COM::ComparePtr(mPlayer->GetSimable(), mVehicle)) {
        if (mVehicle) {
            Detach(mVehicle);
            mVehicle = 0;
        }
    }
    if (mVehicle) {
        return;
    }

    ISimable *simable = mPlayer->GetSimable();
    if (!simable) {
        return;
    }

    mTarget.Set(simable->GetWorldID());
    if (!mTarget.IsValid()) {
        return;
    }

    if (!simable->QueryInterface(&mVehicle)) {
        return;
    }

    Attach(mVehicle);
    Sim::Collision::AddListener(this, mVehicle, "CDActionDrive");

    mAnchor->SetModel(bStringHash(mVehicle->GetVehicleAttributes().MODEL().GetString()));
    mAnchor->SetWorldID(mTarget.GetWorldID());

    IRigidBody *body = simable->GetRigidBody();
    UMath::Vector3 dim = body->GetDimension();
    bVector3 bdim(*reinterpret_cast<bVector3 *>(&dim));
    mAnchor->SetDimension(bdim);

    ITransmission *trans = 0;
    if (mVehicle->QueryInterface(&trans)) {
        mAnchor->SetTopSpeed(trans->GetMaxSpeedometer());
        mLastGear = trans->GetGear();
    }
}

void CDActionDrive::Update(float dT) {
    bool isBeingPursued;
    IVehicleAI *ivehicleai;
    IInput *iinput;
    PlayerSettings *settings;

    gCinematicMomementCamera = 0;
    gGameBreakerCamera = 0;

    mWorldHitTimer -= dT;
    if (mWorldHitTimer < 0.0f) {
        mWorldHitTimer = 0.0f;
    }
    mGroundHitTimer -= dT;
    if (mGroundHitTimer < 0.0f) {
        mGroundHitTimer = 0.0f;
    }
    mObjectHitTimer -= dT;
    if (mObjectHitTimer < 0.0f) {
        mObjectHitTimer = 0.0f;
    }

    if (!mPlayer) {
        if (mVehicle) {
            Detach(mVehicle);
            mVehicle = 0;
        }
        return;
    }

    AquireCar();
    mAnchor->SetVehicleDestroyed(false);
    if (mVehicle && mVehicle->IsDestroyed()) {
        mAnchor->SetVehicleDestroyed(true);
    }

    if (mMover->OutsidePOV() && GRaceStatus::Exists() && GRaceStatus::Get().GetIsTimeLimited() &&
        GRaceStatus::Get().GetRaceTimeRemaining() <= 0.0f) {
        ISimable *playerSim = mPlayer->GetSimable();
        GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(playerSim);
        if (racerInfo && !racerInfo->IsFinishedRacing()) {
            return;
        }
    }

    if (GRaceStatus::Exists()) {
        ISimable *playerSim = mPlayer->GetSimable();
        GRacerInfo *racerInfo = GRaceStatus::Get().GetRacerInfo(playerSim);
        if (racerInfo && racerInfo->GetCameraDetached()) {
            return;
        }
    }

    isBeingPursued = false;
    if (false) {
        IPerpetrator *iperp;
        const IPlayer::List &playerList = IPlayer::GetList(PLAYER_LOCAL);
        for (IPlayer::List::const_iterator iter = playerList.begin(); iter != playerList.end(); ++iter) {
            IPlayer *ip = *iter;
            if (ip) {
                ISimable *simable = ip->GetSimable();
                if (simable && simable->QueryInterface(&iperp)) {
                    isBeingPursued = iperp->IsBeingPursued();
                }
            }
        }
    }
    mAnchor->SetCloseToRoadBlock(isBeingPursued);
    ivehicleai = mVehicle->GetAIVehiclePtr();
    if (ivehicleai) {
        IPursuit *ipursuit = ivehicleai->GetPursuit();
        if (ipursuit) {
            float distance = 0.0f;
            IVehicle *cop = ipursuit->GetNearestCopInRoadblock(&distance);
            if (cop && distance > 3.0f && distance < mAnchor->GetVelocityMagnitude() * 0.25f) {
                for (IVehicle::List::const_iterator iter = IVehicle::GetList(VEHICLE_AICOPS).begin();
                     iter != IVehicle::GetList(VEHICLE_AICOPS).end(); ++iter) {
                    IVehicle *p_car = *iter;
                    if (p_car && p_car->IsActive() && p_car->GetVehicleClass() == VehicleClass::CAR) {
                        UVector3 ucoppos(p_car->GetPosition());
                        bVector3 coppos;
                        eSwizzleWorldVector(*reinterpret_cast<const bVector3 *>(&ucoppos), coppos);
                        bVector3 copdir;
                        bSub(&copdir, &coppos, mAnchor->GetGeometryPosition());
                        float copdist = bLength(&copdir);
                        if (copdist > 3.0f && copdist < mAnchor->GetVelocityMagnitude() * 0.25f) {
                            bVector3 unitcopdir;
                            bNormalize(&unitcopdir, &copdir);
                            float dot = bDot(&unitcopdir, mAnchor->GetForwardVector());
                            if (dot > 0.7f) {
                                dot = bClamp(dot, 0.0f, 1.0f);
                                float s = bSqrt(1.0f - dot * dot);
                                bVector2 target(copdir.y * s, copdir.x * s);
                                float targetsize = bLength(target);
                                if (targetsize < 2.75f) {
                                    mAnchor->SetCloseToRoadBlock(true);
                                    gCamCloseToRoadBlock = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (mVehicle && mVehicle->QueryInterface(&iinput)) {
        mMover->SetLookBack(false);
        if (iinput->IsLookBackButtonPressed()) {
            if (!mVehicle->IsStaging() && !isBeingPursued) {
                mMover->SetLookBack(true);
            }
        }
        mAnchor->SetBrakeEngaged(false);
        if (iinput->GetControls().fHandBrake > 0.0f || iinput->GetControls().fBrake > 0.0f) {
            mAnchor->SetBrakeEngaged(true);
        }
    }

    if (mPulseTimer > 0.0f) {
        mPulseTimer -= dT;
        if (mPulseTimer > 0.0f && mPulseTimer < 0.05f) {
            mPulseTimer = 0.0f;
            IVisualTreatment *vt = IVisualTreatment::Get();
            if (vt) {
                vt->TriggerPulse(0.2f);
            }
        }
    }

    if (mCinematicRising) {
        if (mCinematicTimer < 1.0f) {
            mCinematicTimer += dT * 4.0f / kCinematicMomementSeconds;
        } else {
            mCinematicTimer = 1.0f;
        }
    } else if (mCinematicTimer > 0.0f) {
        mCinematicTimer -= dT / kCinematicMomementSeconds;
    } else {
        mCinematicTimer = 0.0f;
    }
    if (mCinematicTimer > 0.001f) {
        gCinematicMomementCamera = 1;
    }
    if (gCinematicMomementCamera) {
        Camera *camera = mMover->GetCamera();
        if (camera) {
            float timeScale = (1.0f - mCinematicTimer) * 0.75f + 0.25f;
            if (timeScale > 0.9f) {
                timeScale = 1.0f;
            }
            camera->SetSimTimeMultiplier(timeScale);
        }
    }

    if (mPlayer->InGameBreaker()) {
        mGameBreakerTimer += dT * 4.0f;
        mGameBreakerTimer = UMath::Min(mGameBreakerTimer, 1.0f);
    } else {
        mGameBreakerTimer -= dT;
        mGameBreakerTimer = UMath::Max(mGameBreakerTimer, 0.0f);
    }
    if (mGameBreakerTimer > 0.001f) {
        gGameBreakerCamera = 1;
    }

    settings = mPlayer->GetSettings();
    if (settings) {
        int pov_type = GetPOVTypeFromPlayerCamera(settings->CurCam);
        if (mVehicle && mVehicle->QueryInterface(&iinput)) {
            if (iinput->IsPullBackButtonPressed() || (iinput->IsLookBackButtonPressed() && isBeingPursued)) {
                old_pov = pov_type;
                mMover->SetPovType(POV_PURSUIT);
            } else if (old_pov > -1) {
                mMover->SetPovType(old_pov);
                old_pov = -1;
            } else {
                mMover->SetPovType(pov_type);
            }
        } else {
            mMover->SetPovType(pov_type);
        }
    }

    if (mTarget.IsValid()) {
        ICollisionBody *irbc;
        ISimable *isimable;
        ITransmission *itrans;

        if (mVehicle) {
            mAnchor->SetDragRace(mVehicle->GetDriverStyle() == STYLE_DRAG);
        }

        bMatrix4 mat;
        bMatrixCopy(mat, *mTarget.GetMatrix());
        mAnchor->SetSurface(SimSurface::kNull);
        mAnchor->SetTouchingGround(false);

        if (mVehicle && mVehicle->QueryInterface(&irbc)) {
            {
                IRigidBody *irb = mVehicle->GetSimable()->GetRigidBody();
                UVector3 cg = irbc->GetCenterOfGravity();
                cg.x = 0.0f;
                irb->ConvertLocalToWorld(cg, false);
                cg += irb->GetPosition();
                eSwizzleWorldVector(*reinterpret_cast<const bVector3 *>(&cg), *reinterpret_cast<bVector3 *>(&mat.v3));

                ISuspension *isuspension;
                if (mVehicle->QueryInterface(&isuspension)) {
                    if (isuspension->GetNumWheels() == static_cast<unsigned int>(isuspension->GetNumWheelsOnGround())) {
                        mAnchor->SetSurface(SimSurface(mVehicle->GetSimable()->GetWPos().GetSurface()));
                    }
                    if (isuspension->GetNumWheelsOnGround() > 0) {
                        mAnchor->SetTouchingGround(true);
                    }
                }
            }
        }

        mAnchor->SetNosEngaged(false);
        mAnchor->SetOverRev(false);
        if (mVehicle && mVehicle->QueryInterface(&isimable)) {
            {
                IEngine *engine;
                if (isimable->QueryInterface(&engine)) {
                    mAnchor->SetNosEngaged(engine->IsNOSEngaged());
                    mAnchor->SetOverRev(engine->GetRPM() > engine->GetRedline() - 500.0f);
                }
            }
        }

        float drift = 0.0f;
        if (mAnchor->IsTouchingGround() && mVehicle) {
            float slipangle = ANGLE2DEG(UMath::Abs(mVehicle->GetSlipAngle()));
            drift = UMath::Ramp(slipangle, 0.0f, 90.0f);
            drift *= UMath::Ramp(mVehicle->GetAbsoluteSpeed(), 10.0f, 20.0f);
        }
        mAnchor->SetDrift(drift);

        mAnchor->SetGearChanging(false);
        if (mVehicle->QueryInterface(&itrans)) {
            {
                int gear = itrans->GetGear();
                mAnchor->SetGearChanging(gear != mLastGear);
                mLastGear = gear;
            }
        }

        mAnchor->SetCollisionDamping(mWorldHitTimer / mCollisionScale);
        mAnchor->SetGroundCollision(mGroundHitTimer / mCollisionScale);
        mAnchor->SetObjectCollision(mObjectHitTimer / mCollisionScale);

        mAnchor->SetZoom(1.0f - bMax(mGameBreakerTimer, mCinematicTimer) * 0.4f);

        mAnchor->Update(dT, mat, *mTarget.GetVelocity(), *mTarget.GetAcceleration());

        if (static_cast<unsigned int>(mView - 1) <= 1) {
            MaybeCameraShake(mView - 1, mAnchor->GetAcceleration());
            for (IExplosion::List::const_iterator iter = IExplosion::GetList().begin(); iter != IExplosion::GetList().end();
                 ++iter) {
                IExplosion *explosion = *iter;
                const UMath::Vector3 &pos = explosion->GetOrigin();
                bVector3 bpos;
                eSwizzleWorldVector(*reinterpret_cast<const bVector3 *>(&pos), bpos);
                bVector3 dir;
                bSub(&dir, &bpos, mMover->GetCamera()->GetPosition());
                float distance = bLength(&dir);
                bVector3 explosion_dir;
                bVector3 acc;
                if (distance < explosion->GetMaximumRadius() + 2.0f && distance > 0.0f) {
                    explosion_dir = bNormalize(dir);
                    acc = explosion_dir * (-explosion->GetExpansionSpeed() / dT);
                    MaybeCameraShake(mView - 1, &acc);
                }
            }
        }
    }
}

bool CDActionDrive::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    if (!mVehicle) {
        return false;
    }

    IBody *body = 0;
    if (!mVehicle->QueryInterface(&body)) {
        return false;
    }

    body->GetTransform(matrix);
    body->GetLinearVelocity(velocity);
    return true;
}

void CDActionDrive::MessageJumpCut(const MJumpCut &message) {
    if (mAnchor && message.GetAnchorWorldID() == mAnchor->GetWorldID()) {
        static_cast<CubicCameraMover *>(mMover)->SetSnapNext();
    }
}