#include "AnimInternal.hpp"
#include "Speed/Indep/Src/EAGL4Anim/eagl4runtimetransform.h"
#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdlopen.h"

EAGL4::DynamicLoader *AnimBridgeNewDynamicLoader(char *name, char *data, int size) {
    return new ("TODO") EAGL4::DynamicLoader(data, size, nullptr);
}

void AnimBridgeDeleteDynamicLoader(EAGL4::DynamicLoader *pDynLoader) {
    delete pDynLoader;
}

EAGL4::Transform *AnimBridgeNewTransform(char *name, int size) {
    return new (name) EAGL4::Transform[size];
}

void AnimBridgeDeleteTransform(EAGL4::Transform *pTransform) {
    delete[] pTransform;
}
