#include "ERandomEventList.hpp"

#include <new>

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Render/Common/RRandom.h"

ERandomEventList::ERandomEventList(CARP::EventList *pEventList) : Event(0x10), fEventList(pEventList) {
}

ERandomEventList::~ERandomEventList() {
    if (fEventList && fEventList->fNumEvents) {
        gEventDynamicData.Clear();

        unsigned int fireWhich = RRandom::IntRange(fEventList->fNumEvents);

        EventManager::FireOneEvent(fEventList, fireWhich, false);
    }
}

const char *ERandomEventList::GetEventName() const {
    return "ERandomEventList";
}

void ERandomEventList_MakeEvent_Callback(const void *staticData) {
    new ERandomEventList(((ERandomEventList::StaticData *) staticData)->fEventList);
}

int ERandomEventList_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ERandomEventList((CARP::EventList *) lua_tostring(L, 1));
    }
    return 0;
}

void ERandomEventList_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((ERandomEventList::StaticData *) event)->fEventList) CARP::TagReference(group);
}
