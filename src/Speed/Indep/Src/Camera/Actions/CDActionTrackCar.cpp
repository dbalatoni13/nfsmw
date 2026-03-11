#include "Speed/Indep/Src/Camera/Actions/CDActionTrackCar.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionTrackCar("TRACKCAR", CDActionTrackCar::Construct);

const Attrib::StringKey &CDActionTrackCar::GetName() const {
    static Attrib::StringKey name("TRACKCAR");
    return name;
}

Attrib::StringKey CDActionTrackCar::GetNext() const {
    return Attrib::StringKey();
}

bool CDActionTrackCar::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionTrackCar::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionTrackCar::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionTrackCar::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionTrackCar::Construct(CameraAI::Director *director) {
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
            action = new (static_cast<const char *>(0)) CDActionTrackCar(director, player);
        }
        return action;
    }

null_return:
    return nullptr;
}

CDActionTrackCar::CDActionTrackCar(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this), //
      mTarget(0) {
    mPlayer = player;
    mVehicle = nullptr;
    mViewID = director->GetViewID();

    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));
    mAttachments->Attach(mPlayer);

    mAnchor = new (static_cast<const char *>(0), 0) CameraAnchor(0);

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

        mMover = new (static_cast<const char *>(0), 0) TrackCarCameraMover(static_cast<int>(director->GetViewID()), mAnchor, true);
    mMover->ResetState();
}

CDActionTrackCar::~CDActionTrackCar() {
    if (mPlayer) {
        mAttachments->Detach(mPlayer);
    }
    if (mVehicle) {
        mAttachments->Detach(mVehicle);
    }
    CameraMover *m = mMover;
    delete m;
    delete mAnchor;
    delete mAttachments;
}

void CDActionTrackCar::OnDetached(IAttachable *pOther) {
    if (ComparePtr(pOther, mPlayer)) {
        mPlayer = nullptr;
    }
    if (ComparePtr(pOther, mVehicle)) {
        OnCarDetached();
    }
}

void CDActionTrackCar::OnCarDetached() {
    if (mTarget.IsValid()) {
        mTarget.Set(0);
    }
    if (mAnchor) {
        mAnchor->SetWorldID(0);
    }
    mVehicle = nullptr;
}

void CDActionTrackCar::AquireCar() {
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
                const char *model_str = mVehicle->GetVehicleAttributes().MODEL().GetString();
                if (model_str == nullptr) {
                    model_str = "";
                }
                mAnchor->SetModel(bStringHash(model_str));
                mAnchor->SetWorldID(mTarget.GetWorldID());
                ITransmission *itrans;
                if (mVehicle->QueryInterface(&itrans)) {
                    mAnchor->SetTopSpeed(itrans->GetMaxSpeedometer());
                }
            }
        }
    }
}

void CDActionTrackCar::Update(float dT) {
    if (mPlayer == nullptr) {
        if (mVehicle != nullptr) {
            Detach(mVehicle);
            mVehicle = nullptr;
        }
    } else {
        AquireCar();
        if (mTarget.IsValid()) {
            bMatrix4 mat(*mTarget.GetMatrix());

            ICollisionBody *irbc = nullptr;
            mVehicle->QueryInterface(&irbc);
            if (irbc != nullptr) {
                IRigidBody *irb = mVehicle->GetSimable()->GetRigidBody();
                UVector3 cg(irbc->GetCenterOfGravity());
                irb->ConvertLocalToWorld(cg, false);
                cg += irb->GetPosition();
                eSwizzleWorldVector(reinterpret_cast<bVector3 &>(cg), reinterpret_cast<bVector3 &>(cg));
            }

            mAnchor->Update(dT, mat, *mTarget.GetVelocity(), *mTarget.GetAcceleration());
        }
    }
    mMover->Update(dT);
}

bool CDActionTrackCar::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
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
