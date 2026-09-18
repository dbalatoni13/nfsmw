#include "EDispIntroRace.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Speech/MiscSpeech.h"

EDispIntroRace::EDispIntroRace() : Event(0x10) {}

EDispIntroRace::~EDispIntroRace() {
    MiscSpeech::DispIntroRace();
}

const char *EDispIntroRace::GetEventName() const {
    return "EDispIntroRace";
}

void EDispIntroRace_MakeEvent_Callback(const void *staticData) {
    new EDispIntroRace();
}

int EDispIntroRace_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EDispIntroRace();
    }
    return 0;
}

void EDispIntroRace_ResolveEvent_Callback(void *event, const UGroup *group) {
}
