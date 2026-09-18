#include "EEnableModeling.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EEnableModeling::EEnableModeling(int pEnable, unsigned int phSimable) : Event(0x10), fEnable(pEnable), fhSimable(phSimable) {
}

EEnableModeling::~EEnableModeling() {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        ICollisionBody *irbc;

        if (isimable->QueryInterface(&irbc)) {
            if (fEnable) {
                irbc->EnableModeling();
            } else {
                irbc->DisableModeling();
            }
        }
    }
}

const char *EEnableModeling::GetEventName() const {
    return "EEnableModeling";
}

void EEnableModeling_MakeEvent_Callback(const void *staticData) {
    new EEnableModeling(((EEnableModeling::StaticData *) staticData)->fEnable, gEventDynamicData.fhSimable);
}

int EEnableModeling_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EEnableModeling((int) lua_tonumber(L, 1), gEventDynamicData.fhSimable);
    }
    return 0;
}

void EEnableModeling_ResolveEvent_Callback(void *event, const UGroup *group) {
}
