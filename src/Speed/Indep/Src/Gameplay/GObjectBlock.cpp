#include "Speed/Indep/Src/Gameplay/GObjectBlock.h"

#include "Speed/Indep/Src/Gameplay/GVault.h"

GObjectBlock::GObjectBlock(GVault *vault, unsigned char *buffer)
    : mVault(vault), //
      mObjectBuffer(buffer) {}

GObjectBlock::~GObjectBlock() {}

void GObjectBlock::Initialize(unsigned int bufferSize) {}

unsigned int GObjectBlock::CalcSpaceRequired(GVault *vault, unsigned int *outObjCount) {
    if (outObjCount) {
        *outObjCount = 0;
    }

    return 0;
}

bool GObjectBlock::CollectionIsInstanceOfTemplate(Attrib::Gen::gameplay &instanceObj, Attrib::Gen::gameplay &templateObj) {
    return false;
}

unsigned int GObjectBlock::CalcNumConnections(unsigned int collectionKey) {
    return 0;
}
