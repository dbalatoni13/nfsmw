#ifndef WORLD_WWORLD_H
#define WORLD_WWORLD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/world.h"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"

// total size: 0x24
class WWorld {
  public:
    void *operator new(size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, size_t size) {
        gFastMem.Free(mem, size, nullptr);
    }

    // static void *operator new(unsigned int size) {}

    // static void operator delete(void *mem, unsigned int size) {}

    // static void *operator new(unsigned int size, const char *name) {}

    // static void operator delete(void *mem, const char *name) {}

    // static void operator delete(void *mem, unsigned int size, const char *name) {}

    WWorld();
    ~WWorld();

    // static bool IsPresent() {}

    static WWorld &Get() {
        return *fgWorld;
    }

    static void Init();

    // static void Shutdown() {}

    int Loader(bChunk *chunk);
    int Unloader(bChunk *chunk);
    bool Open();
    void Close();

    const Attrib::Gen::world &GetAttributes() const {
        return fAttributes;
    }

    bool IsValid() {
        return fRootWorldGroup != nullptr;
    }

    const UGroup &GetMapGroup() const {
        return *fRootWorldGroup;
    }

    const UGroup *GetMapGroup() {
        return fRootWorldGroup;
    }

    void GetStartPosition(UMath::Vector3 &position);

  private:
    static WWorld *fgWorld;               // size: 0x4
    Attrib::Gen::world fAttributes;       // offset 0x0, size 0x14
    const UGroup *fRootWorldGroup;        // offset 0x14, size 0x4
    const void *fCarpData;                // offset 0x18, size 0x4
    unsigned int fCarpDataSize;           // offset 0x1C, size 0x4
    CARP::Map *fMap;                      // offset 0x20, size 0x4
    static UMath::Vector4 mStartPosition; // size: 0x10
};

#endif
