#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEAnchor.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEMover.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IEngine.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/mtx.h>
#endif

class CDActionIce : public CameraAI::Action, public IAttachable {
  public:
    static CameraAI::Action *Construct(CameraAI::Director *director);

    CDActionIce(CameraAI::Director *director, IPlayer *player);
    ~CDActionIce() override;

    const Attrib::StringKey &GetName() const override {
        static Attrib::StringKey name("CDActionIce");
        return name;
    }
    Attrib::StringKey GetNext() const override {
        return mSwitchOut ? mNext : Attrib::StringKey("");
    }
    CameraMover *GetMover() override {
        return mMover;
    }
    void SetSpecial(float scale) override {}

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
    const IAttachable::List *GetAttachments() const override {
        return (const IAttachable::List *)mAttachments;
    }

    void Reset() override;
    void OnDetached(IAttachable *pOther) override;
    void ReleaseCar(bool detach);
    void AquireCar();
    void Update(float dT) override;

    USE_FASTALLOC(CDActionIce)

  private:
    ActionQueue mQueue;             // offset 0x20, size 0x294
    Attrib::StringKey mNext;        // offset 0x2B8, size 0x10
    ICEAnchor *mAnchor;             // offset 0x2C8, size 0x4
    CameraMover *mMover;            // offset 0x2CC, size 0x4
    WorldConn::Reference mTarget;   // offset 0x2D0, size 0x10
    IPlayer *mPlayer;               // offset 0x2E0, size 0x4
    Sim::Attachments *mAttachments; // offset 0x2E4, size 0x4
    IVehicle *mVehicle;             // offset 0x2E8, size 0x4
    int mUnused;                    // offset 0x2EC, size 0x4
    int mSwitchOut;                 // offset 0x2F0, size 0x4
};

UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionIce("CDActionIce", CDActionIce::Construct);

CameraAI::Action *CDActionIce::Construct(CameraAI::Director *director) {
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

        if (!TheICEManager.ChooseCameraPlaybackTrack() && !Tweak_ForceICEReplay) {
            return 0;
        }

        return new CDActionIce(director, found);
    }

    return 0;
}

CDActionIce::CDActionIce(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(0), IAttachable(this), mQueue(1, 0x98c7a2f5, "CDActionIce", false), mNext(), mTarget(0), mPlayer(player),
      mVehicle(0), mUnused(0), mSwitchOut(0) {
    mAttachments = new Sim::Attachments(this);
    Attach(mPlayer);

    CameraAI::Action *action = director->GetAction();
    if (action) {
        mNext = action->GetName();
    }

    mAnchor = new ("ICEAnchor", 0) ICEAnchor();

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
        mAnchor->Update(0.0f, *reinterpret_cast<const ICE::Matrix4 *>(&mat), *reinterpret_cast<const ICE::Vector3 *>(mTarget.GetVelocity()),
                        *reinterpret_cast<const ICE::Vector3 *>(mTarget.GetAcceleration()));
    }

    mMover = new ("ICEMover", 0) ICEMover(director->GetView(), mAnchor);
}

CDActionIce::~CDActionIce() {
    TheICEManager.SetGenericCameraToPlay("", "");

    if (mPlayer) {
        Detach(mPlayer);
    }

    ReleaseCar(true);

    delete mMover;
    delete mAnchor;
    delete mAttachments;
}

void CDActionIce::Reset() {}

void CDActionIce::OnDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(pOther, mPlayer)) {
        mPlayer = 0;
    }
    if (UTL::COM::ComparePtr(pOther, mVehicle)) {
        ReleaseCar(false);
    }
}

void CDActionIce::ReleaseCar(bool detach) {
    if (mVehicle) {
        if (detach) {
            Detach(mVehicle);
        }
        mVehicle = 0;
    }

    mTarget.Set(0);
}

void CDActionIce::AquireCar() {
    if (!mPlayer) {
        return;
    }

    if (!UTL::COM::ComparePtr(mPlayer->GetSimable(), mVehicle)) {
        ReleaseCar(true);
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

    ITransmission *trans = 0;
    if (mVehicle->QueryInterface(&trans)) {
        mAnchor->SetTopSpeed(trans->GetMaxSpeedometer());
    }

    ISuspension *susp = 0;
    if (mVehicle->QueryInterface(&susp)) {
        mAnchor->SetNumWheels(susp->GetNumWheels() != 0);
    }
}

void CDActionIce::Update(float dT) {
    while (!mQueue.IsEmpty()) {
        const ActionRef action = mQueue.GetAction();

        if (action.ID() == 21 && Tweak_EnableICEAuthoring) {
            Tweak_EnableICEAuthoring = 0;
            mSwitchOut = 1;
        }

        mQueue.PopAction();
    }

    if (!mPlayer) {
        ReleaseCar(true);
        return;
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

        mAnchor->SetSlipAngle(0.0f);
        if (mVehicle) {
            mAnchor->SetSlipAngle(mVehicle->GetSlipAngle());
        }
        mAnchor->SetRPM(0.0f);
        mAnchor->SetNosEngaged(0);
        mAnchor->SetNosPercentageLeft(0.0f);

        ISimable *simable = 0;
        if (mVehicle && mVehicle->QueryInterface(&simable)) {
            IEngine *engine = 0;
            if (simable->QueryInterface(&engine)) {
                mAnchor->SetRPM(engine->GetRPM());
                mAnchor->SetNosEngaged(engine->IsNOSEngaged());
                mAnchor->SetNosPercentageLeft(engine->GetNOSCapacity());
            }
        }

        float slip = 0.0f;
        ISuspension *susp = 0;
        if (mVehicle && mVehicle->QueryInterface(&susp)) {
            for (unsigned int i = 0; i < susp->GetNumWheels(); i++) {
                if (susp->GetWheelSlip(i) > 0.0f) {
                    slip += susp->GetWheelSlip(i);
                }
            }
        }
        mAnchor->SetForwardSlip(slip);

        mAnchor->Update(dT, *reinterpret_cast<const ICE::Matrix4 *>(&mat), *reinterpret_cast<const ICE::Vector3 *>(mTarget.GetVelocity()),
                        *reinterpret_cast<const ICE::Vector3 *>(mTarget.GetAcceleration()));

        if (TheICEManager.IsEditorOff() && Tweak_ForceICEReplay) {
            TheICEManager.ChooseReplayCamera();
        }
    }

    TheICEManager.Update();
}
