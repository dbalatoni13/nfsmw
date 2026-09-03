#ifndef GAMEPLAY_GRUNTIMEINSTANCE_H
#define GAMEPLAY_GRUNTIMEINSTANCE_H

#include "GUserIncludes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"

// total size: 0x28
class GRuntimeInstance : public Attrib::Gen::gameplay {
  public:
    GRuntimeInstance(const Attrib::Key &key, GameplayObjType type);
    virtual ~GRuntimeInstance();

    virtual GameplayObjType GetType() const; // Decl: 47

  private:
    uint16 mFlags;                        // offset 0x14, size 0x2
    uint16 mNumConnected;                 // offset 0x16, size 0x2
    struct ConnectedInstance *mConnected; // offset 0x18, size 0x4
    GRuntimeInstance *mPrev;              // offset 0x1C, size 0x4
    GRuntimeInstance *mNext;              // offset 0x20, size 0x4
};

#endif
