#include "eagl4supportdef.h"

namespace EAGL4Internal {

void *DefaultMalloc(unsigned int size, const char *name);
void DefaultFree(void *block, unsigned int size);

// El orden es el del .data del objetivo, leido de symbols.txt:
//   0x804170F4 gEAGL4ANIM_Malloc / 0x804170F8 gEAGL4ANIM_Free
//   0x804170FC EAGL4Malloc      / 0x80417100 EAGL4Free
// Los teniamos al reves por parejas; los bytes casaban (los cuatro son 0 en
// el objeto, con reubicacion) pero las direcciones no.
MallocFunctionType gEAGL4ANIM_Malloc = 0;
FreeFunctionType gEAGL4ANIM_Free = 0;

MallocFunctionType EAGL4Malloc = DefaultMalloc;
FreeFunctionType EAGL4Free = DefaultFree;

void SetMallocOverride(MallocFunctionType nFunc) {
    gEAGL4ANIM_Malloc = nFunc;
}

void SetFreeOverride(FreeFunctionType nFunc) {
    gEAGL4ANIM_Free = nFunc;
}

void *DefaultMalloc(unsigned int size, const char *name) {
    return gEAGL4ANIM_Malloc(size, name);
}

void DefaultFree(void *block, unsigned int size) {
    gEAGL4ANIM_Free(block, size);
}

}; // namespace EAGL4Internal
