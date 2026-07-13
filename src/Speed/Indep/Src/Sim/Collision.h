#ifndef SIM_COLLISION_H
#define SIM_COLLISION_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

namespace Sim {

namespace Collision {

void AddListener(IListener *listener, HSIMABLE participant, const char *who);
void AddListener(IListener *listener, const UTL::COM::IUnknown *participant, const char *who);
void RemoveListener(IListener *listener, const UTL::COM::IUnknown *participant);
void RemoveListener(IListener *listener);

}; // namespace Collision

}; // namespace Sim

#endif
