#ifndef SIM_SIM_OBJECT_H
#define SIM_SIM_OBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/IServiceable.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"

namespace Sim {

// total size: 0x2C
class Object : public UTL::COM::Object, public IServiceable, public ITaskable, public UTL::Collections::Countable<Object> {
  public:
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

  protected:
    Object(unsigned int num_interfaces);
    HSIMTASK AddTask(const UCrc32 &schedule, float rate, float start_offset, TaskMode mode);
    void RemoveTask(HSIMTASK htask);

    virtual ~Object();

    virtual bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt);

    virtual bool OnTask(HSIMTASK htask, float dT) {}

  private:
    unsigned int mTaskCount;    // offset 0x24, size 0x4
    unsigned int mServiceCount; // offset 0x28, size 0x4
};

}; // namespace Sim

#endif
