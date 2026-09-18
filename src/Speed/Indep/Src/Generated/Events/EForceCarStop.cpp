#include "EForceCarStop.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

EForceCarStop::EForceCarStop(int pStopInstantly, unsigned int phSimable) : Event(0x10), fStopInstantly(pStopInstantly), fhSimable(phSimable) {
}

EForceCarStop::~EForceCarStop() {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        IVehicle *ivehicle;

        if (isimable->QueryInterface(&ivehicle)) {
            char bits = fStopInstantly ? 3 : 1;

            if (ivehicle) {
                ivehicle->ForceStopOn(bits);
            }
        }
    }
}

const char *EForceCarStop::GetEventName() const {
    return "EForceCarStop";
}

void EForceCarStop_MakeEvent_Callback(const void *staticData) {
    new EForceCarStop(((EForceCarStop::StaticData *) staticData)->fStopInstantly, gEventDynamicData.fhSimable);
}

int EForceCarStop_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EForceCarStop((int) lua_tonumber(L, 1), gEventDynamicData.fhSimable);
    }
    return 0;
}

void EForceCarStop_ResolveEvent_Callback(void *event, const UGroup *group) {
}
