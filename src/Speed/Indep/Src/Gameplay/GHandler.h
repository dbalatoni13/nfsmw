#ifndef GAMEPLAY_GHANDLER_H
#define GAMEPLAY_GHANDLER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "GRuntimeInstance.h"

// total size: 0x2C
class GHandler : public GRuntimeInstance {
  public:
    GHandler(const Attrib::Key &handlerKey);

  private:
    bool mAttached; // offset 0x28, size 0x1
};

#endif
