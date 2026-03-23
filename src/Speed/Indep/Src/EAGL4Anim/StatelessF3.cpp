#include "StatelessF3.h"
#include "FnStatelessF3.h"

namespace EAGL4Anim {

void StatelessF3::InitAnimMemoryMap(AnimMemoryMap *anim) {
    StatelessF3 *statelessF3 = reinterpret_cast<StatelessF3 *>(anim);
    FnStatelessF3 *fnStatelessF3Ptr = reinterpret_cast<FnStatelessF3 *>(statelessF3->GetFnLocation());

    {
        FnStatelessF3 fnStatelessF3;
        *reinterpret_cast<void **>(fnStatelessF3Ptr) = *reinterpret_cast<void **>(&fnStatelessF3);
    }
}

}; // namespace EAGL4Anim
