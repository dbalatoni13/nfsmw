#include "Speed/Indep/Src/Camera/Actions/CDActionShowcase.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Camera/Movers/Showcase.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"

static bool IsRightSide() {
    int playerRanking = 0;
    int opponentRanking = 0;
    UMath::Vector3 playerPos;
    UMath::Vector3 opponentPos;
    UMath::Vector3 playerFwd;
    UMath::Vector3 opponentFwd;

    for (int onRacer = 0; onRacer < GRaceStatus::Get().GetRacerCount(); onRacer++) {
        GRacerInfo &racerInfo = GRaceStatus::Get().GetRacerInfo(onRacer);
        ISimable *simable = racerInfo.GetSimable();

        UMath::Matrix4 matrix;
        simable->GetTransform(matrix);
        UMath::Vector3 velocity;
        simable->GetLinearVelocity(velocity);

        if (simable->GetPlayer() == nullptr) {
            opponentRanking = racerInfo.GetRanking();

            if (UMath::LengthSquare(velocity) <= 0.0001f) {
                UMath::Copy(UMath::Vector4To3(matrix.v2), opponentFwd);
            } else {
                UMath::Unit(velocity, opponentFwd);
            }

            UMath::Copy(UMath::Vector4To3(matrix.v3), opponentPos);
        } else {
            playerRanking = racerInfo.GetRanking();

            if (UMath::LengthSquare(velocity) <= 0.0001f) {
                UMath::Copy(UMath::Vector4To3(matrix.v2), playerFwd);
            } else {
                UMath::Unit(velocity, playerFwd);
            }

            UMath::Copy(UMath::Vector4To3(matrix.v3), playerPos);
        }
    }

    if (playerRanking == 0 || opponentRanking == 0) {
        return false;
    }

    UMath::Vector3 opponent2player;
    UMath::Sub(opponentPos, playerPos, opponent2player);
    opponent2player.y = 0.0f;

    if (UMath::LengthSquare(opponent2player) <= 0.0001f) {
        return false;
    }

    UMath::Unit(opponent2player, opponent2player);

    UMath::Vector3 up = UMath::Vector3Make(0.0f, 1.0f, 0.0f);

    UMath::Vector3 playerRight;
    UMath::Cross(up, opponentFwd, playerRight);

    float dot = UMath::Dot(opponent2player, playerRight);

    bool rightSide = false;
    if ((playerRanking == 1 && 0.0f < dot) || (playerRanking == 2 && dot < 0.0f)) {
        rightSide = true;
    }

    return rightSide;
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
