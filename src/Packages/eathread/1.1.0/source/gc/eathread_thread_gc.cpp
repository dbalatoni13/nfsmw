#include "eathread/eathread_thread.h"
#include "eathread/powerpc/eathread_atomic_powerpc.h"

namespace EA {
namespace Thread {

EAThreadDynamicData *AllocateThreadDynamicData() {

}

// char gThreadDynamicData[832][32] = {0};
AtomicInt32 gThreadDynamicDataAllocated[32];

}
}
