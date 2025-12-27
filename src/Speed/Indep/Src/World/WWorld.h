#ifndef WORLD_WWORLD_H
#define WORLD_WWORLD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/world.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

// total size: 0x24
class WWorld {
  public:
    static void Init();

    WWorld();

    int Loader(bChunk *chunk);

    int Unloader(bChunk *chunk);

    ~WWorld();

    bool Open();

    void GetStartPosition(UMath::Vector3 &position);

    void Close();

    // static void *operator new(unsigned int size, void *ptr) {}

    // static void operator delete(void *mem, void *ptr) {}

    // static void *operator new(unsigned int size) {}

    // static void operator delete(void *mem, unsigned int size) {}

    // static void *operator new(unsigned int size, const char *name) {}

    // static void operator delete(void *mem, const char *name) {}

    // static void operator delete(void *mem, unsigned int size, const char *name) {}

    // static bool IsPresent() {}

    // static struct WWorld &Get() {}

    // static void Shutdown() {}

    // const struct world &GetAttributes() const {}

    // bool IsValid() {}

    // const struct UGroup &GetMapGroup() const {}

    // const struct UGroup *GetMapGroup() {}

  private:
    static WWorld *fgWorld;               // size: 0x4
    static UMath::Vector4 mStartPosition; // size: 0x10

    Attrib::Gen::world fAttributes; // offset 0x0, size 0x14
    const UGroup *fRootWorldGroup;  // offset 0x14, size 0x4
    const void *fCarpData;          // offset 0x18, size 0x4
    unsigned int fCarpDataSize;     // offset 0x1C, size 0x4
    struct Map *fMap;               // offset 0x20, size 0x4
};

#endif
