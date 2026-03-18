#include "Speed/Indep/Src/EAXSound/NFSMixMaster.hpp"

void *NFSMixMaster::operator new(unsigned int size, const char *debug_name) {
    return gAudioMemoryManager.AllocateMemory(size, debug_name, false);
}
