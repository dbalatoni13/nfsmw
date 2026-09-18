#include "ESndGameState.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"

ESndGameState::ESndGameState(int pState, bool pTurningOn) : Event(0x10), fState(pState), fTurningOn(pTurningOn) {
}

ESndGameState::~ESndGameState() {
    GameFlowSndState[fState] = fTurningOn ? 32767 : 0;
}

const char *ESndGameState::GetEventName() const {
    return "ESndGameState";
}

void ESndGameState_MakeEvent_Callback(const void *staticData) {
    new ESndGameState(((ESndGameState::StaticData *) staticData)->fState, ((ESndGameState::StaticData *) staticData)->fTurningOn);
}

int ESndGameState_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ESndGameState((int) lua_tonumber(L, 1), lua_toboolean(L, 2));
    }
    return 0;
}

void ESndGameState_ResolveEvent_Callback(void *event, const UGroup *group) {
}
