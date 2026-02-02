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
    void SetInPursuit(bool inPursuit) override {}

    // Overrides: IPursuitAI
    // bool GetInPursuit() override {}

    // Overrides: IPursuitAI
    void SetInFormation(bool inFormation) override {}

    // Overrides: IPursuitAI
    // bool GetInFormation() override {}

    // Overrides: IPursuitAI
    void SetInPosition(bool inPosition) override {}

    // Overrides: IPursuitAI
    // bool GetInPosition() override {}

    // Overrides: IPursuitAI
    void SetPursuitOffset(const UMath::Vector3 &offset) override {}

    // Overrides: IPursuitAI
    // const UMath::Vector3 &GetPursuitOffset() const override {}

    // Overrides: IPursuitAI
    void SetInPositionOffset(const UMath::Vector3 &offset) override {}

    // Overrides: IPursuitAI
    // const UMath::Vector3 &GetInPositionOffset() const override {}

    // Overrides: IPursuitAI
    void SetInPositionGoal(const UCrc32 &ipg) override {}

    // Overrides: IPursuitAI
    // const UCrc32 &GetInPositionGoal() const override {}

    // Overrides: IPursuitAI
    void SetBreaker(bool breaker) override {}

    // Overrides: IPursuitAI
    // bool GetBreaker() override {}

    // Overrides: IPursuitAI
    void SetChicken(bool chicken) override {}

    // Overrides: IPursuitAI
    // bool GetChicken() override {}

    // Overrides: IPursuitAI
    void SetDamagedByPerp(bool damaged) override {}

    // Overrides: IPursuitAI
    // bool GetDamagedByPerp() override {}

    // Overrides: IPursuitAI
    // SirenState GetSirenState() const override {}

    // Overrides: IPursuitAI
    // float GetTimeSinceTargetSeen() const override {}

    // Overrides: IPursuitAI
    void ZeroTimeSinceTargetSeen() override {}

    // Overrides: IPursuitAI
    // const UCrc32 &GetSupportGoal() const override {}

    // Overrides: IPursuitAI
    void SetWithinEngagementRadius() override {}

    // Overrides: IPursuitAI
    // bool WasWithinEngagementRadius() const override {}

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
