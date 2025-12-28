#ifndef SIM_SIMULATION_H
#define SIM_SIMULATION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

namespace Sim {

// TODO move?
// total size: 0x4
class ITimeManager : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ITimeManager(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~ITimeManager() {}

    virtual float OnManageTime(float real_time_delta, float sim_speed);
};

// total size: 0x8
class IStateManager : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IStateManager(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IStateManager() {}

    virtual Sim::State OnManageState(Sim::State state);
    virtual bool ShouldPauseInput();
};

bool Exists();
void StartProfile();
void Suspend();
float GetTime();
unsigned int GetTick();
float GetSpeed();
float GetTimeStep();
const State GetState();
const float GetFrameTimeElapsed();
eUserMode GetUserMode();
void SetStream(UMath::Vector3 &location, bool blocking);
void ProfileTask(HSIMTASK htask, const char *name);
HSIMTASK AddTask(const UCrc32 &schedule, float rate, ITaskable *handler, float start_offset, TaskMode mode);
void RemoveTask(HSIMTASK hTask, ITaskable *handler);
void ModifyTask(HSIMTASK hTask, float rate);

}; // namespace Sim

#endif
