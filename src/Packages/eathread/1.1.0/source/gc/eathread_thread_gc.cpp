#include "eathread/eathread_thread.h"
#include "eathread/powerpc/eathread_atomic_powerpc.h"

namespace EA {
namespace Thread {

static const unsigned int kMaxThreadDynamicDataCount = 32;

// Ni las dos tablas ni las funciones que las usan llegan al DOL: el enlazador las
// estripa y solo queda el static-init que construye los 32 AtomicInt (DWARF).
char gThreadDynamicData[kMaxThreadDynamicDataCount][sizeof(EAThreadDynamicData)];
AtomicInt32 gThreadDynamicDataAllocated[kMaxThreadDynamicDataCount];

EAThreadDynamicData *AllocateThreadDynamicData() {
    for (unsigned int i = 0; i < kMaxThreadDynamicDataCount; i++) {
        if (gThreadDynamicDataAllocated[i].SetValueConditional(1, 0)) {
            return reinterpret_cast<EAThreadDynamicData *>(gThreadDynamicData[i]);
        }
    }
    return nullptr;
}

} // namespace Thread
} // namespace EA
