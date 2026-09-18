#include "EResetSystem.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

EResetSystem::EResetSystem(UCrc32 pSystemName, unsigned int pEventSeqEngine, unsigned int pEventSeqSystem)
    : Event(0x20), fSystemName(pSystemName), fEventSeqEngine(pEventSeqEngine), fEventSeqSystem(pEventSeqSystem) {
}

EResetSystem::~EResetSystem() {
    EventSequencer::IEngine *iengine = NULL;

    if (fEventSeqEngine) {
        iengine = EventSequencer::IEngine::FindInstance((EventSequencer::HENGINE) fEventSeqEngine);
    }

    if (iengine) {
        UCrc32 systemname = fSystemName;

        if (systemname == UCrc32("this")) {
            systemname = UCrc32(fEventSeqSystem);
        }

        if (systemname != UCrc32::kNull) {
            EventSequencer::System *system = iengine->FindSystem(systemname.GetValue());

            if (system) {
                system->Reset(Sim::GetTime(), system->GetActionRate(), NULL);
            }
        }
    }
}

const char *EResetSystem::GetEventName() const {
    return "EResetSystem";
}

void EResetSystem_MakeEvent_Callback(const void *staticData) {
    new EResetSystem(((EResetSystem::StaticData *) staticData)->fSystemName, gEventDynamicData.fEventSeqEngine, gEventDynamicData.fEventSeqSystem);
}

int EResetSystem_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EResetSystem(UCrc32(lua_tostring(L, 1)), gEventDynamicData.fEventSeqEngine, gEventDynamicData.fEventSeqSystem);
    }
    return 0;
}

void EResetSystem_ResolveEvent_Callback(void *event, const UGroup *group) {
}
