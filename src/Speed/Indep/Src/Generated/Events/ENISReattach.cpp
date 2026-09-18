#include "ENISReattach.hpp"

#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Animation/AnimPlayer.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IDamageable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"

ENISReattach::ENISReattach(int pRepairDamage, unsigned int phSimable) : Event(0x10), fRepairDamage(pRepairDamage), fhSimable(phSimable) {
}

ENISReattach::~ENISReattach() {
    ISimable *isimable = ISimable::FindInstance((HSIMABLE) fhSimable);

    if (isimable) {
        IVehicle *ivehicle;

        if (isimable->QueryInterface(&ivehicle)) {
            ivehicle->SetAnimating(true);
            ResetCarAnimState(ivehicle);

            if (fRepairDamage) {
                IDamageable *idamage;

                if (ivehicle->QueryInterface(&idamage)) {
                    idamage->ResetDamage();
                }
            }
        }
    }
}

const char *ENISReattach::GetEventName() const {
    return "ENISReattach";
}

void ENISReattach_MakeEvent_Callback(const void *staticData) {
    new ENISReattach(((ENISReattach::StaticData *) staticData)->fRepairDamage, gEventDynamicData.fhSimable);
}

int ENISReattach_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new ENISReattach((int) lua_tonumber(L, 1), gEventDynamicData.fhSimable);
    }
    return 0;
}

void ENISReattach_ResolveEvent_Callback(void *event, const UGroup *group) {
}
