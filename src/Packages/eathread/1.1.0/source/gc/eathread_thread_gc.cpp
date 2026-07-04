#include "eathread/eathread_thread.h"
#include "eathread/powerpc/eathread_atomic_powerpc.h"

namespace EA {
namespace Thread {

EAThreadDynamicData *AllocateThreadDynamicData() {}

static const unsigned int kMaxThreadDynamicDataCount = 32;

char gThreadDynamicData[832][kMaxThreadDynamicDataCount] = {0};
AtomicInt<int> gThreadDynamicDataAllocated[kMaxThreadDynamicDataCount];

}
}
