#ifndef INPUT_IFEEDBACK_H
#define INPUT_IFEEDBACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Physics/PhysicsTypes.h"
#include "Speed/Indep/Src/Sim/Collision.h"

class IFeedback : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IFeedback(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IFeedback() {}

    virtual void PauseEffects() = 0;
    virtual void ResumeEffects() = 0;
    virtual void ResetEffects() = 0;
    virtual void BeginUpdate() = 0;
    virtual void EndUpdate() = 0;
    virtual void UpdateRoadNoise(bool front, const struct SimSurface &surface, float speed) = 0;
    virtual void UpdateTireSkid(bool front, const struct SimSurface &surface, float speed) = 0;
    virtual void UpdateTireSlip(bool front, const struct SimSurface &surface, float speed) = 0;
    virtual void UpdateRPM(float powerband, float overrev, float throttle) = 0;
    virtual void UpdateShiftPotential(ShiftPotential potential) = 0;
    virtual void UpdateNOS(bool engaged, float NOSLevel) = 0;
    virtual void UpdateEngineBlown(bool blown) = 0;
    virtual void UpdateShifting(bool shifting) = 0;
    virtual void ReportCollision(const COLLISION_INFO &cinfo, bool iamA) = 0;
};

#endif
