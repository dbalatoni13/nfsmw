#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"

namespace CARP {

static void EventSeqEngineResolver(const UData *d, const UGroup *context) {
    const EventSeqEngine *e = static_cast<const EventSeqEngine *>(d->GetDataConst());
    new (const_cast<EventSeqEngine *>(e)) TagReference(context);

    const EventSeqSystem *const *systems = e->GetSystems();
    for (unsigned int i = 0; i < e->mNumSystems; i++) {
        new (const_cast<EventSeqSystem **>(&systems[i])) TagReference(context);
    }
}

}
