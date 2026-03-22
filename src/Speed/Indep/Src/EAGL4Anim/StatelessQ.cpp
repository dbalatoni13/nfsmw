#include "StatelessQ.h"
#include "FnStatelessF3.h"
#include "FnStatelessQ.h"
#include "StatelessF3.h"

namespace EAGL4Anim {

void StatelessQ::InitAnimMemoryMap(AnimMemoryMap *anim) {
    StatelessQ *statelessQ = reinterpret_cast<StatelessQ *>(anim);
    FnStatelessQ fnStatelessQ;
    FnStatelessQ *fnStatelessQPtr = reinterpret_cast<FnStatelessQ *>(statelessQ->GetFnLocation());

    *reinterpret_cast<void **>(fnStatelessQPtr) = *reinterpret_cast<void **>(&fnStatelessQ);
    fnStatelessQPtr->SetAnimMemoryMap(anim);

    if (statelessQ->mF3Ptr) {
        StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(statelessQ->mF3Ptr);
        FnStatelessF3 fnStatelessF3;
        FnStatelessF3 *fnStatelessF3Ptr = reinterpret_cast<FnStatelessF3 *>(statelessF3->GetFnLocation());

        *reinterpret_cast<void **>(fnStatelessF3Ptr) = *reinterpret_cast<void **>(&fnStatelessF3);
        fnStatelessF3Ptr->SetAnimMemoryMap(statelessF3);
        statelessQ->mF3Ptr = fnStatelessF3Ptr;
    }
}

}; // namespace EAGL4Anim
