#include "EAIEngineRev.hpp"

#include "Speed/Indep/Src/EAXSound/eaxcarstate.hpp"
#include "Speed/Indep/Src/Generated/Messages/MAIEngineRev.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EAIEngineRev::EAIEngineRev(unsigned int phSimable, unsigned int pCarID, unsigned int pPatterPlay)
    : Event(0x20), fhSimable(phSimable), fCarID(pCarID), fPatterPlay(pPatterPlay) {
    ISimable *simable = ISimable::FindInstance((HSIMABLE) phSimable);

    EAX_CarState *state = NULL;

    if (simable) {

        state = EAX_CarState::Find(simable->GetWorldID());
    }

    if (state) {
        MAIEngineRev(fhSimable, fCarID, state, fPatterPlay).Send("QRev");
    }
}

EAIEngineRev::~EAIEngineRev() {
}

const char *EAIEngineRev::GetEventName() const {
    return "EAIEngineRev";
}

void EAIEngineRev_MakeEvent_Callback(const void *staticData) {
    new EAIEngineRev(gEventDynamicData.fhSimable, ((EAIEngineRev::StaticData *) staticData)->fCarID,
                     ((EAIEngineRev::StaticData *) staticData)->fPatterPlay);
}

int EAIEngineRev_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new EAIEngineRev(gEventDynamicData.fhSimable, (unsigned int) lua_tonumber(L, 1), (unsigned int) lua_tonumber(L, 2));
    }
    return 0;
}

void EAIEngineRev_ResolveEvent_Callback(void *event, const UGroup *group) {
}
