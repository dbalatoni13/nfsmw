#include "Speed/Indep/Src/Camera/Actions/CDActionIce.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

extern ICEManager TheICEManager;
extern bool Tweak_ForceICEReplay;
#include "Speed/Indep/Src/Camera/ICE/ICEMover.hpp"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionIce("ICE", CDActionIce::Construct);

const Attrib::StringKey &CDActionIce::GetName() const {
    static Attrib::StringKey name("ICE");
    return name;
}

Attrib::StringKey CDActionIce::GetNext() const {
    if (!mDone) {
        return Attrib::StringKey("");
    }
    return mPrev;
}

CameraMover *CDActionIce::GetMover() {
    return mMover;
}

bool CDActionIce::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionIce::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionIce::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionIce::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionIce::Construct(CameraAI::Director *director) {
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

    if (!Tweak_ForceICEReplay && !TheICEManager.IsActive()) {
        return nullptr;
    }

    return new ("CDActionIce") CDActionIce(director, player);
}

CDActionIce::CDActionIce(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this),   //
      mActionQ(1, 0x98c7a2f5, "ICE", false) {
    mDone = false;
    mPrev = Attrib::StringKey();
    mTrack = nullptr;
    mTarget = WorldConn::Reference(0);
    mPlayer = player;
    mVehicle = nullptr;

    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));
    mAttachments->Attach(mPlayer);

    CameraMover *m = director->GetMover();
    if (m != nullptr) {
        CameraAnchor *prevAnchor = m->GetAnchor();
        if (prevAnchor != nullptr) {
            mPrev.mHash = prevAnchor->GetWorldID();
        }
    }

    mAnchor = new ICEAnchor();

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

        mAnchor->Update(0.0f, *reinterpret_cast<const ICE::Matrix4 *>(mTarget.GetMatrix()),
                        *reinterpret_cast<const ICE::Vector3 *>(mTarget.GetVelocity()),
                        *reinterpret_cast<const ICE::Vector3 *>(mTarget.GetAcceleration()));
    }

    mMover = new ICEMover(static_cast<int>(director->GetViewID()), mAnchor);
}

CDActionIce::~CDActionIce() {
    // TODO
}

void CDActionIce::OnDetached(IAttachable *pOther) {
    // TODO
}

void CDActionIce::ReleaseCar(bool detach) {
    // TODO
}

void CDActionIce::AquireCar() {
    // TODO
}

void CDActionIce::Update(float dT) {
    // TODO
}
