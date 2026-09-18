#include "EEnableCollisionElement.hpp"

#include <new>

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Libs/Support/Miscellaneous/CARP.h"

EEnableCollisionElement::EEnableCollisionElement(int pEnable, CARP::CollisionObject *pColElement) : Event(0x10), fEnable(pEnable), fColElement(pColElement) {
}

EEnableCollisionElement::~EEnableCollisionElement() {
    if (fColElement) {
        if (fEnable) {
            fColElement->Enable();
        } else {
            fColElement->Disable();
        }
    }
}

const char *EEnableCollisionElement::GetEventName() const {
    return "EEnableCollisionElement";
}

void EEnableCollisionElement_MakeEvent_Callback(const void *staticData) {
    new EEnableCollisionElement(((EEnableCollisionElement::StaticData *) staticData)->fEnable, ((EEnableCollisionElement::StaticData *) staticData)->fColElement);
}

int EEnableCollisionElement_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new EEnableCollisionElement((int) lua_tonumber(L, 1), (CARP::CollisionObject *) lua_tostring(L, 2));
    }
    return 0;
}

void EEnableCollisionElement_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EEnableCollisionElement::StaticData *) event)->fColElement) CARP::TagReference(group);
}
