#ifndef GAMEPLAY_GRUNTIMEINSTANCE_H
#define GAMEPLAY_GRUNTIMEINSTANCE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"

// total size: 0x28
class GRuntimeInstance : public Attrib::Gen::gameplay {
  public:
    GRuntimeInstance(const Attrib::Key &key, GameplayObjType type);

  private:
    unsigned short mFlags;                // offset 0x14, size 0x2
    unsigned short mNumConnected;         // offset 0x16, size 0x2
    struct ConnectedInstance *mConnected; // offset 0x18, size 0x4
    GRuntimeInstance *mPrev;              // offset 0x1C, size 0x4
    GRuntimeInstance *mNext;              // offset 0x20, size 0x4
};

#endif
