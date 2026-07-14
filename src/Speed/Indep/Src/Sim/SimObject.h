#ifndef SIM_SIM_OBJECT_H
#define SIM_SIM_OBJECT_H

#include "SimServer.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/IServiceable.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"

namespace Sim {

// total size: 0x2C
class Object : public UTL::COM::Object, public IServiceable, public ITaskable, public UTL::Collections::Countable<Object> {
  public:
    USE_FASTALLOC(Sim::Object);

  protected:
    Object(std::size_t num_interfaces);
    HSIMTASK AddTask(const UCrc32 &schedule, float rate, float start_offset, TaskMode mode);
    void ModifyTask(HSIMTASK htask, float rate);
    void RemoveTask(HSIMTASK htask);
    HSIMSERVICE OpenService(UCrc32 server, Packet *pkt);
    void CloseService(HSIMSERVICE hservice);
    ConnStatus CheckService(HSIMSERVICE hservice) const;

    ~Object() override;

    // IServiceable
    bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt) override {
        return false;
    }

    // ITaskable
    bool OnTask(HSIMTASK htask, float dT) override {
        return false;
    }

  private:
    unsigned int mTaskCount;    // offset 0x24, size 0x4
    unsigned int mServiceCount; // offset 0x28, size 0x4
};

}; // namespace Sim

#endif
