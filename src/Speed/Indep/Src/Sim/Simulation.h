#ifndef __SIMULATION_H
#define __SIMULATION_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Math/SimRandom.h"

namespace Sim {

enum State {
    STATE_NONE = 0,
    STATE_INITIALIZING = 1,
    STATE_ACTIVE = 3,
    STATE_IDLE = 4,
};

enum eUserMode {
    USER_SINGLE = 0,
    USER_SPLIT_SCREEN = 1,
    USER_ONLINE = 2,
};

// total size: 0x4
class ITimeManager : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ITimeManager);

    virtual float OnManageTime(float real_time_delta, float sim_speed);
};

// total size: 0x8
class IStateManager : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(IStateManager);

    virtual Sim::State OnManageState(Sim::State state);
    virtual bool ShouldPauseInput();
};

bool Exists();
void StartProfile();
void Suspend();
float GetTime();
SimRandom &GetRandom();
unsigned int GetTick();
float GetSpeed();
float GetTimeStep();
const State GetState();
const float GetFrameTimeElapsed();
eUserMode GetUserMode();
void SetStream(const UMath::Vector3 &location, bool blocking);
void ProfileTask(HSIMTASK htask, const char *name);
HSIMTASK AddTask(const UCrc32 &schedule, float rate, ITaskable *handler, float start_offset, TaskMode mode);
void RemoveTask(HSIMTASK hTask, ITaskable *handler);
void ModifyTask(HSIMTASK hTask, float rate);
float DistanceToCamera(const UMath::Vector3 &v);
void Init(const UCrc32 &track, eUserMode mode);
void Shutdown();

}; // namespace Sim

#endif
