#ifndef AI_AIVEHICLEPURSUIT_H
#define AI_AIVEHICLEPURSUIT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/AI/AIVehicle.h"

// total size: 0x7D0
class AIVehiclePursuit : public AIVehiclePid, public IPursuitAI {
  public:
    AIVehiclePursuit(const BehaviorParams &bp);

    // Overrides: IUnknown
    ~AIVehiclePursuit() override;

    // Overrides: AIVehicle
    void ResetInternals() override;

    // Overrides: IPursuitAI
    void StartPatrol() override;

    // Overrides: IPursuitAI
    void StartFlee() override;

    // Overrides: IPursuitAI
    void StartRoadBlock() override;

    // Overrides: IPursuitAI
    void StartPursuit(AITarget *target, ISimable *itargetSimable) override;

    // Overrides: IPursuitAI
    void DoInPositionGoal() override;

    // Overrides: IPursuitAI
    void EndPursuit() override;

    // Overrides: IPursuitAI
    bool StartSupportGoal() override;

    // Overrides: IPursuitAI
    void SetSupportGoal(UCrc32 sg) override;

    // Overrides: IPursuitAI
    AITarget *GetPursuitTarget() override;

    // Overrides: IPursuitAI
    AITarget *PursuitRequest() override;

    // Overrides: AIVehicle
    void Update(float dT) override;

    virtual void UpdateSiren(float dT);

    // Overrides: IPursuitAI
    void SetInPursuit(bool inPursuit) override {
        mInPursuit = inPursuit;
    }

    // Overrides: IPursuitAI
    bool GetInPursuit() override {
        return mInPursuit;
    }

    // Overrides: IPursuitAI
    void SetInFormation(bool inFormation) override {
        mInFormation = inFormation;
    }

    // Overrides: IPursuitAI
    bool GetInFormation() override {
        return mInFormation;
    }

    // Overrides: IPursuitAI
    void SetInPosition(bool inPosition) override {
        mInPosition = inPosition;
    }

    // Overrides: IPursuitAI
    bool GetInPosition() override {
        return mInPosition;
    }

    // Overrides: IPursuitAI
    void SetPursuitOffset(const UMath::Vector3 &offset) override {
        mPursuitOffset = offset;
    }

    // Overrides: IPursuitAI
    const UMath::Vector3 &GetPursuitOffset() const override {
        return mPursuitOffset;
    }

    // Overrides: IPursuitAI
    void SetInPositionOffset(const UMath::Vector3 &offset) override {
        mInPositionOffset = offset;
    }

    // Overrides: IPursuitAI
    const UMath::Vector3 &GetInPositionOffset() const override {
        return mInPositionOffset;
    }

    // Overrides: IPursuitAI
    void SetInPositionGoal(const UCrc32 &ipg) override {
        mInPositionGoal = ipg;
    }

    // Overrides: IPursuitAI
    const UCrc32 &GetInPositionGoal() const override {
        return mInPositionGoal;
    }

    // Overrides: IPursuitAI
    void SetBreaker(bool breaker) override {
        mBreaker = breaker;
    }

    // Overrides: IPursuitAI
    bool GetBreaker() override {
        return mBreaker;
    }

    // Overrides: IPursuitAI
    void SetChicken(bool chicken) override {
        mChicken = chicken;
    }

    // Overrides: IPursuitAI
    bool GetChicken() override {
        return mChicken;
    }

    // Overrides: IPursuitAI
    void SetDamagedByPerp(bool damaged) override {
        mDamagedByPerp = damaged;
    }

    // Overrides: IPursuitAI
    bool GetDamagedByPerp() override {
        return mDamagedByPerp;
    }

    // Overrides: IPursuitAI
    SirenState GetSirenState() const override {
        return mSirenState;
    }

    // Overrides: IPursuitAI
    float GetTimeSinceTargetSeen() const override {
        return mTimeSinceTargetSeen;
    }

    // Overrides: IPursuitAI
    void ZeroTimeSinceTargetSeen() override {
        mTimeSinceTargetSeen = 0.0f;
    }

    // Overrides: IPursuitAI
    const UCrc32 &GetSupportGoal() const override {
        return mSupportGoal;
    }

    // Overrides: IPursuitAI
    void SetWithinEngagementRadius() override {
        mWithinEngagementRadius = true;
    }

    // Overrides: IPursuitAI
    bool WasWithinEngagementRadius() const override {
        return mWithinEngagementRadius;
    }

  private:
    static float mStagger; // size: 0x4, address: 0x8041540C

    bool mInPursuit;                  // offset 0x778, size 0x1
    bool mBreaker;                    // offset 0x77C, size 0x1
    bool mChicken;                    // offset 0x780, size 0x1
    bool mDamagedByPerp;              // offset 0x784, size 0x1
    SirenState mSirenState;           // offset 0x788, size 0x4
    bool mSirenInit;                  // offset 0x78C, size 0x1
    Timer mT_siren[3];                // offset 0x790, size 0xC
    bool mInFormation;                // offset 0x79C, size 0x1
    bool mInPosition;                 // offset 0x7A0, size 0x1
    bool mWithinEngagementRadius;     // offset 0x7A4, size 0x1
    UMath::Vector3 mPursuitOffset;    // offset 0x7A8, size 0xC
    UMath::Vector3 mInPositionOffset; // offset 0x7B4, size 0xC
    UCrc32 mInPositionGoal;           // offset 0x7C0, size 0x4
    float mTimeSinceTargetSeen;       // offset 0x7C4, size 0x4
    float mVisibiltyTestTimer;        // offset 0x7C8, size 0x4
    UCrc32 mSupportGoal;              // offset 0x7CC, size 0x4
};

#endif
