#ifndef INTERFACES_ILISTENER_H
#define INTERFACES_ILISTENER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Sim {
namespace Collision {

class IListener {
  public:
    virtual void OnCollision(const class Info &cinfo);
};

} // namespace Collision
} // namespace Sim

#endif
