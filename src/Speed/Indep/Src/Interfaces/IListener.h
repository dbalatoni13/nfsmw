#ifndef INTERFACES_ILISTENER_H
#define INTERFACES_ILISTENER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Sim/Collision.h"

namespace Sim {
namespace Collision {

class IListener {
  public:
    virtual void OnCollision(const Info &cinfo);
};

void RemoveListener(IListener *listener);

} // namespace Collision
} // namespace Sim

#endif
