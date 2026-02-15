#ifndef INTERFACES_SIMABLES_IHELICOPTER_H
#define INTERFACES_SIMABLES_IHELICOPTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

class ISimpleChopper : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ISimpleChopper(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    ~ISimpleChopper() override {}

  public:
    virtual void SetDesiredVelocity(const UMath::Vector3 &vel);
    virtual void GetDesiredVelocity(UMath::Vector3 &vel);
    virtual void MaxDeceleration(bool t);
    virtual void SetDesiredFacingVector(const UMath::Vector3 &facingDir);
    virtual void GetDesiredFacingVector(UMath::Vector3 &facingDir);
};

#endif
