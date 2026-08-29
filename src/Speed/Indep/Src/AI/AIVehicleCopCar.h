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
#ifndef __AIVEHICLECOPCAR_H
#define __AIVEHICLECOPCAR_H 1

#include "AIVehiclePursuit.h"

// total size: 0x7D8
// Decl: 25
class AIVehicleCopCar : public AIVehiclePursuit {
  public:
    static Behavior *Construct(const BehaviorParams &bp);

    // Overrides: AIVehicle
    void Update(float dT) override;

    // Overrides: IPursuitAI
    bool CanSeeTarget(AITarget *target) override;

    // Overrides: AIVehicle
    bool IsTetheredToTarget(IUnknown *object) override;

    // Overrides: IVehicleAI
    float GetSkill() const override {
        return 1.0f;
    }

    // Overrides: IVehicleAI
    float GetShortcutSkill() const override {
        return 1.0f;
    }

  protected:
    AIVehicleCopCar(const BehaviorParams &bp);
    ~AIVehicleCopCar() override;

    void WatchForPerps();

    float mLOSAngleFront;   // offset 0x7D0, size 0x4
    bool mPerpHiddenFromMe; // offset 0x7D4, size 0x1

  private:
    bool CheckForPursuit(IVehicle *itargetVehicle);
};

#endif
