#include "EDebugScreenMessage.hpp"

#include <new>

#include "Speed/Indep/Src/Lua/source/lua.h"

EDebugScreenMessage::EDebugScreenMessage(const char *pDebugMsg, float pDuration, int pX, int pY) : Event(0x20), fDebugMsg(EventManager::EmbedField(this, pDebugMsg)), fDuration(pDuration), fX(pX), fY(pY) {
}

EDebugScreenMessage::~EDebugScreenMessage() {
}

const char *EDebugScreenMessage::GetEventName() const {
    return "EDebugScreenMessage";
}

void EDebugScreenMessage_MakeEvent_Callback(const void *staticData) {
    new EDebugScreenMessage(((EDebugScreenMessage::StaticData *) staticData)->fDebugMsg, ((EDebugScreenMessage::StaticData *) staticData)->fDuration, ((EDebugScreenMessage::StaticData *) staticData)->fX, ((EDebugScreenMessage::StaticData *) staticData)->fY);
}

int EDebugScreenMessage_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 4) {
        new EDebugScreenMessage(lua_tostring(L, 1), lua_tonumber(L, 2), (int) lua_tonumber(L, 3), (int) lua_tonumber(L, 4));
    }
    return 0;
}

void EDebugScreenMessage_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EDebugScreenMessage::StaticData *) event)->fDebugMsg) CARP::TagReference(group);
}
