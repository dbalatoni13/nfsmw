#include "eagl4supportdef.h"

namespace EAGL4Internal {

MallocFunctionType EAGL4Malloc;
FreeFunctionType EAGL4Free;

MallocFunctionType gEAGL4ANIM_Malloc;
FreeFunctionType gEAGL4ANIM_Free;

void SetMallocOverride(MallocFunctionType nFunc) {
    gEAGL4ANIM_Malloc = nFunc;
}

void SetFreeOverride(FreeFunctionType nFunc) {
    gEAGL4ANIM_Free = nFunc;
}

static void *DefaultMalloc(unsigned int size, const char *name) {
    return gEAGL4ANIM_Malloc(size, name);
}

static void DefaultFree(void *block, unsigned int size) {
    gEAGL4ANIM_Free(block, size);
}

}; // namespace EAGL4Internal
