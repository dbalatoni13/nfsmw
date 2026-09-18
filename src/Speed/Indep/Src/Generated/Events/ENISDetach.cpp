#include "ENISDetach.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISDetach::ENISDetach(float pSpeed, unsigned int phSimable) : Event(0x10), fSpeed(pSpeed), fhSimable(phSimable) {
}

ENISDetach::~ENISDetach() {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        IVehicle *ivehicle;

        if (isimable->QueryInterface(&ivehicle)) {
            ivehicle->SetAnimating(false);
        }
    }
}

const char *ENISDetach::GetEventName() const {
    return "ENISDetach";
}

void ENISDetach_MakeEvent_Callback(const void *staticData) {
    new ENISDetach(((ENISDetach::StaticData *) staticData)->fSpeed, gEventDynamicData.fhSimable);
}

int ENISDetach_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISDetach(lua_tonumber(L, 1), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISDetach_ResolveEvent_Callback(void *event, const UGroup *group) {
}
