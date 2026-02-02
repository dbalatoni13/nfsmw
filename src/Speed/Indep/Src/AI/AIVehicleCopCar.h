#ifndef AI_AIVEHICLECOPCAR_H
#define AI_AIVEHICLECOPCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AIVehiclePursuit.h"

// total size: 0x7D8
class AIVehicleCopCar : public AIVehiclePursuit {
  public:
    AIVehicleCopCar(const BehaviorParams &bp);

    // Overrides: IUnknown
    ~AIVehicleCopCar() override;

    static Behavior *Construct(const BehaviorParams &bp);

    // Overrides: AIVehicle
    void Update(float dT) override;

    // Overrides: AIVehicle
    bool IsTetheredToTarget(IUnknown *object) override;

    // Overrides: IPursuitAI
    bool CanSeeTarget(AITarget *target) override;

    void WatchForPerps();

    bool CheckForPursuit(IVehicle *itargetVehicle);

    // Overrides: IVehicleAI
    float GetSkill() const override {
        return 1.0f;
    }

    // Overrides: IVehicleAI
    float GetShortcutSkill() const override {
        return 1.0f;
    }

  private:
    float mLOSAngleFront;   // offset 0x7D0, size 0x4
    bool mPerpHiddenFromMe; // offset 0x7D4, size 0x1
};

#endif
