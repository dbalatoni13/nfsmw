#ifndef FRONTEND_HUD_FEMENUZONETRIGGER_H
#define FRONTEND_HUD_FEMENUZONETRIGGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct GRuntimeInstance;

struct IMenuZoneTrigger : public UTL::COM::IUnknown {
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    IMenuZoneTrigger(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IMenuZoneTrigger() {}

  public:
    virtual bool ShouldSeeMenuZoneCluster();
    virtual bool IsPlayerInsideTrigger();
    virtual void EnterTriggerForAutoSave();
    virtual void ExitTriggerForAutoSave();
    virtual void EnterTrigger(GRuntimeInstance *pRaceActivity);
    virtual void EnterTrigger(char *zoneType);
    virtual void ExitTrigger();
    virtual void RequestEventInfoDialog(int port);
    virtual void RequestZoneInfoDialog(int port);
    virtual bool IsType(const char *t);
    virtual void RequestDoAction();
};

#endif
