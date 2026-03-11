#include "Speed/Indep/Src/Camera/Actions/CDActionIce.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"

extern ICEManager TheICEManager;
extern bool Tweak_ForceICEReplay;
extern bool Tweak_EnableICEAuthoring;
#include "Speed/Indep/Src/Camera/ICE/ICEMover.hpp"

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionIce("ICE", CDActionIce::Construct);

const Attrib::StringKey &CDActionIce::GetName() const {
    static Attrib::StringKey name("ICE");
    return name;
}

Attrib::StringKey CDActionIce::GetNext() const {
    if (mDone) {
        return mPrev;
    }
    return Attrib::StringKey("");
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

    if (player->GetSettings() == nullptr) {
        return nullptr;
    }

    ISimable *isimable = player->GetSimable();
    if (isimable == nullptr) {
        return nullptr;
    }

    unsigned int world_id = isimable->GetWorldID();
    if (world_id == 0) {
        return nullptr;
    }

    if (TheICEManager.ChooseCameraPlaybackTrack() || Tweak_ForceICEReplay) {
        return new (static_cast<const char *>(0)) CDActionIce(director, player);
    }
    return nullptr;
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
            // mPrev from prevAnchor
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
    TheICEManager.SetGenericCameraToPlay("", "");
    if (mPlayer) {
        mAttachments->Detach(mPlayer);
    }
    ReleaseCar(true);
    CameraMover *m = mMover;
    delete m;
    delete mAnchor;
    delete mAttachments;
}

void CDActionIce::OnDetached(IAttachable *pOther) {
    if (ComparePtr(pOther, mPlayer)) {
        mPlayer = nullptr;
    }
    if (ComparePtr(pOther, mVehicle)) {
        ReleaseCar(false);
    }
}

void CDActionIce::ReleaseCar(bool detach) {
    if (mVehicle != nullptr) {
        if (detach) {
            Detach(mVehicle);
        }
        mVehicle = nullptr;
    }
    mTarget.Set(0);
}

void CDActionIce::AquireCar() {
    ISimable *isimable;
    ITransmission *itrans;
    ISuspension *isuspension;

    if (mPlayer == nullptr) {
        return;
    }

    if (!ComparePtr(mPlayer->GetSimable(), mVehicle)) {
        ReleaseCar(true);
    }
    if (mVehicle != nullptr) {
        return;
    }

    isimable = mPlayer->GetSimable();
    if (isimable == nullptr) {
        return;
    }

    mTarget.Set(isimable->GetWorldID());
    if (!mTarget.IsValid()) {
        return;
    }

    if (!isimable->QueryInterface(&mVehicle)) {
        return;
    }

    Attach(mVehicle);

    if (mVehicle->QueryInterface(&itrans)) {
        mAnchor->SetTopSpeed(itrans->GetMaxSpeedometer());
    }

    if (mVehicle->QueryInterface(&isuspension)) {
        mAnchor->SetNumWheels(isuspension->GetNumWheels() != 0);
    }
}

void CDActionIce::Update(float dT) {
    while (!mActionQ.IsEmpty()) {
        ActionRef aRef = mActionQ.GetAction();
        float data = aRef.Data();
        if (aRef.ID() == 0x15 && Tweak_EnableICEAuthoring) {
            Tweak_EnableICEAuthoring = false;
            mDone = true;
        }
        mActionQ.PopAction();
    }

    if (mPlayer == nullptr) {
        ReleaseCar(true);
    } else {
        AquireCar();
        if (mTarget.IsValid()) {
            bMatrix4 mat(*mTarget.GetMatrix());
            ICollisionBody *irbc;
            ISimable *isimable;
            float forward_slip;
            ISuspension *isuspension;

            if (mVehicle != nullptr && mVehicle->QueryInterface(&irbc)) {
                IRigidBody *irb = mVehicle->GetSimable()->GetRigidBody();
                UVector3 cg(irbc->GetCenterOfGravity());
                irb->ConvertLocalToWorld(cg, false);
                cg += irb->GetPosition();
                eSwizzleWorldVector(reinterpret_cast<const bVector3 &>(cg), reinterpret_cast<bVector3 &>(mat.v3));
            }

            mAnchor->SetSlipAngle(0.0f);
            if (mVehicle != nullptr) {
                mAnchor->SetSlipAngle(mVehicle->GetSlipAngle());
            }

            mAnchor->SetRPM(0.0f);
            mAnchor->SetNosEngaged(false);
            mAnchor->SetNosPercentageLeft(0.0f);

            if (mVehicle != nullptr && mVehicle->QueryInterface(&isimable)) {
                IEngine *iengine;
                if (isimable->QueryInterface(&iengine)) {
                    mAnchor->SetRPM(iengine->GetRPM());
                    mAnchor->SetNosEngaged(iengine->IsNOSEngaged());
                    mAnchor->SetNosPercentageLeft(iengine->GetNOSCapacity());
                }
            }

            forward_slip = 0.0f;
            if (mVehicle != nullptr && mVehicle->QueryInterface(&isuspension)) {
                for (unsigned int i = 0; i < isuspension->GetNumWheels(); i++) {
                    if (isuspension->GetWheelSlip(i) > 0.0f) {
                        forward_slip += isuspension->GetWheelSlip(i);
                    }
                }
            }

            mAnchor->SetForwardSlip(forward_slip);
            mAnchor->Update(dT, *reinterpret_cast<const ICE::Matrix4 *>(&mat),
                            *reinterpret_cast<const ICE::Vector3 *>(mTarget.GetVelocity()),
                            *reinterpret_cast<const ICE::Vector3 *>(mTarget.GetAcceleration()));
            if (TheICEManager.IsEditorOff() && Tweak_ForceICEReplay) {
                TheICEManager.ChooseReplayCamera();
            }
        }
        TheICEManager.Update();
    }
}
