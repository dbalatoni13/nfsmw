#include "StatelessQ.h"
#include "FnStatelessF3.h"
#include "FnStatelessQ.h"
#include "StatelessF3.h"

namespace EAGL4Anim {

void StatelessQ::InitAnimMemoryMap(AnimMemoryMap *anim) {
    StatelessQ *statelessQ = reinterpret_cast<StatelessQ *>(anim);
    FnStatelessQ *fnStatelessQ = reinterpret_cast<FnStatelessQ *>(statelessQ->GetFnLocation());

    FnStatelessQ::PatchVtbl(fnStatelessQ);

    if (statelessQ->mF3Ptr) {

        StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(statelessQ->mF3Ptr);
        FnStatelessF3 *fnStatelessF3 = reinterpret_cast<FnStatelessF3 *>(statelessF3->GetFnLocation());

        FnStatelessF3::PatchVtbl(fnStatelessF3);

        fnStatelessF3->SetAnimMemoryMap(statelessF3);
        statelessQ->mF3Ptr = fnStatelessF3;
    }
}

}; // namespace EAGL4Anim
