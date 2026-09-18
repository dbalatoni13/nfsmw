#include "EHideRaceOverMessage.hpp"

#include <new>

#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Lua/source/lua.h"

EHideRaceOverMessage::EHideRaceOverMessage(IPlayer *pPlayer) : Event(0x10), fPlayer(pPlayer) {
    IRaceOverMessage *iRaceOver;

    if (fPlayer->GetHud()->QueryInterface(&iRaceOver)) {
        iRaceOver->DismissRaceOverMessage();
    }
}

EHideRaceOverMessage::~EHideRaceOverMessage() {
}

const char *EHideRaceOverMessage::GetEventName() const {
    return "EHideRaceOverMessage";
}

void EHideRaceOverMessage_MakeEvent_Callback(const void *staticData) {
    new EHideRaceOverMessage(((EHideRaceOverMessage::StaticData *) staticData)->fPlayer);
}

int EHideRaceOverMessage_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EHideRaceOverMessage((IPlayer *) lua_tostring(L, 1));
    }
    return 0;
}

void EHideRaceOverMessage_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EHideRaceOverMessage::StaticData *) event)->fPlayer) CARP::TagReference(group);
}
