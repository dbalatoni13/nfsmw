#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Movers/Showcase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/mtx.h>
#endif

class CDActionShowcase : public CameraAI::Action, public IAttachable {
  public:
    static CameraAI::Action *Construct(CameraAI::Director *director);

    CDActionShowcase(CameraAI::Director *director, IPlayer *player);
    ~CDActionShowcase() override;

    void AquireCar();
    void OnCarDetached();
    void Update(float dT) override;
    void OnDetached(IAttachable *pOther) override;

    void Reset() override;
    const Attrib::StringKey &GetName() const override {
        static Attrib::StringKey name("CDActionShowcase");
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

    USE_FASTALLOC(CDActionShowcase)

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
    _CDActionShowcase("CDActionShowcase", CDActionShowcase::Construct);

CameraAI::Action *CDActionShowcase::Construct(CameraAI::Director *director) {
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

        return new CDActionShowcase(director, found);
    }

    return 0;
}

static bool IsRightSide() {
    int playerRanking = 0;
    int opponentRanking = 0;
    UMath::Vector3 playerPos;
    UMath::Vector3 opponentPos;
    UMath::Vector3 playerFwd;
    UMath::Vector3 opponentFwd;
    UMath::Vector3 opponent2player;

    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(onRacer);
        ISimable *simable = racerInfo.GetSimable();
        UMath::Matrix4 matrix;
        UMath::Vector3 velocity;
        simable->GetTransform(matrix);
        simable->GetLinearVelocity(velocity);

        if (simable->IsPlayer()) {
            playerRanking = racerInfo.GetRanking();
            if (UMath::LengthSquare(velocity) > 0.1f) {
                UMath::Unit(velocity, playerFwd);
            } else {
                UMath::Copy(UMath::Vector4To3(matrix.v0), playerFwd);
            }
            UMath::Copy(UMath::Vector4To3(matrix.v3), playerPos);
        } else {
            opponentRanking = racerInfo.GetRanking();
            if (UMath::LengthSquare(velocity) > 0.1f) {
                UMath::Unit(velocity, opponentFwd);
            } else {
                UMath::Copy(UMath::Vector4To3(matrix.v0), opponentFwd);
            }
            UMath::Copy(UMath::Vector4To3(matrix.v3), opponentPos);
        }
    }

    if (!playerRanking || !opponentRanking) {
        return false;
    }

    UMath::Sub(opponentPos, playerPos, opponent2player);
    opponent2player.y = 0.0f;
    if (UMath::LengthSquare(opponent2player) > 36.0f) {
        return false;
    }
    UMath::Unit(opponent2player, opponent2player);

    UMath::Vector3 up = {0.0f, 1.0f, 0.0f};
    UMath::Vector3 playerRight;
    UMath::Cross(up, playerFwd, playerRight);
    float dot = UMath::Dot(opponent2player, playerRight);

    bool rightSide = false;
    if ((playerRanking == 1 && dot > 0.0f) || (playerRanking == 2 && dot < 0.0f)) {
        rightSide = true;
    }
    return rightSide;
}

CDActionShowcase::CDActionShowcase(CameraAI::Director *director, IPlayer *player)
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

    mMover = new ("ShowcaseCameraMover", 0) ShowcaseCameraMover(director->GetView(), mAnchor, IsRightSide());
    mMover->ResetState();
}

CDActionShowcase::~CDActionShowcase() {
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

void CDActionShowcase::Reset() {}

void CDActionShowcase::OnDetached(IAttachable *pOther) {
    if (UTL::COM::ComparePtr(pOther, mPlayer)) {
        mPlayer = 0;
    }
    if (UTL::COM::ComparePtr(pOther, mVehicle)) {
        OnCarDetached();
    }
}

void CDActionShowcase::OnCarDetached() {
    if (mTarget.IsValid()) {
        mTarget.Set(0);
    }
    if (mAnchor) {
        mAnchor->SetWorldID(0);
    }
    mVehicle = 0;
}

void CDActionShowcase::AquireCar() {
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
}

void CDActionShowcase::Update(float dT) {
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
