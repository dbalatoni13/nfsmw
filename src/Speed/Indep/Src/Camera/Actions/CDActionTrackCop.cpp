#include "Speed/Indep/Src/Camera/Actions/CDActionTrackCop.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionTrackCop("TRACKCOP", CDActionTrackCop::Construct);

const Attrib::StringKey &CDActionTrackCop::GetName() const {
    static Attrib::StringKey name("TRACKCOP");
    return name;
}

Attrib::StringKey CDActionTrackCop::GetNext() const {
    return Attrib::StringKey();
}

bool CDActionTrackCop::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionTrackCop::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionTrackCop::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionTrackCop::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionTrackCop::Construct(CameraAI::Director *director) {
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

    return new ("CDActionTrackCop") CDActionTrackCop(director, player);
}

CDActionTrackCop::CDActionTrackCop(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this) {
    mTarget = WorldConn::Reference(0);
    mPlayer = player;
    mVehicle = nullptr;
    bool renderCarPOV = true;
    mViewID = director->GetViewID();

    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));
    mAttachments->Attach(mPlayer);

    CameraMover *m = director->GetMover();
    if (m != nullptr) {
        renderCarPOV = m->RenderCarPOV();
    }

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
            eSwizzleWorldVector(reinterpret_cast<bVector3 &>(cg), reinterpret_cast<bVector3 &>(cg));
        }

        mAnchor->Update(0.0f, mat, *mTarget.GetVelocity(), *mTarget.GetAcceleration());
    }

    mMover = new TrackCopCameraMover(static_cast<int>(director->GetViewID()), mAnchor, false);
    mMover->GetCamera()->SetRenderDash(0);
    static_cast<TrackCopCameraMover *>(mMover)->SetRenderCarPOV(renderCarPOV);
}

CDActionTrackCop::~CDActionTrackCop() {
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

void CDActionTrackCop::OnDetached(IAttachable *pOther) {
    if (ComparePtr(pOther, mPlayer)) {
        mPlayer = nullptr;
    }
    if (ComparePtr(pOther, mVehicle)) {
        OnCarDetached();
    }
}

void CDActionTrackCop::OnCarDetached() {
    if (mTarget.IsValid()) {
        mTarget.Set(0);
    }
    if (mAnchor) {
        mAnchor->SetWorldID(0);
    }
    mVehicle = nullptr;
}

void CDActionTrackCop::AquireCar() {
    // TODO
}

void CDActionTrackCop::Update(float dT) {
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

bool CDActionTrackCop::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
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
