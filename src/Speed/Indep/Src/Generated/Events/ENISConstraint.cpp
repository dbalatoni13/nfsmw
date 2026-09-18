#include "ENISConstraint.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/INISCarControl.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISConstraint::ENISConstraint(float pDegrees, unsigned int phSimable) : Event(0x10), fDegrees(pDegrees), fhSimable(phSimable) {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        INISCarControl *inis;

        if (isimable->QueryInterface(&inis)) {
            inis->SetConstraintAngle(fDegrees);
        }
    }
}

ENISConstraint::~ENISConstraint() {
}

const char *ENISConstraint::GetEventName() const {
    return "ENISConstraint";
}

void ENISConstraint_MakeEvent_Callback(const void *staticData) {
    new ENISConstraint(((ENISConstraint::StaticData *) staticData)->fDegrees, gEventDynamicData.fhSimable);
}

int ENISConstraint_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISConstraint(lua_tonumber(L, 1), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISConstraint_ResolveEvent_Callback(void *event, const UGroup *group) {
}
