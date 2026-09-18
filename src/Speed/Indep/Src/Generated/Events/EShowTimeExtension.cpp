#include "EShowTimeExtension.hpp"

#include <new>

#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

EShowTimeExtension::EShowTimeExtension(IPlayer *pPlayer, float pTimeToShow) : Event(0x10), fPlayer(pPlayer), fTimeToShow(pTimeToShow) {
}

EShowTimeExtension::~EShowTimeExtension() {
    ITimeExtension *itimeextension;

    if (fPlayer->GetHud()->QueryInterface(&itimeextension)) {
        itimeextension->RequestTimeExtensionMessage(fPlayer, fTimeToShow);
    }
}

const char *EShowTimeExtension::GetEventName() const {
    return "EShowTimeExtension";
}

void EShowTimeExtension_MakeEvent_Callback(const void *staticData) {
    new EShowTimeExtension(((EShowTimeExtension::StaticData *) staticData)->fPlayer, ((EShowTimeExtension::StaticData *) staticData)->fTimeToShow);
}

int EShowTimeExtension_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new EShowTimeExtension((IPlayer *) lua_tostring(L, 1), lua_tonumber(L, 2));
    }
    return 0;
}

void EShowTimeExtension_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EShowTimeExtension::StaticData *) event)->fPlayer) CARP::TagReference(group);
}
