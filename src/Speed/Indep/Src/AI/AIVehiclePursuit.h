//
//
//
//
//
//
//
//
//
//
//
#ifndef __AIVEHICLEPURSUIT_H
#define __AIVEHICLEPURSUIT_H 1

#include "Speed/Indep/Src/AI/AIVehicle.h"

// total size: 0x7D0
// Decl: 27
class AIVehiclePursuit : public AIVehiclePid, public IPursuitAI {
  public:
    // Overrides: AIVehicle
    void Update(float dT) override;

    // Overrides: IPursuitAI
    void StartPatrol() override;
    void StartRoadBlock() override;
    void StartFlee() override;

    // Overrides: IPursuitAI
    void SetInPursuit(bool inPursuit) override {
        this->mInPursuit = inPursuit;
    }

    // Overrides: IPursuitAI
    bool GetInPursuit() override {
        return this->mInPursuit;
    }

    // Overrides: IPursuitAI
    void StartPursuit(AITarget *target, ISimable *itargetSimable) override;
    void DoInPositionGoal() override;
    void EndPursuit() override;
    AITarget *GetPursuitTarget() override;
    bool StartSupportGoal() override;
    AITarget *PursuitRequest() override;

    // Overrides: IPursuitAI
    void SetInFormation(bool inFormation) override {
        this->mInFormation = inFormation;
    }

    // Overrides: IPursuitAI
    bool GetInFormation() override {
        return this->mInFormation;
    }

    // Overrides: IPursuitAI
    void SetInPosition(bool inPosition) override {
        this->mInPosition = inPosition;
    }

    // Overrides: IPursuitAI
    bool GetInPosition() override {
        return this->mInPosition;
    }

    // Overrides: IPursuitAI
    void SetPursuitOffset(const UMath::Vector3 &offset) override {
        this->mPursuitOffset = offset;
    }

    // Overrides: IPursuitAI
    const UMath::Vector3 &GetPursuitOffset() const override {
        return this->mPursuitOffset;
    }

    // Overrides: IPursuitAI
    void SetInPositionOffset(const UMath::Vector3 &offset) override {
        this->mInPositionOffset = offset;
    }

    // Overrides: IPursuitAI
    const UMath::Vector3 &GetInPositionOffset() const override {
        return this->mInPositionOffset;
    }

    // Overrides: IPursuitAI
    void SetInPositionGoal(const UCrc32 &ipg) override {
        this->mInPositionGoal = ipg;
    }

    // Overrides: IPursuitAI
    const UCrc32 &GetInPositionGoal() const override {
        return this->mInPositionGoal;
    }

    // Overrides: IPursuitAI
    void SetBreaker(bool breaker) override {
        this->mBreaker = breaker;
    }

    // Overrides: IPursuitAI
    bool GetBreaker() override {
        return this->mBreaker;
    }

    // Overrides: IPursuitAI
    void SetChicken(bool chicken) override {
        this->mChicken = chicken;
    }

    // Overrides: IPursuitAI
    bool GetChicken() override {
        return this->mChicken;
    }

    // Overrides: IPursuitAI
    void SetDamagedByPerp(bool damaged) override {
        this->mDamagedByPerp = damaged;
    }

    // Overrides: IPursuitAI
    bool GetDamagedByPerp() override {
        return this->mDamagedByPerp;
    }

    // Overrides: IPursuitAI
    Sound::SirenState GetSirenState() const override {
        return this->mSirenState;
    }

    // Overrides: IPursuitAI
    float GetTimeSinceTargetSeen() const override {
        return this->mTimeSinceTargetSeen;
    }

    // Overrides: IPursuitAI
    void ZeroTimeSinceTargetSeen() override {
        this->mTimeSinceTargetSeen = 0.0f;
    }

    // Overrides: IPursuitAI
    const UCrc32 &GetSupportGoal() const override {
        return this->mSupportGoal;
    }

    // Overrides: IPursuitAI
    void SetSupportGoal(UCrc32 sg) override;

    // Overrides: IPursuitAI
    void SetWithinEngagementRadius() override {
        this->mWithinEngagementRadius = true;
    }

    // Overrides: IPursuitAI
    bool WasWithinEngagementRadius() const override {
        return this->mWithinEngagementRadius;
    }

  protected:
    AIVehiclePursuit(const BehaviorParams &bp);
    ~AIVehiclePursuit() override;

    // Overrides: AIVehicle

    void ResetInternals() override;

    virtual void UpdateSiren(float dT);

  private:
    bool mInPursuit;                  // offset 0x778, size 0x1
    bool mBreaker;                    // offset 0x77C, size 0x1
    bool mChicken;                    // offset 0x780, size 0x1
    bool mDamagedByPerp;              // offset 0x784, size 0x1
    Sound::SirenState mSirenState;    // offset 0x788, size 0x4
    bool mSirenInit;                  // offset 0x78C, size 0x1
    Timer mT_siren[3];                // offset 0x790, size 0xC
    bool mInFormation;                // offset 0x79C, size 0x1
    bool mInPosition;                 // offset 0x7A0, size 0x1
    bool mWithinEngagementRadius;     // offset 0x7A4, size 0x1
    UMath::Vector3 mPursuitOffset;    // offset 0x7A8, size 0xC
    UMath::Vector3 mInPositionOffset; // offset 0x7B4, size 0xC

    static float mStagger; // size: 0x4, address: 0x8041540C

    UCrc32 mInPositionGoal;     // offset 0x7C0, size 0x4
    float mTimeSinceTargetSeen; // offset 0x7C4, size 0x4
    float mVisibiltyTestTimer;  // offset 0x7C8, size 0x4
    UCrc32 mSupportGoal;        // offset 0x7CC, size 0x4
};

#endif
