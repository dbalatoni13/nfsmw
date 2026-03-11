#include "Speed/Indep/Src/Camera/Actions/CDActionShowcase.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Movers/Showcase.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
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
        UMath::Vector3 velocity;

        simable->GetTransform(matrix);
        simable->GetLinearVelocity(velocity);

        if (simable->IsPlayer()) {
            playerRanking = racerInfo.GetRanking();
            if (UMath::LengthSquare(velocity) > 0.0f) {
                UMath::Unit(velocity, playerFwd);
            } else {
                playerFwd = UMath::Vector4To3(matrix.v0);
            }
            playerPos = UMath::Vector4To3(matrix.v3);
        } else {
            opponentRanking = racerInfo.GetRanking();
            if (UMath::LengthSquare(velocity) > 0.0f) {
                UMath::Unit(velocity, opponentFwd);
            } else {
                opponentFwd = UMath::Vector4To3(matrix.v0);
            }
            opponentPos = UMath::Vector4To3(matrix.v3);
        }
    }

    if (playerRanking == 0 || opponentRanking == 0) {
        return false;
    }

    UMath::Vector3 opponent2player;
    UMath::Sub(opponentPos, playerPos, opponent2player);
    opponent2player.y = 0.0f;

    if (UMath::LengthSquare(opponent2player) > 10000.0f) {
        return false;
    }

    UMath::Unit(opponent2player, opponent2player);
    UMath::Vector3 up = {0.0f, 1.0f, 0.0f};
    UMath::Vector3 playerRight;
    UMath::Cross(up, playerFwd, playerRight);
    float dot = UMath::Dot(opponent2player, playerRight);

    bool rightSide = false;
    if (playerRanking == 1) {
        if (dot > 0.0f) {
            rightSide = true;
        }
    } else if (playerRanking == 2) {
        if (dot < 0.0f) {
            rightSide = true;
        }
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
            action = new (static_cast<const char *>(0)) CDActionShowcase(director, player);
        }
        return action;
    }

null_return:
    return nullptr;
}

CDActionShowcase::CDActionShowcase(CameraAI::Director *director, IPlayer *player)
    : CameraAI::Action(), //
      IAttachable(this), //
      mTarget(0) {
    mPlayer = player;
    mVehicle = nullptr;
    mViewID = director->GetViewID();

    mAttachments = new Sim::Attachments(static_cast<IAttachable *>(this));
    mAttachments->Attach(mPlayer);

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

        mMover = new (static_cast<const char *>(0), 0) ShowcaseCameraMover(static_cast<int>(director->GetViewID()), mAnchor, IsRightSide());
    mMover->ResetState();
}

CDActionShowcase::~CDActionShowcase() {
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

void CDActionShowcase::OnDetached(IAttachable *pOther) {
    if (ComparePtr(pOther, mPlayer)) {
        mPlayer = nullptr;
    }
    if (ComparePtr(pOther, mVehicle)) {
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
    mVehicle = nullptr;
}

void CDActionShowcase::AquireCar() {
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
            }
        }
    }
}

void CDActionShowcase::Update(float dT) {
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
