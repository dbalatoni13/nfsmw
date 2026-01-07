#include "../SimObject.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IServiceable.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

namespace Sim {

Object::Object(std::size_t num_interfaces)
    : UTL::COM::Object(num_interfaces + 2), //
      IServiceable(this),                   //
      ITaskable(this),                      //
      mTaskCount(0),                        //
      mServiceCount(0) {}

Object::~Object() {}

void Object::ModifyTask(HSIMTASK htask, float rate) {
    Sim::ModifyTask(htask, rate);
}

HSIMTASK Object::AddTask(const UCrc32 &schedule, float rate, float start_offset, TaskMode mode) {
    HSIMTASK htask = Sim::AddTask(schedule, rate, this, start_offset, mode);
    if (htask) {
        mTaskCount++;
    }
    return htask;
}

void Object::RemoveTask(HSIMTASK htask) {
    if (htask) {
        mTaskCount--;
        Sim::RemoveTask(htask, this);
    }
}

ConnStatus Object::CheckService(HSIMSERVICE hservice) const {
    return Sim::CheckService(hservice);
}

HSIMSERVICE Object::OpenService(UCrc32 server, Packet *pkt) {
    HSIMSERVICE hservice = Sim::OpenService(server, this, pkt);
    if (hservice) {
        mServiceCount++;
    }
    return hservice;
}

void Object::CloseService(HSIMSERVICE hservice) {
    if (hservice) {
        mServiceCount--;
        Sim::CloseService(hservice);
    }
}

}; // namespace Sim
