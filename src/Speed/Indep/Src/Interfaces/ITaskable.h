#ifndef INTTERFACES_ITASKABLE_H
#define INTTERFACES_ITASKABLE_H

#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

namespace Sim {

enum TaskMode {
    TASK_FRAME_FIXED = 0,
    TASK_FRAME_VARIABLE = 1,
};

// total size: 0x8
class ITaskable : public UTL::COM::IUnknown {
  public:
    DECL_INTERFACE(ITaskable);

  public:
    virtual bool OnTask(HSIMTASK htask, float dT);
};

}; // namespace Sim

#endif
