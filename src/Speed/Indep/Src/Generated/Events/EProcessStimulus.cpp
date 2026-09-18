#include "EProcessStimulus.hpp"

#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Main/EventSequencer.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

EProcessStimulus::EProcessStimulus(UCrc32 pSystemName, UCrc32 pStimulus, UCrc32 pQueueMode, unsigned int pEventSeqEngine,
                                   unsigned int pEventSeqSystem)
    : Event(0x20), fSystemName(pSystemName), fStimulus(pStimulus), fQueueMode(pQueueMode), fEventSeqEngine(pEventSeqEngine),
      fEventSeqSystem(pEventSeqSystem) {
}

EProcessStimulus::~EProcessStimulus() {
    EventSequencer::IEngine *iengine = NULL;

    if (fEventSeqEngine) {
        iengine = EventSequencer::IEngine::FindInstance((EventSequencer::HENGINE) fEventSeqEngine);
    }

    if (iengine) {
        UCrc32 systemname = fSystemName;

        if (systemname == UCrc32("this")) {
            systemname = UCrc32(fEventSeqSystem);
        }

        EventSequencer::QueueMode mode = EventSequencer::QUEUE_ALLOW;

        switch (fQueueMode.GetValue()) {
            case 0:
            case UCRC32_QUEUE_ALLOW:
                mode = EventSequencer::QUEUE_ALLOW;
                break;
            case UCRC32_QUEUE_SHALLOW:
                mode = EventSequencer::QUEUE_SHALLOW;
                break;
            case UCRC32_QUEUE_DISABLE:
                mode = EventSequencer::QUEUE_DISABLE;
                break;
            case UCRC32_QUEUE_FLUSH:
                mode = EventSequencer::QUEUE_FLUSH;
                break;
            case UCRC32_QUEUE_ABORT:
                mode = EventSequencer::QUEUE_ABORT;
                break;

            default:
                bBreak();
                break;
        }

        if (systemname != UCrc32::kNull) {
            EventSequencer::System *system = iengine->FindSystem(systemname.GetValue());

            if (system) {
                system->ProcessStimulus(fStimulus.GetValue(), Sim::GetTime(), NULL, mode);
            }
        } else {
            iengine->ProcessStimulus(fStimulus.GetValue(), Sim::GetTime(), NULL, mode);
        }
    }
}

const char *EProcessStimulus::GetEventName() const {
    return "EProcessStimulus";
}

void EProcessStimulus_MakeEvent_Callback(const void *staticData) {
    new EProcessStimulus(((EProcessStimulus::StaticData *) staticData)->fSystemName, ((EProcessStimulus::StaticData *) staticData)->fStimulus,
                         ((EProcessStimulus::StaticData *) staticData)->fQueueMode, gEventDynamicData.fEventSeqEngine,
                         gEventDynamicData.fEventSeqSystem);
}

int EProcessStimulus_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 3) {
        new EProcessStimulus(UCrc32(lua_tostring(L, 1)), UCrc32(lua_tostring(L, 2)), UCrc32(lua_tostring(L, 3)), gEventDynamicData.fEventSeqEngine,
                             gEventDynamicData.fEventSeqSystem);
    }
    return 0;
}

void EProcessStimulus_ResolveEvent_Callback(void *event, const UGroup *group) {
}
