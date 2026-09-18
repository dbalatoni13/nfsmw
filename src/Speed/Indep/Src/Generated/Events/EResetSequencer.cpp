#include "EResetSequencer.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

EResetSequencer::EResetSequencer(unsigned int pEventSeqEngine) : Event(0x10), fEventSeqEngine(pEventSeqEngine) {
}

EResetSequencer::~EResetSequencer() {
    EventSequencer::IEngine *iengine = NULL;

    if (fEventSeqEngine) {
        iengine = EventSequencer::IEngine::FindInstance((EventSequencer::HENGINE) fEventSeqEngine);
    }

    if (iengine) {
        iengine->Reset(Sim::GetTime());
    }
}

const char *EResetSequencer::GetEventName() const {
    return "EResetSequencer";
}

void EResetSequencer_MakeEvent_Callback(const void *staticData) {
    new EResetSequencer(gEventDynamicData.fEventSeqEngine);
}

int EResetSequencer_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EResetSequencer(gEventDynamicData.fEventSeqEngine);
    }
    return 0;
}

void EResetSequencer_ResolveEvent_Callback(void *event, const UGroup *group) {
}
