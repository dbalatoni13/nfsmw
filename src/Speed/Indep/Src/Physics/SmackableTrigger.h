#ifndef SMACKABLE_TRIGGER_H
#define SMACKABLE_TRIGGER_H

#include "Speed/Indep/Src/Generated/Events/ESpawnSmackable.hpp"

class SmackableTrigger {
  public:
    USE_FASTALLOC(SmackableTrigger);

    SmackableTrigger(HMODEL hmodel, bool virgin, const UMath::Matrix4 &objectmatrix, const UMath::Vector3 &dim, unsigned int extra_flags);

    void Fire();

    void Disable();

    void Enable();

    bool IsEnabled() const;
    void GetObjectMatrix(UMath::Matrix4 &matrix) const;

    void Move(const UMath::Matrix4 &matrix, const UMath::Vector3 &dim, bool virgin);

    ~SmackableTrigger();

  private:
    WTrigger *mTrigger;                      // offset 0x0, size 0x4
    ESpawnSmackable::StaticData *mEventData; // offset 0x4, size 0x4
};

#endif
