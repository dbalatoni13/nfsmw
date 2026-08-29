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
        if (this->mMsgJackKnife != nullptr) {
            Hermes::Handler::Destroy(this->mMsgJackKnife);
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
    return new AIActionJackKnife(params, AIACTION_SCORE_HIGH);
}

AIActionJackKnife::AIActionJackKnife(AIActionParams *params, float score) : AIAction(params, score) {
    params->mOwner->QueryInterface(&this->mIVehicle);
    params->mOwner->QueryInterface(&this->mIInput);
    params->mOwner->QueryInterface(&this->mArticulation);
    params->mOwner->QueryInterface(&this->mISuspension);

    this->mMsgJackKnife = Hermes::Handler::Create<MJackKnife, AIActionJackKnife, AIActionJackKnife>(
        this, &AIActionJackKnife::MessageJackKnife, UCrc32("AIAction"), this->mIVehicle->GetSimable()->GetWorldID());
    this->mForceJackKnife = false;
    this->SentAudioMsg = false;
}

void AIActionJackKnife::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_INPUT) {
        this->GetOwner()->QueryInterface(&this->mIInput);
    }
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&this->mISuspension);
    }
}

float kActionJackKnifeSpeed = 50.0f;

bool AIActionJackKnife::CanBeAttempted(float dT) {
    if (this->mIInput == nullptr || this->mIVehicle == nullptr || this->mArticulation == nullptr || this->mArticulation->GetTrailer() == nullptr) {
        return false;
    }
    if (this->mForceJackKnife) {
        this->mForceJackKnife = false;
        return true;
    }
    if (this->mIVehicle->GetSpeed() < MPH2MPS(kActionJackKnifeSpeed)) {
        return false;
    }
    IRigidBody *playerIRB = IPlayer::First(PLAYER_LOCAL)->GetSimable()->GetRigidBody();
    IRigidBody *irb = this->mIVehicle->GetSimable()->GetRigidBody();
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
            this->mAccelTime = 0.5f;
            return true;
        }
    }
    return false;
}

void AIActionJackKnife::Update(float dT) {
    this->mIInput->SetControlGas(1.0f);
    this->mIInput->SetControlBrake(0.0f);

    this->mAccelTime = UMath::Max(this->mAccelTime - dT, 0.0f);

    if (this->mAccelTime > 0.0f) {
        this->mIInput->SetControlSteering(0.0f);
        return;
    }

    if (this->SentAudioMsg == false) {
        this->SentAudioMsg = true;
        UMath::Vector4 vpos;
        vpos.x = this->mIVehicle->GetPosition().x;
        vpos.y = this->mIVehicle->GetPosition().y;
        vpos.z = this->mIVehicle->GetPosition().z;
        // TODO magic
        new EMomentStrm(vpos, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, nullptr, 0x5202c045);
    }
    this->mIInput->SetControlHandBrake(1.0f);
    this->mIInput->SetControlSteering(-1.0f);

    if (this->mArticulation == nullptr) {
        return;
    }
    IVehicle *trailer = this->mArticulation->GetTrailer();
    if (this->mArticulation->IsHitched() && trailer != nullptr && this->mIVehicle->GetSpeed() < MPH2MPS(kActionJackKnifeSpeed / 5.0f)) {
        this->mArticulation->SetHitch(false);
    }
}

void AIActionJackKnife::MessageJackKnife(const MJackKnife &message) {
    if (this->mIVehicle != nullptr && message.GetID() == this->mIVehicle->GetSimable()->GetWorldID()) {
        this->mForceJackKnife = true;
    }
}
