#include "EJumpToStrategyFlow.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

EJumpToStrategyFlow::EJumpToStrategyFlow() : Event(0x10) {}

EJumpToStrategyFlow::~EJumpToStrategyFlow() {
    if (SoundAI::Get()) {
        SoundAI::Get()->SetFocus(SoundAI::kStrategyFlow);
    }
}

const char *EJumpToStrategyFlow::GetEventName() const {
    return "EJumpToStrategyFlow";
}

void EJumpToStrategyFlow_MakeEvent_Callback(const void *staticData) {
    new EJumpToStrategyFlow();
}

int EJumpToStrategyFlow_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EJumpToStrategyFlow();
    }
    return 0;
}

void EJumpToStrategyFlow_ResolveEvent_Callback(void *event, const UGroup *group) {
}
