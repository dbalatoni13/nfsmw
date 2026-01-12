#ifndef AI_ACTIVITIES_AVOIDABLEMANAGER_H
#define AI_ACTIVITIES_AVOIDABLEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"

// total size: 0x54
class AvoidableManager : public Sim::Activity, public Debugable {
  public:
    static Sim::IActivity *Construct(Sim::Param params);

    AvoidableManager(Sim::Param params);

    // Virtual functions
    virtual void OnDebugDraw();

    // Virtual overrides
    // IUnknown
    ~AvoidableManager() override;

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

  private:
    HSIMTASK mSimulateTask; // offset 0x50, size 0x4
};

#endif
