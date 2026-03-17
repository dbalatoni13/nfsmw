#ifndef GAMEPLAY_GOBJECTBLOCK_H
#define GAMEPLAY_GOBJECTBLOCK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"

class GVault;
class GRuntimeInstance;

// total size: 0x50
struct GObjectBlock {
    GObjectBlock(GVault *vault, unsigned char *buffer);
    ~GObjectBlock();

    void Initialize(unsigned int bufferSize);
    static unsigned int CalcSpaceRequired(GVault *vault, unsigned int *outObjCount);
    static bool CollectionIsInstanceOfTemplate(Attrib::Gen::gameplay &instanceObj, Attrib::Gen::gameplay &templateObj);
    static unsigned int CalcNumConnections(unsigned int collectionKey);

    template <typename T>
    void DeleteObjects();

    template <typename T>
    unsigned int CreateObjects(GVault *vault, unsigned char *buffer);

    template <typename T>
    static unsigned int GetPaddedObjectSize() {
        return (sizeof(T) + 15) & ~15;
    }

    GVault *mVault;                       // offset 0x0, size 0x4
    unsigned char *mObjectBuffer;         // offset 0x4, size 0x4
    unsigned int mObjectCount[6];         // offset 0x8, size 0x18
    unsigned int mObjectSize[6];          // offset 0x20, size 0x18
    GRuntimeInstance *mObjectList[6];     // offset 0x38, size 0x18
};

#endif
