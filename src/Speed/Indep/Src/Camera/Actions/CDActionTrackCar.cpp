#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Movers/TrackCar.hpp"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ITransmission.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/mtx.h>
#endif

class CDActionTrackCar : public CameraAI::Action, public IAttachable, public ITrafficCenter {
  public:
    static CameraAI::Action *Construct(CameraAI::Director *director);

    CDActionTrackCar(CameraAI::Director *director, IPlayer *player);
    ~CDActionTrackCar() override;

    void AquireCar();
    void OnCarDetached();
    void Update(float dT) override;
    void OnDetached(IAttachable *pOther) override;

    void Reset() override;
    const Attrib::StringKey &GetName() const override {
        static Attrib::StringKey name("CDActionTrackCar");
        return name;
    }
    Attrib::StringKey GetNext() const override {
        return Attrib::StringKey();
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
    bool GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) override;

    USE_FASTALLOC(CDActionTrackCar)

  private:
    CameraMover *mMover;
    CameraAnchor *mAnchor;
    WorldConn::Reference mTarget;
    IPlayer *mPlayer;
    Sim::Attachments *mAttachments;
    IVehicle *mVehicle;
    EVIEW_ID mView;
};

UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype
    _CDActionTrackCar("CDActionTrackCar", CDActionTrackCar::Construct);

CameraAI::Action *CDActionTrackCar::Construct(CameraAI::Director *director) {
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

        return new CDActionTrackCar(director, found);
    }

    return 0;
}

CDActionTrackCar::CDActionTrackCar(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(0), IAttachable(this), mTarget(0), mPlayer(player), mVehicle(0), mView(director->GetView()) {
    mAttachments = new Sim::Attachments(this);
    Attach(mPlayer);

    mAnchor = new ("CameraAnchor", 0) CameraAnchor(0);

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

    mMover = new ("TrackCarCameraMover", 0) TrackCarCameraMover(director->GetView(), mAnchor, true);
    mMover->ResetState();
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

void CDActionTrackCar::Reset() {}

void CDActionTrackCar::OnDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(pOther, mPlayer)) {
        mPlayer = 0;
    }
    if (UTL::COM::ComparePtr(pOther, mVehicle)) {
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
    mVehicle = 0;
}

void CDActionTrackCar::AquireCar() {
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

    mAnchor->SetModel(bStringHash(mVehicle->GetVehicleAttributes().MODEL().GetString()));
    mAnchor->SetWorldID(mTarget.GetWorldID());

    ITransmission *trans = 0;
    if (mVehicle->QueryInterface(&trans)) {
        mAnchor->SetTopSpeed(trans->GetMaxSpeedometer());
    }
}

void CDActionTrackCar::Update(float dT) {
    if (!mPlayer) {
        if (mVehicle) {
            Detach(mVehicle);
            mVehicle = 0;
        }
        return;
    }

    AquireCar();
    if (!mTarget.IsValid()) {
        return;
    }

    bMatrix4 mat;
    bMatrixCopy(mat, *mTarget.GetMatrix());

    ICollisionBody *cbody = 0;
    if (mVehicle && mVehicle->QueryInterface(&cbody)) {
        IRigidBody *body = mVehicle->GetSimable()->GetRigidBody();
        UVector3 cg = cbody->GetCenterOfGravity();
        cg.x = 0.0f;
        body->ConvertLocalToWorld(cg, false);
        VU0_v3add(cg, body->GetPosition(), cg);
        bSetPosition(mat.v3, cg);
    }

    mAnchor->Update(dT, mat, *mTarget.GetVelocity(), *mTarget.GetAcceleration());
}

bool CDActionTrackCar::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
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
