#include "EDDaySpeech.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Speech/MiscSpeech.h"

EDDaySpeech::EDDaySpeech() : Event(0x10) {}

EDDaySpeech::~EDDaySpeech() {
    MiscSpeech::D_Day();
}

const char *EDDaySpeech::GetEventName() const {
    return "EDDaySpeech";
}

void EDDaySpeech_MakeEvent_Callback(const void *staticData) {
    new EDDaySpeech();
}

int EDDaySpeech_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EDDaySpeech();
    }
    return 0;
}

void EDDaySpeech_ResolveEvent_Callback(void *event, const UGroup *group) {
}
