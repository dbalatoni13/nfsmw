#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/PackedBinaryTree.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UGroup.hpp"

#include <map>

inline void *operator new(std::size_t, void *place, unsigned int) {
    return place;
}

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

IMPLEMENT_INSTANCABLE(EventSequencer::HENGINE, EventSequencer::IEngine)

namespace EventSequencer {

typedef std::map<UCrc32, const UData *> NameHashToDataMap;

static unsigned int gCreateStimulus;
static unsigned int gTriggerStimulus;
static unsigned int gStartAction;
static unsigned int gEndAction;
static unsigned int gStopAction;
static unsigned int gPauseAction;
static unsigned int gResumeAction;
static unsigned int gUnloadAction;
static unsigned int gStimulusParameter;
static unsigned int gStimulusResult;

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
    for (unsigned int activeIndex = 0; activeIndex < group->DataCountType('eE  '); activeIndex++) {
        const UData *data = group->DataLocate('eE  ', activeIndex);
        const EventSeqEngine *engine = (EventSeqEngine *)data->GetDataConst();
        UCrc32 nameHash(engine->mName);

        gEngineData[nameHash] = data;
    }
}

static void UnregisterEngines(const UGroup *group) {
    for (unsigned int activeIndex = 0; activeIndex < group->DataCountType('eE  '); activeIndex++) {
        const UData *data = group->DataLocate('eE  ', activeIndex);
        const EventSeqEngine *engine = (EventSeqEngine *)data->GetDataConst();
        UCrc32 nameHash(engine->mName);

        gEngineData.erase(nameHash);
    }
}

static const UData *FindEngineData(UCrc32 name) {
    NameHashToDataMap::iterator iter = gEngineData.find(UCrc32(name));
    if (iter != gEngineData.end()) {
        return (*iter).second;
    }

    return nullptr;
}

IEngine *Create(UTL::COM::Object *baseObject, IContext *context, const UData *data, float externalTime, float rate);

IEngine *Create(UTL::COM::Object *baseObject, IContext *context, UCrc32 name, float externalTime, float rate) {
    const UData *data = FindEngineData(UCrc32(name));
    if (data == nullptr) {
        return nullptr;
    }

    return Create(baseObject, context, data, externalTime, rate);
}

const UGroup *PrepareBinary(const void *binData) {
    const UGroup *g = UGroup::Deserialize(binData, true, 0);
    CARP::ResolveTagReferences(g, 0);
    return g;
}

unsigned int StringToID(const char *str) {
    unsigned int hash = stringhash32(str);
    return hash;
}

static bool EraseActiveSystem(unsigned int index) {
    if (index < gNumActiveSystems) {
        for (unsigned int i = index; i < gNumActiveSystems - 1; i++) {
            gActiveSystemList[i] = gActiveSystemList[i + 1];
        }

        gActiveSystemList[--gNumActiveSystems] = nullptr;
        return true;
    }

    return false;
}

}
