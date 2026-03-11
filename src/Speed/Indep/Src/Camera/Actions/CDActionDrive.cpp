#include "Speed/Indep/Src/Camera/Actions/CDActionDrive.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/Messages/MJumpCut.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

extern bool gCinematicMomementCamera;
extern bool gGameBreakerCamera;

static float kCinematicMomementSeconds;

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
    IPlayer *ip;
    for (IPlayer *const *iter = IPlayer::GetList(PLAYER_LOCAL).begin(); iter != IPlayer::GetList(PLAYER_LOCAL).end(); ++iter) {
        ip = *iter;
        if (ip->GetControllerPort() == static_cast<int>(director->GetViewID())) {
            player = ip;
            break;
        }
    }

    if (player == nullptr) {
        return nullptr;
    }

    if (player->GetSettingsIndex() == 0) {
        return nullptr;
    }

    ISimable *isimable = player->GetSimable();
    if (isimable == nullptr) {
        return nullptr;
    }

    if (isimable->GetWorldID() == 0) {
        return nullptr;
    }

    return new CDActionDrive(director, player);
}

CDActionDrive::CDActionDrive(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this), //
      mTarget(0), //
      mMaxCollisionTime(0.5f) {
    mPlayer = player;
    mVehicle = nullptr;
    mGameBreakerScale = 0.0f;
    mViewID = director->GetViewID();
    mCinematicMomementTimer = 0.0f;
    mDampCollisionTime = 0.0f;
    mGroundCollisionTime = 0.0f;
    mObjectCollisionTime = 0.0f;
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
    delete mRearViewMirrorMover;
    delete mMover;
    delete mAnchor;
    delete mAttachments;
    Sim::Collision::RemoveListener(static_cast<Sim::Collision::IListener *>(this));
}

void CDActionDrive::Reset() {
    mGear = 0;
    mCinematicMomementTimer = 0.0f;
    mGameBreakerScale = 0.0f;
    mDampCollisionTime = 0.0f;
    mGroundCollisionTime = 0.0f;
    mObjectCollisionTime = 0.0f;
    mPulseTimer = 0.0f;
    mCinematicMomementTimerInc = false;
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
    if (mPlayer == nullptr) {
        return;
    }
    ISimable *isimable = mPlayer->GetSimable();
    if (!ComparePtr(isimable, mVehicle)) {
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
            isimable->QueryInterface(&mVehicle);
            if (mVehicle != nullptr) {
                Attach(mVehicle);
                Sim::Collision::AddListener(static_cast<Sim::Collision::IListener *>(this), mVehicle, "Camera");
                const char *model_str = mVehicle->GetVehicleAttributes().MODEL().GetString();
                mAnchor->SetModel(bStringHash(model_str));
                mAnchor->SetWorldID(mTarget.GetWorldID());
                IRigidBody *body = isimable->GetRigidBody();
                UMath::Vector3 dimension;
                body->GetDimension(dimension);
                mAnchor->SetDimension(dimension);
                ITransmission *itrans = nullptr;
                mVehicle->QueryInterface(&itrans);
                if (itrans != nullptr) {
                    mAnchor->SetTopSpeed(itrans->GetMaxSpeedometer());
                    mGear = itrans->GetGear();
                }
            }
        }
    }
}

void CDActionDrive::Update(float dT) {
    // TODO
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
