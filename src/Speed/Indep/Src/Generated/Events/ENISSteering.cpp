#include "ENISSteering.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISSteering::ENISSteering(float pDegrees, float pBlendWeight, unsigned int phSimable) : Event(0x20), fDegrees(pDegrees), fBlendWeight(pBlendWeight), fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        INISCarControl *inis;

        if (isimable->QueryInterface(&inis)) {
            inis->SetSteering(fDegrees / 360.0f, fBlendWeight);
        }
    }
}

ENISSteering::~ENISSteering() {
}

const char *ENISSteering::GetEventName() const {
    return "ENISSteering";
}

void ENISSteering_MakeEvent_Callback(const void *staticData) {
    new ENISSteering(((ENISSteering::StaticData *) staticData)->fDegrees, ((ENISSteering::StaticData *) staticData)->fBlendWeight, gEventDynamicData.fhSimable);
}

int ENISSteering_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 2) {
        new ENISSteering(lua_tonumber(L, 1), lua_tonumber(L, 2), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISSteering_ResolveEvent_Callback(void *event, const UGroup *group) {
}
