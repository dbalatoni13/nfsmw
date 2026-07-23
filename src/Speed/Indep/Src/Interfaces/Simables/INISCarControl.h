#ifndef INISCARCONTROL_H
#define INISCARCONTROL_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class INISCarControl : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(INISCarControl);

    virtual float GetBurnout() const = 0;
    virtual void SetBurnout(float speed) = 0;
    virtual void SetBrakeLock(bool front, bool rear) = 0;
    virtual void SetConstraintAngle(float angle) = 0;
    virtual void SetSteering(float angle, float weight) = 0;
    virtual bool SetNISPosition(const UMath::Matrix4 &position, bool initial, float dT) = 0;
    virtual void RestoreState() = 0;
    virtual void SetAnimPitch(float dip, float time) = 0;
    virtual void SetAnimRoll(float dip, float time) = 0;
    virtual void SetAnimShake(float dip, float cycleRate, float cycleRamp, float time) = 0;
    virtual float GetAnimPitch() const = 0;
    virtual float GetAnimRoll() const = 0;
    virtual float GetAnimShake() const = 0;
};

class INISCarEngine : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    INISCarEngine(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~INISCarEngine() {}

    virtual void SetNeutralRev(bool b, float throttle, float speed) = 0;
    virtual void RestoreState() = 0;
    virtual void SetNitro(bool b) = 0;
};

#endif
