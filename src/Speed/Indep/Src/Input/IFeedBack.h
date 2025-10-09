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

    virtual void PauseEffects();
    virtual void ResumeEffects();
    virtual void ResetEffects();
    virtual void BeginUpdate();
    virtual void EndUpdate();
    virtual void UpdateRoadNoise(bool front, const struct SimSurface &surface, float speed);
    virtual void UpdateTireSkid(bool front, const struct SimSurface &surface, float speed);
    virtual void UpdateTireSlip(bool front, const struct SimSurface &surface, float speed);
    virtual void UpdateRPM(float powerband, float overrev, float throttle);
    virtual void UpdateShiftPotential(ShiftPotential potential);
    virtual void UpdateNOS(bool engaged, float NOSLevel);
    virtual void UpdateEngineBlown(bool blown);
    virtual void UpdateShifting(bool shifting);
    virtual void ReportCollision(const COLLISION_INFO &cinfo, bool iamA);
};

#endif
