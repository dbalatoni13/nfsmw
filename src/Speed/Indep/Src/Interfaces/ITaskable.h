#ifndef INTTERFACES_ITASKABLE_H
#define INTTERFACES_ITASKABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

struct HSIMTASK__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef HSIMTASK__ *HSIMTASK;

namespace Sim {

enum TaskMode {
    TASK_FRAME_FIXED = 0,
    TASK_FRAME_VARIABLE = 1,
};

// total size: 0x8
class ITaskable : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

  protected:
    virtual ~ITaskable() {}

    ITaskable(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  private:
    ITaskable();
    ITaskable(const ITaskable &);
    ITaskable &operator=(const ITaskable &);

  public:
    virtual bool OnTask(HSIMTASK htask, float dT);
};

}; // namespace Sim

#endif
