#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/PackedBinaryTree.h"
#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"

// unfinished
static CARP::ExprValType StimulusFilterLookup(unsigned int name, unsigned int subindex, const void *context, const CARP::ExprValType *value) {
    CARP::ExprValType result = *value;
    const CARP::StimulusFilter *sf = reinterpret_cast<const CARP::StimulusFilter *>(context);
    unsigned int index = SearchPackedBinaryTree<const QueryDesc, unsigned int>(sf->mNumQueries, sf->GetQueries(), name);

    if (index < value->u && (unsigned int)context < value->u) {
        const QueryDesc *qDesc = sf->GetQueries() + index;
        result.u = qDesc->mDataOffset;
    } else {
        result.u = 0;
    }

    return result;
}

namespace EventSequencer {

typedef struct UTL::Std::map<UCrc32,const UData *,_STL::less<UCrc32>,_STL::allocator<_STL::pair<const UCrc32,const UData *> > > NameHashToDataMap;

static NameHashToDataMap gEngineData;
static unsigned int gNumActiveSystems = 0;
static float gLastUpdateTime = 0.0f;
static System *gActiveSystemList[1024];

void Init(float externalTime) {
    gLastUpdateTime = externalTime;
    gNumActiveSystems = 0;
}

void Reset(float externalTime) {
    gLastUpdateTime = externalTime;
}

void Update(float externalTime) {
    if (externalTime <= gLastUpdateTime) {
        return;
    }

    for (unsigned int activeIndex = 0; activeIndex < gNumActiveSystems;) {
        if (!gActiveSystemList[activeIndex]->Update(activeIndex, externalTime)) {
            activeIndex++;
        }
    }

    gLastUpdateTime = externalTime;
}

void UpdateDelta(float deltaTime) {
    if (deltaTime <= 0.0f) return;

    Update(gLastUpdateTime + deltaTime);
}

static void RegisterEngines(const UGroup *group) {

}

}
