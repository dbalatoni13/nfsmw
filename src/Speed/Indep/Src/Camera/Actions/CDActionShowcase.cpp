#include "Speed/Indep/Src/Camera/Actions/CDActionShowcase.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Camera/Movers/Showcase.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

static bool IsRightSide() {
    return false;
}

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionShowcase("SHOWCASE", CDActionShowcase::Construct);

const Attrib::StringKey &CDActionShowcase::GetName() const {
    static Attrib::StringKey name("SHOWCASE");
    return name;
}

Attrib::StringKey CDActionShowcase::GetNext() const {
    return Attrib::StringKey();
}

bool CDActionShowcase::Attach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Attach(pOther);
}

bool CDActionShowcase::Detach(UTL::COM::IUnknown *pOther) {
    return mAttachments->Detach(pOther);
}

bool CDActionShowcase::IsAttached(const UTL::COM::IUnknown *pOther) const {
    return mAttachments->IsAttached(pOther);
}

const IAttachable::List *CDActionShowcase::GetAttachments() const {
    return &mAttachments->GetList();
}

CameraAI::Action *CDActionShowcase::Construct(CameraAI::Director *director) {
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

    return new ("CDActionShowcase") CDActionShowcase(director, player);
}

CDActionShowcase::CDActionShowcase(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this) {
    mTarget = WorldConn::Reference(0);
    mPlayer = player;
    mVehicle = nullptr;
    mViewID = director->GetViewID();

    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));

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

    bool flipSide = IsRightSide();
    mMover = new ShowcaseCameraMover(static_cast<int>(director->GetViewID()), mAnchor, flipSide);
    mMover->GetCamera()->SetRenderDash(0);
}

CDActionShowcase::~CDActionShowcase() {
    // TODO
}

void CDActionShowcase::OnDetached(IAttachable *pOther) {
    // TODO
}

void CDActionShowcase::OnCarDetached() {
    // TODO
}

void CDActionShowcase::AquireCar() {
    // TODO
}

void CDActionShowcase::Update(float dT) {
    // TODO
}
