#include "Speed/Indep/Src/Camera/Actions/CDActionTrackCop.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
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

    if (player->GetSettings() == nullptr) {
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
            action = new (static_cast<const char *>(0)) CDActionTrackCop(director, player);
        }
        return action;
    }

null_return:
    return nullptr;
}

CDActionTrackCop::CDActionTrackCop(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this), //
      mTarget(0) {
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

        mMover = new (static_cast<const char *>(0), 0) TrackCopCameraMover(static_cast<int>(director->GetViewID()), mAnchor, false);
    mMover->ResetState();
    static_cast<TrackCopCameraMover *>(mMover)->SetRenderCarPOV(renderCarPOV);
}

CDActionTrackCop::~CDActionTrackCop() {
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
                CameraAnchor *anchor = mAnchor;
                const char *model_str = mVehicle->GetVehicleAttributes().MODEL().GetString();
                if (model_str == nullptr) {
                    model_str = "";
                }
                anchor->SetModel(bStringHash(model_str));
                mAnchor->SetWorldID(mTarget.GetWorldID());
                ITransmission *itrans;
                if (mVehicle->QueryInterface(&itrans)) {
                    mAnchor->SetTopSpeed(itrans->GetMaxSpeedometer());
                }
            }
        }
    }
}

void CDActionTrackCop::Update(float dT) {
    if (mPlayer == nullptr) {
        if (mVehicle != nullptr) {
            Detach(mVehicle);
            mVehicle = nullptr;
        }
        return;
    }

    AquireCar();
    if (!mTarget.IsValid()) {
        return;
    }

    bMatrix4 mat(*mTarget.GetMatrix());

    ICollisionBody *irbc;
    if (mVehicle != nullptr) {
        if (mVehicle->QueryInterface(&irbc)) {
            IRigidBody *irb = mVehicle->GetSimable()->GetRigidBody();
            UVector3 cg(irbc->GetCenterOfGravity());
            irb->ConvertLocalToWorld(cg, false);
            cg += irb->GetPosition();
            eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(cg), reinterpret_cast<bVector3 &>(mat.v3));
        }
    }

    mAnchor->Update(dT, mat, *mTarget.GetVelocity(), *mTarget.GetAcceleration());
}

bool CDActionTrackCop::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
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
