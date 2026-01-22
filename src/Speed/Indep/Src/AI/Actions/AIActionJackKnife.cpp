#include "Speed/Indep/Src/AI/AIAction.h"
#include "Speed/Indep/Src/AI/AIMath.h"
#include "Speed/Indep/Src/Generated/Events/EMomentStrm.hpp"
#include "Speed/Indep/Src/Generated/Messages/MJackKnife.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Physics/Behavior.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

// total size: 0x48
class AIActionJackKnife : public AIAction {
  public:
    static AIAction *Construct(AIActionParams *params);

    AIActionJackKnife(AIActionParams *params, float score);
    void MessageJackKnife(const MJackKnife &message);

    // Virtual overrides
    // IUnknown
    ~AIActionJackKnife() override {
        if (mMsgJackKnife) {
            Hermes::Handler::Destroy(mMsgJackKnife);
        }
    }

    // AIAction
    bool CanBeAttempted(float dT) override;

    bool IsFinished() override {
        return false;
    }

    void BeginAction(float dT) override {}

    void FinishAction(float dT) override {}

    void Update(float dT) override;
    void OnBehaviorChange(const UCrc32 &mechanic) override;

  private:
    IVehicle *mIVehicle;                // offset 0x48, size 0x4
    IArticulatedVehicle *mArticulation; // offset 0x4C, size 0x4
    IInput *mIInput;                    // offset 0x50, size 0x4
    ISuspension *mISuspension;          // offset 0x54, size 0x4
    Hermes::HHANDLER mMsgJackKnife;     // offset 0x58, size 0x4
    float mAccelTime;                   // offset 0x5C, size 0x4
    bool mForceJackKnife;               // offset 0x60, size 0x1
    bool SentAudioMsg;                  // offset 0x64, size 0x1
};

AIAction *AIActionJackKnife::Construct(AIActionParams *params) {
    return new AIActionJackKnife(params, 1.0f);
}

AIActionJackKnife::AIActionJackKnife(AIActionParams *params, float score) : AIAction(params, score) {
    params->mOwner->QueryInterface(&mIVehicle);
    params->mOwner->QueryInterface(&mIInput);
    params->mOwner->QueryInterface(&mArticulation);
    params->mOwner->QueryInterface(&mISuspension);

    mMsgJackKnife = Hermes::Handler::Create<MJackKnife, AIActionJackKnife, AIActionJackKnife>(this, &AIActionJackKnife::MessageJackKnife, "AIAction",
                                                                                              mIVehicle->GetSimable()->GetWorldID());
    mForceJackKnife = false;
    SentAudioMsg = false;
}

void AIActionJackKnife::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        GetOwner()->QueryInterface(&mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mISuspension);
    }
}

float kActionJackKnifeSpeed = 50.0f;

bool AIActionJackKnife::CanBeAttempted(float dT) {
    if (!mIInput || !mIVehicle || !mArticulation || !mArticulation->GetTrailer()) {
        return false;
    }
    if (mForceJackKnife) {
        mForceJackKnife = false;
        return true;
    }
    if (mIVehicle->GetSpeed() < MPH2MPS(kActionJackKnifeSpeed)) {
        return false;
    }
    IRigidBody *playerIRB = IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody();
    IRigidBody *irb = mIVehicle->GetSimable()->GetRigidBody();
    const UMath::Vector3 &playerPosition = playerIRB->GetPosition();
    const UMath::Vector3 &position = irb->GetPosition();
    UMath::Vector3 forwardVector;
    irb->GetForwardVector(forwardVector);

    const UMath::Vector3 &playerVelocity = playerIRB->GetLinearVelocity();
    const UMath::Vector3 &velocity = irb->GetLinearVelocity();

    float timetoimpact = AI::Math::TimeToImpactXZ(position, velocity, irb->GetRadius(), playerPosition, playerVelocity, playerIRB->GetRadius());
    if (timetoimpact <= 4.25f && timetoimpact > 0.0f) {
        // TODO this variable isn't scoped
        float angletoplayer = AI::Math::AngleTo(position, forwardVector, playerPosition);
        if (angletoplayer <= DEG2ANGLE(25.0f)) {
            mAccelTime = 0.5f;
            return true;
        }
    }
    return false;
}

void AIActionJackKnife::Update(float dT) {
    mIInput->SetControlGas(1.0f);
    mIInput->SetControlBrake(0.0f);

    mAccelTime = UMath::Max(mAccelTime - dT, 0.0f);

    if (mAccelTime > 0.0f) {
        mIInput->SetControlSteering(0.0f);
        return;
    }

    if (SentAudioMsg == false) {
        SentAudioMsg = true;
        UMath::Vector4 vpos;
        vpos.x = mIVehicle->GetPosition().x;
        vpos.y = mIVehicle->GetPosition().y;
        vpos.z = mIVehicle->GetPosition().z;
        // TODO magic
        new EMomentStrm(vpos, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, nullptr, 0x5202c045);
    }
    mIInput->SetControlHandBrake(1.0f);
    mIInput->SetControlSteering(-1.0f);

    if (!mArticulation) {
        return;
    }
    IVehicle *trailer = mArticulation->GetTrailer();
    if (mArticulation->IsHitched() && trailer && mIVehicle->GetSpeed() < MPH2MPS(kActionJackKnifeSpeed / 5.0f)) {
        mArticulation->SetHitch(false);
    }
}

void AIActionJackKnife::MessageJackKnife(const MJackKnife &message) {
    if (mIVehicle && message.GetID() == mIVehicle->GetSimable()->GetWorldID()) {
        mForceJackKnife = true;
    }
}
