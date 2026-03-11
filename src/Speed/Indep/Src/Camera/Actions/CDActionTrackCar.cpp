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
    ISimable *isimable;
    const IPlayer::List &list = IPlayer::GetList(PLAYER_LOCAL);
    for (IPlayer *const *iter = list.begin(); iter != list.end(); ++iter) {
        IPlayer *ip = *iter;
        if (ip->GetControllerPort() == static_cast<int>(director->GetViewID())) {
            player = ip;
            break;
        }
    }

    if (player == nullptr) {
        return nullptr;
    }

    isimable = player->GetSimable();
    if (isimable == nullptr) {
        return nullptr;
    }

    IVehicle *ivehicle = nullptr;
    isimable->QueryInterface(&ivehicle);
    if (ivehicle == nullptr) {
        return nullptr;
    }

    unsigned int world_id = isimable->GetWorldID();
    if (world_id == 0) {
        return nullptr;
    }

    return new ("CDActionTrackCar") CDActionTrackCar(director, player);
}

CDActionTrackCar::CDActionTrackCar(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this) {
    mTarget = WorldConn::Reference(0);
    mPlayer = player;
    mVehicle = nullptr;
    mViewID = director->GetViewID();

    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));
    mAttachments->Attach(mPlayer);

    mAnchor = new CameraAnchor(0);

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
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(cg), reinterpret_cast<bVector3 &>(cg));
        }

        mAnchor->Update(0.0f, mat, *mTarget.GetVelocity(), *mTarget.GetAcceleration());
    }

    mMover = new TrackCarCameraMover(static_cast<int>(director->GetViewID()), mAnchor, true);
    mMover->GetCamera()->SetRenderDash(0);
}

CDActionTrackCar::~CDActionTrackCar() {
    if (mPlayer) {
        Detach(mPlayer);
    }
    if (mVehicle) {
        Detach(mVehicle);
    }
    delete mMover;
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
                const char *model_str = mVehicle->GetVehicleAttributes().MODEL().GetString();
                mAnchor->SetModel(bStringHash(model_str));
                mAnchor->SetWorldID(mTarget.GetWorldID());
                ITransmission *itrans = nullptr;
                mVehicle->QueryInterface(&itrans);
                if (itrans != nullptr) {
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
    IBody *ibody = nullptr;
    if (mVehicle == nullptr) {
        return false;
    }
    mVehicle->QueryInterface(&ibody);
    if (ibody == nullptr) {
        return false;
    }
    ibody->GetTransform(matrix);
    ibody->GetLinearVelocity(velocity);
    return true;
}
