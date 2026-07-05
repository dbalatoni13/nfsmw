#ifndef EATHREAD_EATHREAD_THREAD_H
#define EATHREAD_EATHREAD_THREAD_H

#include "dolphin/os/OSThread.h"
#include "eathread/powerpc/eathread_atomic_powerpc.h"
#include "types.h"

typedef unsigned long long ThreadId;

namespace EA {
namespace Thread {

struct EAThreadDynamicData {
    EAThreadDynamicData() ; // Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:74

    ~EAThreadDynamicData() ; // Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:75

    void AddRef() ; // Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:76

    void Release() ; // Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:77

    struct OSThread mThread; // offset 0x0, size 0x318
    ThreadId mThreadId; // offset 0x318, size 0x4, Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:79
    int mnStatus; // offset 0x31C, size 0x4, Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:80
    intptr_t mnReturnValue; // offset 0x320, size 0x4, Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:81
    void * mpStartContext[2]; // offset 0x324, size 0x8, Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:82
    bool mbFunctionOrRunnable; // offset 0x32C, size 0x1, Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:83
    void * mpStack; // offset 0x330, size 0x4, Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:85
    void * mpAllocatedStack; // offset 0x334, size 0x4, Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:86
    AtomicInt<int> mnRefCount; // offset 0x338, size 0x4, Decl: speed/indep/libs/eathread/1.09.00/include/eathread/eathread_thread.h:88
};

}
}

#endif
